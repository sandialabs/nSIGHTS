///////////////////////////////////////////////////////////////////////////////////
//
// PS_WriteTGA.cpp
//
///////////////////////////////////////////////////////////////////////////////////
//
// Copyright 2012 Sandia Corporation. Under the terms of Contract DE-AC04-94AL85000 with Sandia Corporation, the U.S. Government retains certain rights in this software.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
//    * Redistributions of source code must retain the above copyright
//      notice, this list of conditions and the following disclaimer.
//    * Redistributions in binary form must reproduce the above copyright
//      notice, this list of conditions and the following disclaimer in the
//      documentation and/or other materials provided with the distribution.
//    * Neither the name of Sandia Corporation nor the
//      names of its contributors may be used to endorse or promote products
//      derived from this software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
// ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
// WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
// DISCLAIMED. IN NO EVENT SHALL SANDIA CORPORATION BE LIABLE FOR ANY
// DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
// (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
// LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
// ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
// SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//
///////////////////////////////////////////////////////////////////////////////////
//
// DESCRIPTION: Platform specific but application independent code.
//              Writes plot bitmaps in TGA format (plus many other features below)
//
///////////////////////////////////////////////////////////////////////////////////


#include "GenLibAfx.h"

#include <stdio.h>
#include <assert.h>

#include <genClass/U_Msg.h>
#include <genClass/U_Str.h>

#include "PS_WriteTGA.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#ifdef TGAIMAGE

//  most code below scooped from ImageMagick 4.0.6
//  and slightly modified (replace malloc with new, reduce complexity)

#define MaxTextExtent   255
#define False           0
#define True            1

#if defined(QuantumLeap)
/*
  Color quantum is [0..65535].
*/
#define DownScale(quantum)  (((unsigned int) (quantum)) >> 8)
#define MaxRGB          65535L
#define MaxRunlength    65535L
#define Opaque          65535L
#define QuantumDepth    16
#define UpScale(quantum)    (((unsigned int) (quantum))*257)
#define XDownScale(color)   ((unsigned int) (color))
#define XUpScale(color)     ((unsigned int) (color))

typedef unsigned short Quantum;
#else
/*
  Color quantum is [0..255].
*/
#define DownScale(quantum)  ((unsigned int) (quantum))
#define MaxRGB          255
#define MaxRunlength    255
#define Opaque          255
#define QuantumDepth    8
#define UpScale(quantum)    ((unsigned int) (quantum))
#define XDownScale(color)   (((unsigned int) (color)) >> 8)
#define XUpScale(color)     (((unsigned int) (color))*257)

typedef unsigned char Quantum;
#endif

#define Extent(string)  ((int) StringLength(string))

#define XDestroyImage(ximage) \
    ((*((ximage)->f.destroy_image))((ximage)))
#define XGetPixel(ximage, x, y) \
    ((*((ximage)->f.get_pixel))((ximage), (x), (y)))

#define ColorMatch(color,target,delta) \
  ((((int) ((color).red)-delta) <= (int) ((target).red)) && \
    ((int) ((target).red) <= ((int) ((color).red)+delta)) && \
   (((int) ((color).green)-delta) <= (int) ((target).green)) && \
    ((int) ((target).green) <= ((int) ((color).green)+delta)) && \
   (((int) ((color).blue)-delta) <= (int) ((target).blue)) && \
    ((int) ((target).blue) <= ((int) ((color).blue)+delta)))

#define Min(x,y)  (((x) < (y)) ? (x) : (y))



typedef enum
{
  UndefinedCompression,
  NoCompression,
  RunlengthEncodedCompression
} CompressionType;


typedef enum
{
  UndefinedClass,
  DirectClass,
  PseudoClass
} ClassType;


typedef struct _ColorPacket
{
  Quantum
    red,
    green,
    blue;

  unsigned char
    flags;

  char
    key[3];

  unsigned short
    index;
} ColorPacket;

typedef struct _RunlengthPacket
{
  Quantum
    red,
    green,
    blue,
    length;

  unsigned short
    index;
} RunlengthPacket;

typedef struct _RectangleInfo
{
  unsigned int
    width,
    height;

  int
    x,
    y;
} RectangleInfo;


typedef struct _ImageInfo
{
  char              filename[MaxTextExtent];
  CompressionType   compression;

} ImageInfo;

typedef struct _Image
{
    FILE*   file;
    int     status;
    char    filename[MaxTextExtent];

    ClassType   c_class;

    CompressionType compression;

  unsigned int
    columns,
    rows,
    depth;


  unsigned int
    scene,
    number_scenes;

  char
    *montage,
    *directory;

  ColorPacket
    *colormap;

  unsigned int
    colors;

  unsigned int
    mean_error_per_pixel;

  double
    normalized_mean_error,
    normalized_maximum_error;

  unsigned long
    total_colors;

  char
    *signature;

  RunlengthPacket
    *pixels,
    *packet;

  unsigned long
    packets;

  unsigned int
    runlength,
    packet_size;

  unsigned char
    *packed_pixels;

  char
    *geometry,
    *page;

  unsigned int
    dispose,
    delay,
    iterations;

  unsigned int
    orphan;

  struct _Image
    *previous,
    *list,
    *next;
} Image;

#ifndef _WIN32
/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   X E r r o r                                                               %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Function XError ignores BadWindow errors for XQueryTree and
%  XGetWindowAttributes, and ignores BadDrawable errors for XGetGeometry, and
%  ignores BadValue errors for XQueryColor.  It returns False in those cases.
%  Otherwise it returns True.
%
%  The format of the XError function is:
%
%      XError(display,error)
%
%  A description of each parameter follows:
%
%    o display: Specifies a pointer to the Display structure;  returned from
%      XOpenDisplay.
%
%    o error: Specifies the error event.
%
%
*/

static volatile unsigned int
  xerror_alert = False;

static int XError(Display *display,XErrorEvent *error)
{
  assert(display != (Display *) NULL);
  assert(error != (XErrorEvent *) NULL);
  xerror_alert=True;
  switch (error->request_code)
  {
    case X_GetGeometry:
    {
      if (error->error_code == BadDrawable)
        return(False);
      break;
    }
    case X_GetWindowAttributes:
    case X_QueryTree:
    {
      if (error->error_code == BadWindow)
        return(False);
      break;
    }
    case X_QueryColors:
    {
      if (error->error_code == BadValue)
        return(False);
      break;
    }
  }
  return(True);
}

#endif


/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   G e t I m a g e I n f o                                                   %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Function GetImageInfo initializes the ImageInfo structure.
%
%  The format of the GetImageInfo routine is:
%
%      GetImageInfo(image_info)
%
%  A description of each parameter follows:
%
%    o image_info: Specifies a pointer to a ImageInfo structure.
%
%
*/
static void GetImageInfo(ImageInfo *image_info)
{
  assert(image_info != (ImageInfo *) NULL);
  image_info->filename[0] = '\0';
  image_info->compression = UndefinedCompression;
}


//  prototypes

static void     DestroyImage(Image *image);
static Image*   AllocateImage(const ImageInfo *image_info);
static Image*   CloneImage(               Image*  image,
                            const unsigned int     columns,
                            const unsigned int     rows,
                            const unsigned int     clone_pixels);
static void     CondenseImage(Image *image);
static Image*   CropImage(Image *image, RectangleInfo *crop_info);




/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   A l l o c a t e I m a g e                                                 %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Function AllocateImage allocates an Image structure and initializes each
%  field to a default value.
%
%  The format of the AllocateImage routine is:
%
%      allocated_image=AllocateImage(image_info)
%
%  A description of each parameter follows:
%
%    o allocated_image: Function AllocateImage returns a pointer to an image
%      structure initialized to default values.  A null image is returned if
%      there is a memory shortage.
%
%    o image_info: Specifies a pointer to a ImageInfo structure.
%
%
*/
static Image *AllocateImage(const ImageInfo *image_info)
{
    //  Allocate image structure.
    Image* allocated_image = new Image;
    if (allocated_image == 0)
    {
        GenAppWarningMsg("PS_WriteTGA::AllocateImage","Memory allocation failed");
        return((Image *) NULL);
    }

    //Initialize Image structure.

    allocated_image->file=(FILE *) NULL;
    allocated_image->status=False;
    *allocated_image->filename='\0';
    allocated_image->c_class=DirectClass;
    allocated_image->compression=RunlengthEncodedCompression;
    allocated_image->columns=0;
    allocated_image->rows=0;
    allocated_image->depth=QuantumDepth;
    allocated_image->scene=0;
    allocated_image->number_scenes=1;
    allocated_image->montage=(char *) NULL;
    allocated_image->directory=(char *) NULL;
    allocated_image->colormap=(ColorPacket *) NULL;
    allocated_image->colors=0;
    allocated_image->normalized_maximum_error=0.0;
    allocated_image->normalized_mean_error=0.0;
    allocated_image->mean_error_per_pixel=0;
    allocated_image->total_colors=0;
    allocated_image->signature=(char *) NULL;
    allocated_image->pixels=(RunlengthPacket *) NULL;
    allocated_image->packet=(RunlengthPacket *) NULL;
    allocated_image->packets=0;
    allocated_image->packet_size=0;
    allocated_image->packed_pixels=(unsigned char *) NULL;
    allocated_image->geometry=(char *) NULL;
    allocated_image->page=(char *) NULL;
    allocated_image->dispose=0;
    allocated_image->delay=0;
    allocated_image->iterations=1;

    if (image_info != (ImageInfo *) NULL)
    {
      CopyString(allocated_image->filename, image_info->filename, MaxTextExtent);
    }
    allocated_image->orphan=False;
    allocated_image->previous=(Image *) NULL;
    allocated_image->list=(Image *) NULL;
    allocated_image->next=(Image *) NULL;

    return(allocated_image);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   C l o n e I m a g e                                                       %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Function CloneImage returns a copy of all fields of the input image.  The
%  the pixel memory is allocated but the pixel data is not copied.
%
%  The format of the CloneImage routine is:
%
%      clone_image=CloneImage(image,columns,rows,clone_pixels)
%
%  A description of each parameter follows:
%
%    o clone_image: Function CloneImage returns a pointer to the image after
%      copying.  A null image is returned if there is a memory shortage.
%
%    o image: The address of a structure of type Image.
%
%    o columns: An integer that specifies the number of columns in the copied
%      image.
%
%    o rows: An integer that specifies the number of rows in the copied
%      image.
%
%    o clone_pixels: Specifies whether the pixel data is copied.  Must be
%      either True or False;
%
%
*/

#pragma warning( push )
#pragma warning(disable : 4018 ) // 'signed/unsigned mismatch

static Image *CloneImage(               Image*  image,
                         const unsigned int     columns,
                         const unsigned int     rows,
                         const unsigned int     clone_pixels)
{
  register int
    i;

  /*
    Allocate image structure.
  */
  assert(image != (Image *) NULL);
  Image* clone_image = new Image;
  if (clone_image == (Image *) NULL)
    return((Image *) NULL);
  /*
    Allocate the image pixels.
  */
  *clone_image=(*image);
  clone_image->columns=columns;
  clone_image->rows=rows;
  if (clone_pixels)
    clone_image->pixels =   new RunlengthPacket[image->packets];
  else
    {
      clone_image->packets=clone_image->columns*clone_image->rows;
      clone_image->pixels= new RunlengthPacket[clone_image->packets];
    }
  if (clone_image->pixels == (RunlengthPacket *) NULL)
    return((Image *) NULL);

  if (clone_pixels)
    {
      register RunlengthPacket
        *p,
        *q;

      //         Copy image pixels.
      p=image->pixels;
      q=clone_image->pixels;
      for (i=0; i < image->packets; i++)
      {
        *q=(*p);
        p++;
        q++;
      }
    }
  clone_image->packed_pixels=(unsigned char *) NULL;
  if (image->colormap != (ColorPacket *) NULL)
    {
      //  Allocate and copy the image colormap.
      clone_image->colormap = new ColorPacket[image->colors];
      if (clone_image->colormap == (ColorPacket *) NULL)
        return((Image *) NULL);
      for (i=0; i < image->colors; i++)
        clone_image->colormap[i]=image->colormap[i];
    }
  if (image->signature != (char *) NULL)
    {
      //  Allocate and copy the image signature.
      clone_image->signature = NULL;
      AllocAndCopyString(clone_image->signature, image->signature);
    }
  clone_image->page=(char *) NULL;
  if ((image->columns == columns) && (image->rows == rows))
    if (image->page != (char *) NULL)
      {
        //  Allocate and copy the image page.
        AllocAndCopyString(clone_image->page, image->page);
      }

  clone_image->montage=(char *) NULL;
  if ((image->columns == columns) && (image->rows == rows))
    if (image->montage != (char *) NULL)
      {
        AllocAndCopyString(clone_image->montage, image->montage);
      }

  clone_image->directory=(char *) NULL;
  if ((image->columns == columns) && (image->rows == rows))
    if (image->directory != (char *) NULL)
      {
        AllocAndCopyString(clone_image->directory, image->directory);
      }
  if (image->orphan)
    {
      clone_image->file=(FILE *) NULL;
      clone_image->previous=(Image *) NULL;
      clone_image->next=(Image *) NULL;
    }
  else
    {
      //  Link image into image list.
      if (clone_image->previous != (Image *) NULL)
        clone_image->previous->next=clone_image;
      if (clone_image->next != (Image *) NULL)
        clone_image->next->previous=clone_image;
    }
  clone_image->orphan=False;
  return(clone_image);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   C o n d e n s e I m a g e                                                 %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Function CondenseImage compresses an image to the minimum number of
%  runlength-encoded packets.
%
%  The format of the CondenseImage routine is:
%
%      CondenseImage(image)
%
%  A description of each parameter follows:
%
%    o image: The address of a structure of type Image.
%
%
*/
static void CondenseImage(Image *image)
{
  register int
    i;

  register RunlengthPacket
    *p,
    *q;

  /*
    Compress image.
  */
  assert(image != (Image *) NULL);
  if (image == (Image *) NULL)
    return;
  p=image->pixels;
  image->runlength=p->length+1;
  image->packets=0;
  q=image->pixels;
  q->length=MaxRunlength;

  for (i=0; i < (image->columns*image->rows); i++)
  {
    if (image->runlength != 0)
      image->runlength--;
    else
      {
        p++;
        image->runlength=p->length;
      }
    if ((p->red == q->red) && (p->green == q->green) &&
        (p->blue == q->blue) && ((int) q->length < MaxRunlength))
      q->length++;
    else
      {
        if (image->packets != 0)
          q++;
        image->packets++;
        *q=(*p);
        q->length=0;
      }
  }

//  realloc commented out -- not needed (i think)
//  image->pixels=(RunlengthPacket *)
//    realloc((char *) image->pixels,image->packets*sizeof(RunlengthPacket));

//    Runlength-encode only if it takes up less space than no compression.
  if (image->c_class == DirectClass)
    {
      if (image->packets >= ((image->columns*image->rows*3) >> 2))
        image->compression=NoCompression;
      return;
    }
  if (image->packets >= ((image->columns*image->rows) >> 1))
    image->compression=NoCompression;
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   C r o p I m a g e                                                         %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Function CropImage creates a new image that is a subregion of an existing
%  one.  It allocates the memory necessary for the new Image structure and
%  returns a pointer to the new image.  This routine is optimized to perserve
%  the runlength encoding.  That is, the cropped image will always use less
%  memory than the original.
%
%  The format of the CropImage routine is:
%
%      cropped_image=CropImage(image,crop_info)
%
%  A description of each parameter follows:
%
%    o cropped_image: Function CropImage returns a pointer to the cropped
%      image.  A null image is returned if there is a a memory shortage or
%      if the image width or height is zero.
%
%    o image: The address of a structure of type Image.
%
%    o crop_info: Specifies a pointer to a RectangleInfo which defines the
%      region of the image to crop.
%
%
*/


static Image *CropImage(Image *image, RectangleInfo *crop_info)
{
  Image
    *cropped_image;

  int
    x,
    y;

  register RunlengthPacket
    *p,
    *q;

  //  Check crop geometry.
  assert(image != (Image *) NULL);
  assert(crop_info != (RectangleInfo *) NULL);
  if (((crop_info->x+(int) crop_info->width) < 0) ||
      ((crop_info->y+(int) crop_info->height) < 0) ||
      (crop_info->x > (int) image->columns) ||
      (crop_info->y > (int) image->rows))
    {
        GenAppWarningMsg("PS_WriteTGA::CropImage", "geometry does not contain image");
        return((Image *) NULL);
    }
  if ((crop_info->x+(int) crop_info->width) > (int) image->columns)
    crop_info->width=(unsigned int) ((int) image->columns-crop_info->x);
  if ((crop_info->y+(int) crop_info->height) > (int) image->rows)
    crop_info->height=(unsigned int) ((int) image->rows-crop_info->y);
  if (crop_info->x < 0)
    {
      crop_info->width-=(unsigned int) (-crop_info->x);
      crop_info->x=0;
    }
  if (crop_info->y < 0)
    {
      crop_info->height-=(unsigned int) (-crop_info->y);
      crop_info->y=0;
    }
  if ((crop_info->width == 0) && (crop_info->height == 0))
    {
      int
        x_border,
        y_border;

      register int
        i;

      RunlengthPacket
        corners[4];

      //    Set bounding box to the image dimensions.
      x_border=crop_info->x;
      y_border=crop_info->y;
      crop_info->width=0;
      crop_info->height=0;
      crop_info->x=image->columns;
      crop_info->y=image->rows;
      p=image->pixels;
      image->runlength=p->length+1;
      corners[0]=(*p);
      for (i=1; i <= (image->rows*image->columns); i++)
      {
        if (image->runlength != 0)
          image->runlength--;
        else
          {
            p++;
            image->runlength=p->length;
          }
        if (i == image->columns)
          corners[1]=(*p);
        if (i == (image->rows*image->columns-image->columns+1))
          corners[2]=(*p);
        if (i == (image->rows*image->columns))
          corners[3]=(*p);
      }
      p=image->pixels;
      image->runlength=p->length+1;
      for (y=0; y < image->rows; y++)
      {
        for (x=0; x < image->columns; x++)
        {
          if (image->runlength != 0)
            image->runlength--;
          else
            {
              p++;
              image->runlength=p->length;
            }
          if (!ColorMatch(*p,corners[0],0))
            if (x < crop_info->x)
              crop_info->x=x;
          if (!ColorMatch(*p,corners[1],0))
            if (x > crop_info->width)
              crop_info->width=x;
          if (!ColorMatch(*p,corners[0],0))
            if (y < crop_info->y)
              crop_info->y=y;
          if (!ColorMatch(*p,corners[2],0))
            if (y > crop_info->height)
              crop_info->height=y;
        }
      }
      if ((crop_info->width != 0) || (crop_info->height != 0))
        {
          crop_info->width-=crop_info->x-1;
          crop_info->height-=crop_info->y-1;
        }
      crop_info->width+=x_border*2;
      crop_info->height+=y_border*2;
      crop_info->x-=x_border;
      if (crop_info->x < 0)
        crop_info->x=0;
      crop_info->y-=y_border;
      if (crop_info->y < 0)
        crop_info->y=0;
    }
  if ((crop_info->width == 0) || (crop_info->height == 0))
    {
      GenAppWarningMsg("PS_WriteTGA::CropImage","geometry dimensions are zero");
      return((Image *) NULL);
    }
  if ((crop_info->width == image->columns) &&
      (crop_info->height == image->rows) && (crop_info->x == 0) &&
      (crop_info->y == 0))
    return((Image *) NULL);

    //    Initialize cropped image attributes.
  cropped_image=CloneImage(image,crop_info->width,crop_info->height,True);
  if (cropped_image == (Image *) NULL)
    {
      GenAppWarningMsg("PS_WriteTGA::CropImage", "Memory allocation failed");
      return((Image *) NULL);
    }

    //    Skip pixels up to the cropped image.
  p=image->pixels;
  image->runlength=p->length+1;
  for (x=0; x < (crop_info->y*image->columns+crop_info->x); x++)
    if (image->runlength != 0)
      image->runlength--;
    else
      {
        p++;
        image->runlength=p->length;
      }

    //    Extract cropped image.
  cropped_image->packets=0;
  q=cropped_image->pixels;
  q->red=0;
  q->green=0;
  q->blue=0;
  q->index=0;
  q->length=MaxRunlength;
  for (y=0; y < (cropped_image->rows-1); y++)
  {
    // Transfer scanline.
    for (x=0; x < cropped_image->columns; x++)
    {
      if (image->runlength != 0)
        image->runlength--;
      else
        {
          p++;
          image->runlength=p->length;
        }
      if ((p->red == q->red) && (p->green == q->green) &&
          (p->blue == q->blue) && (p->index == q->index) &&
          ((int) q->length < MaxRunlength))
        q->length++;
      else
        {
          if (cropped_image->packets != 0)
            q++;
          cropped_image->packets++;
          *q=(*p);
          q->length=0;
        }
    }
    //   Skip to next scanline.
    for (x=0; x < (int) (image->columns-cropped_image->columns); x++)
      if (image->runlength != 0)
        image->runlength--;
      else
        {
          p++;
          image->runlength=p->length;
        }

  }
  //   Transfer last scanline.
  for (x=0; x < cropped_image->columns; x++)
  {
    if (image->runlength != 0)
      image->runlength--;
    else
      {
        p++;
        image->runlength=p->length;
      }
    if ((p->red == q->red) && (p->green == q->green) &&
        (p->blue == q->blue) && (p->index == q->index) &&
        ((int) q->length < MaxRunlength))
      q->length++;
    else
      {
        if (cropped_image->packets != 0)
          q++;
        cropped_image->packets++;
        *q=(*p);
        q->length=0;
      }
  }

//  realloc commented out -- not needed (i think)
//  cropped_image->pixels=(RunlengthPacket *) realloc((char *)
//  cropped_image->pixels,cropped_image->packets*sizeof(RunlengthPacket));
  return(cropped_image);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   F l o p I m a g e                                                         %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Function FlopImage creates a new image that reflects each scanline in the
%  horizontal direction It allocates the memory necessary for the new Image
%  structure and returns a pointer to the new image.
%
%  The format of the FlopImage routine is:
%
%      flopped_image=FlopImage(image)
%
%  A description of each parameter follows:
%
%    o flopped_image: Function FlopImage returns a pointer to the image
%      after reflecting.  A null image is returned if there is a memory
%      shortage.
%
%    o image: The address of a structure of type Image.
%
%
*/
static Image *FlopImage(Image *image)
{
  Image
    *flopped_image;

  register RunlengthPacket
    *p,
    *q,
    *s;

  register unsigned int
    x,
    y;

  RunlengthPacket
    *scanline;

  //    Initialize flopped image attributes.
  assert(image != (Image *) NULL);
  flopped_image=CloneImage(image,image->columns,image->rows,False);
  if (flopped_image == (Image *) NULL)
    {
      GenAppWarningMsg("PS_WriteTGA::FlopImage", "Memory allocation failed");
      return((Image *) NULL);
    }
  //    Allocate scan line buffer and column offset buffers.
  scanline = new RunlengthPacket[image->columns];
  if (scanline == (RunlengthPacket *) NULL)
    {
      GenAppWarningMsg("PS_WriteTGA::FlopImage", "Memory allocation failed");
      DestroyImage(flopped_image);
      return((Image *) NULL);
    }
  //   Flop each row.
  p=image->pixels;
  image->runlength=p->length+1;
  q=flopped_image->pixels;
  for (y=0; y < flopped_image->rows; y++)
  {
    //   Read a scan line.
    s=scanline;
    for (x=0; x < image->columns; x++)
    {
      if (image->runlength != 0)
        image->runlength--;
      else
        {
          p++;
          image->runlength=p->length;
        }
      *s=(*p);
      s++;
    }
    //  Flop each column.
    s=scanline+image->columns;
    for (x=0; x < flopped_image->columns; x++)
    {
      s--;
      *q=(*s);
      q->length=0;
      q++;
    }
  }
  delete [] scanline;
  return(flopped_image);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   S y n c I m a g e                                                         %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Function SyncImage initializes the red, green, and blue intensities of each
%  pixel as defined by the colormap index.
%
%  The format of the SyncImage routine is:
%
%      SyncImage(image)
%
%  A description of each parameter follows:
%
%    o image: The address of a structure of type Image.
%
%
*/
static void SyncImage(Image *image)
{
  register int
    i;

  register RunlengthPacket
    *p;

  register unsigned short
    index;

  assert(image != (Image *) NULL);
  if (image->c_class == DirectClass)
    return;
  for (i=0; i < image->colors; i++)
  {
    image->colormap[i].index=0;
    image->colormap[i].flags=0;
  }
  p=image->pixels;
  for (i=0; i < image->packets; i++)
  {
    index=p->index;
    p->red=image->colormap[index].red;
    p->green=image->colormap[index].green;
    p->blue=image->colormap[index].blue;
    p++;
  }
}


/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   C l o s e I m a g e                                                       %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Function CloseImage closes a file associated with the image.
%  The format of the CloseImage routine is:
%
%      CloseImage(image)
%
%  A description of each parameter follows:
%
%    o image: The address of a structure of type Image.
%
%
*/

static void CloseImage(Image *image)
{
    //  Close image file.
    assert(image != (Image *) NULL);
    if (image->file == (FILE *) NULL)
        return;
    image->status=ferror(image->file);
    (void) fclose(image->file);
    image->file=(FILE *) NULL;

    // ???
    if (!image->orphan)
        do
        {
            image->file=(FILE *) NULL;
            image=image->next;
        }
        while (image != (Image *) NULL);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   D e s t r o y I m a g e                                                   %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Function DestroyImage deallocates memory associated with an image.
%
%  The format of the DestroyImage routine is:
%
%      DestroyImage(image)
%
%  A description of each parameter follows:
%
%    o image: The address of a structure of type Image.
%
%
*/
static void DestroyImage(Image *image)
{
    //   Close image.
    assert(image != (Image *) NULL);
    if (image->file != (FILE *) NULL)
    {
        CloseImage(image);
    }
    // Deallocate the image montage directory.
    if (image->montage != (char *) NULL)
        delete [] image->montage;
    if (image->directory != (char *) NULL)
        delete [] image->directory;
    //  Deallocate the image colormap.
    if (image->colormap != (ColorPacket *) NULL)
        delete [] image->colormap;
    //  Deallocate the image signature.
    if (image->signature != (char *) NULL)
        delete [] image->signature;
    //  Deallocate the image pixels.
    if (image->pixels != (RunlengthPacket *) NULL)
        delete [] image->pixels;
    if (image->packed_pixels != (unsigned char *) NULL)
        delete [] image->packed_pixels;
    // Deallocate the image page geometry.
    if (image->page != (char *) NULL)
        delete [] image->page;
    if (!image->orphan)
    {
        // Unlink from linked list.
        if (image->previous != (Image *) NULL)
            if (image->next != (Image *) NULL)
                image->previous->next=image->next;
            else
            image->previous->next=(Image *) NULL;

        if (image->next != (Image *) NULL)
            if (image->previous != (Image *) NULL)
                image->next->previous=image->previous;
            else
            image->next->previous=(Image *) NULL;
    }
    //  Deallocate the image structure.
    delete image;
    image = (Image *) NULL;
}

#ifndef _WIN32

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   X G e t W i n d o w I m a g e                                             %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Function XGetWindowImage reads an image from the target X window and returns
%  it.  XGetWindowImage optionally descends the window hierarchy and overlays
%  the target image with each child image in an optimized fashion.  Any child
%  window that have the same visual, colormap, and are contained by its
%  parent are exempted.
%
%  The format of the XGetWindowImage routine is:
%
%      image=XGetWindowImage(display,window)
%
%  A description of each parameter follows:
%
%    o image: Function XGetWindowImage returns a MIFF image if it can be
%      successfully read from the X window.  A null image is returned if
%      any errors occurs.
%
%    o display: Specifies a connection to an X server;  returned from
%      XOpenDisplay.
%
%    o window: Specifies the window to obtain the image from.
%
%
*/
static Image*   XGetWindowImage(      Display*    display,
                                const Window      window)
{
    typedef struct _ColormapInfo
    {
        Colormap    colormap;
        XColor*     colors;

        struct _ColormapInfo*   next;

    } ColormapInfo;

    typedef struct _WindowInfo
    {
        Window          window, parent;
        Visual          *visual;
        Colormap        colormap;
        XSegment        bounds;
        RectangleInfo   crop_info;
    } WindowInfo;

    int display_height, display_width, status, x_offset, y_offset;
    RectangleInfo crop_info;


    Window    child, root_window;

    XWindowAttributes window_attributes;

    // Verify window is viewable.

    assert(display != (Display *) NULL);
    status=XGetWindowAttributes(display,window,&window_attributes);
    if ((status == False) || (window_attributes.map_state != IsViewable))
        return((Image *) NULL);

    //  Cropping rectangle is relative to root window.
    root_window=XRootWindow(display, XDefaultScreen(display));
    XTranslateCoordinates(display, window, root_window, 0, 0, &x_offset, &y_offset, &child);

    crop_info.x=x_offset;
    crop_info.y=y_offset;
    crop_info.width=window_attributes.width;
    crop_info.height=window_attributes.height;

    // Crop to root window.
    if (crop_info.x < 0)
    {
      crop_info.width+=crop_info.x;
      crop_info.x=0;
    }
    if (crop_info.y < 0)
    {
      crop_info.height+=crop_info.y;
      crop_info.y=0;
    }

    //  crop to onscreen
    display_width=XDisplayWidth(display, XDefaultScreen(display));
    if ((crop_info.x+(int) crop_info.width) > display_width)
        crop_info.width=display_width-crop_info.x;
    display_height=XDisplayHeight(display,XDefaultScreen(display));
    if ((crop_info.y+(int) crop_info.height) > display_height)
        crop_info.height=display_height-crop_info.y;

    WindowInfo       window_info;

    window_info.window=window;
    window_info.visual=window_attributes.visual;
    window_info.colormap=window_attributes.colormap;
    window_info.bounds.x1=crop_info.x;
    window_info.bounds.y1=crop_info.y;
    window_info.bounds.x2=crop_info.x+(int) crop_info.width-1;
    window_info.bounds.y2=crop_info.y+(int) crop_info.height-1;
    crop_info.x-=x_offset;
    crop_info.y-=y_offset;
    window_info.crop_info=crop_info;

    Image*          composite_image;
    Image*          image;

    int                 i, x, y;
    RunlengthPacket*    p;

    unsigned long       pixel;
    unsigned int        number_colors;

    XColor*             colors;

    image = (Image *) NULL;

    //   Get X image.
    XImage* ximage = XGetImage(display, window_info.window,
                     window_info.crop_info.x, window_info.crop_info.y,
                     window_info.crop_info.width,window_info.crop_info.height,
                     AllPlanes,ZPixmap);

    if (ximage == (XImage *) NULL)
    {
        XDestroyImage(ximage);
        return((Image *) NULL);
    }

    // Initialize window colormap.
    number_colors = 0;
    colors = (XColor *) NULL;
    ColormapInfo     colormap_info;
    colormap_info.colormap = window_info.colormap;

    if (window_info.colormap != (Colormap) NULL)
    {
        number_colors=window_info.visual->map_entries;
        colors= new XColor[number_colors];
        if (colors == (XColor *) NULL)
        {
            XDestroyImage(ximage);
            return((Image *) NULL);
        }

        if ((window_info.visual->c_class != DirectColor) && (window_info.visual->c_class != TrueColor))
            for (i=0; i < number_colors; i++)
            {
                colors[i].pixel=i;
                colors[i].pad=0;
            }
        else
        {
            unsigned long   blue,   blue_bit,
                            green,  green_bit,
                            red,    red_bit;

            //  DirectColor or TrueColor visual.
            red   = 0;
            green = 0;
            blue  = 0;
            red_bit   = window_info.visual->red_mask &
                            (~(window_info.visual->red_mask)+1);
            green_bit = window_info.visual->green_mask &
                            (~(window_info.visual->green_mask)+1);
            blue_bit  = window_info.visual->blue_mask &
                            (~(window_info.visual->blue_mask)+1);
            for (i=0; i < number_colors; i++)
            {
                colors[i].pixel=red | green | blue;
                colors[i].pad=0;
                red+=red_bit;
                if (red > window_info.visual->red_mask)
                    red=0;
                green+=green_bit;
                if (green > window_info.visual->green_mask)
                    green=0;
                blue+=blue_bit;
                if (blue > window_info.visual->blue_mask)
                    blue=0;
            }
        }

        XQueryColors(display,window_info.colormap, colors, (int) number_colors);
        colormap_info.colors   =   colors;
    }

    //  Allocate image structure.
    composite_image=AllocateImage((ImageInfo *) NULL);
    if (composite_image == (Image *) NULL)
    {
        XDestroyImage(ximage);
        return((Image *) NULL);
    }

    //  Convert X image to MIFF format.
    if ((window_info.visual->c_class != TrueColor) &&
        (window_info.visual->c_class != DirectColor))
          composite_image->c_class=PseudoClass;

    composite_image->columns=ximage->width;
    composite_image->rows=ximage->height;
    composite_image->packets=composite_image->columns*composite_image->rows;
    composite_image->pixels= new RunlengthPacket[composite_image->packets];

    if (composite_image->pixels == (RunlengthPacket *) NULL)
    {
        XDestroyImage(ximage);
        DestroyImage(composite_image);
        return((Image *) NULL);
    }

    p=composite_image->pixels;
    switch (composite_image->c_class)
    {
        case DirectClass:
        default:
        {
            unsigned long   color, index;
            unsigned long   blue_mask,  blue_shift,
                            green_mask, green_shift,
                            red_mask,   red_shift;

            // Determine shift and mask for red, green, and blue.

            red_mask=window_info.visual->red_mask;
            red_shift=0;
            while ((red_mask & 0x01) == 0)
            {
              red_mask>>=1;
              red_shift++;
            }
            green_mask=window_info.visual->green_mask;
            green_shift=0;
            while ((green_mask & 0x01) == 0)
            {
              green_mask>>=1;
              green_shift++;
            }
            blue_mask=window_info.visual->blue_mask;
            blue_shift=0;
            while ((blue_mask & 0x01) == 0)
            {
              blue_mask>>=1;
              blue_shift++;
            }

            // Convert X image to DirectClass packets.

            if ((number_colors != 0) &&
                (window_info.visual->c_class == DirectColor))
                for (y=0; y < composite_image->rows; y++)
                {
                    for (x=0; x < composite_image->columns; x++)
                    {
                        pixel=XGetPixel(ximage,x,y);
                        index=(pixel >> red_shift) & red_mask;
                        p->red=XDownScale(colors[index].red);
                        index=(pixel >> green_shift) & green_mask;
                        p->green=XDownScale(colors[index].green);
                        index=(pixel >> blue_shift) & blue_mask;
                        p->blue=XDownScale(colors[index].blue);
                        p->index=0;
                        p->length=0;
                        p++;
                    }
                }
            else
                for (y=0; y < composite_image->rows; y++)
                {
                    for (x=0; x < composite_image->columns; x++)
                    {
                        pixel=XGetPixel(ximage,x,y);
                        color=(pixel >> red_shift) & red_mask;
                        p->red=XDownScale((color*65535L)/red_mask);
                        color=(pixel >> green_shift) & green_mask;
                        p->green=XDownScale((color*65535L)/green_mask);
                        color=(pixel >> blue_shift) & blue_mask;
                        p->blue=XDownScale((color*65535L)/blue_mask);
                        p->index=0;
                        p->length=0;
                        p++;
                    }
                }
            break;
        }
        case PseudoClass:
        {
            // Create colormap.
            composite_image->colors=number_colors;
            composite_image->colormap= new ColorPacket[composite_image->colors];
            if (composite_image->colormap == (ColorPacket *) NULL)
            {
                XDestroyImage(ximage);
                DestroyImage(composite_image);
                return((Image *) NULL);
            }
            for (i=0; i < composite_image->colors; i++)
            {
                composite_image->colormap[colors[i].pixel].red = XDownScale(colors[i].red);
                composite_image->colormap[colors[i].pixel].green = XDownScale(colors[i].green);
                composite_image->colormap[colors[i].pixel].blue = XDownScale(colors[i].blue);
            }

            // Convert X image to PseudoClass packets.
            for (y=0; y < composite_image->rows; y++)
            {
                for (x=0; x < composite_image->columns; x++)
                {
                    pixel=XGetPixel(ximage,x,y);
                    p->index=(unsigned short) pixel;
                    p->length=0;
                    p++;
                }
            }

            SyncImage(composite_image);
            break;
        }
    }

    XDestroyImage(ximage);
    image=composite_image;

    //        Free resources.
    delete [] colormap_info.colors;

    return(image);
}
#endif

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%  L S B F i r s t W r i t e S h o r t                                        %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Function LSBFirstWriteShort writes a long value as a 16 bit quantity in
%  least-significant byte first order.
%
%  The format of the LSBFirstWriteShort routine is:
%
%       LSBFirstWriteShort(value,file)
%
%  A description of each parameter follows.
%
%   o  value:  Specifies the value to write.
%
%   o  file:  Specifies the file to write the data to.
%
%
*/


static void LSBFirstWriteShort(const unsigned int value,FILE *file)
{
  unsigned char
    buffer[2];

  assert(file != (FILE *) NULL);
  buffer[0]=(unsigned char) (value);
  buffer[1]=(unsigned char) ((value) >> 8);
  (void) fwrite((char *) buffer,1,2,file);
}


/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   O p e n I m a g e                                                         %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Function OpenImage open a file associated with the image.
%
%  The format of the OpenImage routine is:
%
%      OpenImage(image)
%
%  A description of each parameter follows:
%
%    o image: The address of a structure of type Image.
%
%
*/
static bool OpenImage(Image *image)
{
    char  filename[MaxTextExtent];

    assert(image != (Image *) NULL);
    CopyString(filename, image->filename, MaxTextExtent);

    image->file=(FILE *) fopen(filename, "wb");
    if (image->file == (FILE *) NULL)
        return false;
    if (ferror(image->file))
        return false;


    (void) fseek(image->file,0L,SEEK_END);
    (void) fseek(image->file,0L,SEEK_SET);

    image->status=False;
    return true;
}



/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
+   W r i t e T G A I m a g e                                                 %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Function WriteTGAImage writes a image in the Truevision Targa rasterfile
%  format.
%
%  The format of the WriteTGAImage routine is:
%
%      status=WriteTGAImage(image_info,image)
%
%  A description of each parameter follows.
%
%    o status: Function WriteTGAImage return True if the image is written.
%      False is returned is there is a memory shortage or if the image file
%      fails to write.
%
%    o image_info: Specifies a pointer to an ImageInfo structure.
%
%    o image:  A pointer to a Image structure.
%
%
*/
static unsigned int WriteTGAImage(const ImageInfo* image_info, Image *image)
{
#define TargaColormap       1
#define TargaRGB            2
#define TargaRLEColormap    9
#define TargaRLERGB         10

    typedef struct _TargaHeader
    {
        unsigned char   id_length,
                        colormap_type,
                        image_type;

        unsigned short  colormap_index,
                        colormap_length;

        unsigned char   colormap_size;

        unsigned short  x_origin,
                        y_origin,
                        width,
                        height;

        unsigned char
                        bits_per_pixel,
                        attributes;
    } TargaHeader;

    Image*  flopped_image;

    int i, j, count, runlength;

    RunlengthPacket*    p;
    unsigned char*      q;
    unsigned char*      r;

    TargaHeader     targa_header;

    unsigned char*  targa_pixels;

    if (!OpenImage(image))
    {
        GenAppWarningMsg("PS_WriteTGA::WriteTGAImage", "Error opening file");
        return 0;
    }

    //   Flop image.
    image->orphan=True;
    flopped_image=FlopImage(image);
    image->orphan=False;
    if (flopped_image == (Image *) NULL)
    {
        GenAppWarningMsg("PS_WriteTGA::WriteTGAImage", "Unable to flop image");
        return 0;
    }
    CondenseImage(flopped_image);

    //   Initialize TGA raster file header.
    targa_header.id_length=0;
    targa_header.colormap_type=0;
    targa_header.colormap_index=0;
    targa_header.colormap_length=0;
    targa_header.colormap_size=0;
    targa_header.x_origin=0;
    targa_header.y_origin=0;
    targa_header.width=flopped_image->columns;
    targa_header.height=flopped_image->rows;
    targa_header.bits_per_pixel=8;
    targa_header.attributes=0;

    // Full color TGA raster.
    targa_header.image_type=TargaRGB;
    if (image_info->compression != NoCompression)
        targa_header.image_type = TargaRLERGB;
    targa_header.bits_per_pixel = 24;

    //  Write TGA header.
    (void) fputc((char) targa_header.id_length,image->file);
    (void) fputc((char) targa_header.colormap_type,image->file);
    (void) fputc((char) targa_header.image_type,image->file);
    LSBFirstWriteShort(targa_header.colormap_index,image->file);
    LSBFirstWriteShort(targa_header.colormap_length,image->file);
    (void) fputc((char) targa_header.colormap_size,image->file);
    LSBFirstWriteShort(targa_header.x_origin,image->file);
    LSBFirstWriteShort(targa_header.y_origin,image->file);
    LSBFirstWriteShort(targa_header.width,image->file);
    LSBFirstWriteShort(targa_header.height,image->file);
    (void) fputc((char) targa_header.bits_per_pixel,image->file);
    (void) fputc((char) targa_header.attributes,image->file);

    // Convert MIFF to TGA raster pixels.
    count=(unsigned int) (targa_header.bits_per_pixel * targa_header.width * targa_header.height) >> 3;
    if (image_info->compression != NoCompression)
        count+=(count/128)+1;

    targa_pixels= new unsigned char[count];
    if (targa_pixels == (unsigned char *) NULL)
    {
        GenAppWarningMsg("PS_WriteTGA::WriteTGAImage", "alloc bad");
        return 0;
    }
    p=flopped_image->pixels+(flopped_image->packets-1);
    q=targa_pixels;

    // Convert DirectClass packet to TGA RGB pixel.
    if (image_info->compression == NoCompression)
        for (i=0; i < flopped_image->packets; i++)
        {
            for (j=0; j <= ((int) p->length); j++)
            {
                *q++=DownScale(p->blue);
                *q++=DownScale(p->green);
                *q++=DownScale(p->red);
            }
            p--;
        }
    else
        for (i=0; i < flopped_image->packets; i++)
        {
            for (runlength=p->length+1; runlength > 128; runlength-=128)
            {
                *q++=0xff;
                *q++=DownScale(p->blue);
                *q++=DownScale(p->green);
                *q++=DownScale(p->red);
            }
            r=q;
            *q++=0x80+(runlength-1);
            *q++=DownScale(p->blue);
            *q++=DownScale(p->green);
            *q++=DownScale(p->red);
            if (runlength != 1)
                p--;
            else
            {
                for ( ; i < flopped_image->packets; i++)
                {
                    p--;
                    if ((p->length != 0) || (runlength == 128))
                        break;
                    *q++=DownScale(p->blue);
                    *q++=DownScale(p->green);
                    *q++=DownScale(p->red);
                    runlength++;
                }
                *r=runlength-1;
            }
        }

    (void) fwrite((char *) targa_pixels,1,(int) (q-targa_pixels),image->file);

    DestroyImage(flopped_image);
    delete [] targa_pixels;

    CloseImage(image);
    return(True);
}

#ifdef _WIN32

// Microsoft Windows BMP image file, clear 24bit, no color map
static Image* GetImageFromBMP_RGB(BITMAPINFOHEADER& bmpInfoHeader, unsigned char *pBitmapData)
{
    Image *image = AllocateImage(NULL);

    image->columns = (unsigned int) bmpInfoHeader.biWidth;
    image->rows = (unsigned int) bmpInfoHeader.biHeight;
    image->depth = 8;

    unsigned int bytes_per_line = 4*((image->columns * bmpInfoHeader.biBitCount+31)/32);
    unsigned int image_size = bytes_per_line*image->rows;
    unsigned char* pixels = pBitmapData;
    image->packets = image->columns * image->rows;
    image->pixels = new RunlengthPacket[image->packets];

    //  Convert BMP raster image to pixel packets.
    ASSERT(bmpInfoHeader.biBitCount == 24);
    //  Convert DirectColor scanline.
    RunlengthPacket* q = image->pixels;
    for (int y=0; y < image->rows; y++) {
        unsigned char* p = pixels + (image->rows-y-1) * bytes_per_line;
        for (int x=0; x < image->columns; x++) {
            q->red = UpScale(*p++);
            q->green = UpScale(*p++);
            q->blue = UpScale(*p++);
            q->index = 0;
            q->length = 0;
            q++;
        }
    }
    return image;
}

#pragma warning( pop )

void PS_WriteTGA(const char* fileName, BITMAPINFOHEADER& bmpInfoHeader, unsigned char *pBitmapData)
{
    Image* image = GetImageFromBMP_RGB(bmpInfoHeader, pBitmapData);

    if (image == NULL) {
        GenAppWarningMsg("PS_WriteTGA::PS_WriteTGA", "X Get bad");
        return;
    }

    CopyString(image->filename, fileName, MaxTextExtent);
    CondenseImage(image);

    ImageInfo image_info;
    GetImageInfo(&image_info);

    WriteTGAImage(&image_info, image);
    DestroyImage(image);
}

#else // UNIX version

void PS_WriteTGA(const char*     fileName,
                 const Widget    widgetToWrite)
{
    Image*          image;

    // get display
    Display*  display = XtDisplay(widgetToWrite);

    XSetErrorHandler(XError);

    //    Select target window.
    Window target = XtWindow(widgetToWrite);

    XGrabServer(display);
    image = XGetWindowImage(display, target);
    XUngrabServer(display);

    if (image == (Image *) NULL)
    {
        GenAppWarningMsg("PS_WriteTGA::PS_WriteTGA", "X Get bad");
        return;
    }

    CopyString(image->filename, fileName, MaxTextExtent);
    CondenseImage(image);

    ImageInfo image_info;
    GetImageInfo(&image_info);

    WriteTGAImage(&image_info, image);
    DestroyImage(image);
}

#endif // WIN32

#endif // TGAIMAGE

