///////////////////////////////////////////////////////////////////////////////////
//
// PS_WriteJPEG.cpp
//
///////////////////////////////////////////////////////////////////////////////////
//
// Copyright 2012 Sandia Corporation. Under the terms of Contract
// DE-AC04-94AL85000 with Sandia Corporation, the U.S. Government
// retains certain rights in this software.
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

#include "GenLibAfx.h"

#include <genClass/U_Msg.h>

#include <stdio.h>
#include "jpeglib.h"  // libjpeg-turbo header

void PS_WriteJPEG(const char* fileName, unsigned char* pBitmapData, 
	              int width, int height, int nQuality)
{

  // three-channel data
  JSAMPLE * image_buffer = new unsigned char[3 * width * height];

  // Swapping lines in the buffer
  // OpenGL writes from lower line going up but jpeg library works
  // reverse (from up to down)
  for (int j = 0; j < height; j++) { // line counter
    for (int i = 0; i < width; i++) { // column counter

      image_buffer[(j*width+i)*3]   = pBitmapData[((height-1-j)*width+i)*3];
      image_buffer[(j*width+i)*3+1] = pBitmapData[((height-1-j)*width+i)*3+1];
      image_buffer[(j*width+i)*3+2] = pBitmapData[((height-1-j)*width+i)*3+2];
    }
  }

  // based upon example.c distributed with libjpeg-turbo source code.

  // struct contains the JPEG compression parameters and pointers to
  // working space (allocated as needed by the JPEG library).
  struct jpeg_compress_struct cinfo;

  // JPEG error handler.
  struct jpeg_error_mgr jerr;

  FILE * outfile;		// target file
  JSAMPROW row_pointer[1]; // pointer to JSAMPLE rows
  int row_stride;       // row width in image buffer

  // setup error handler and initialize JPEG compression object 
  cinfo.err = jpeg_std_error(&jerr);
  jpeg_create_compress(&cinfo);
  
  if ((outfile = fopen(fileName, "wb")) == NULL) {
	  GenAppWarningMsg("PS_WriteJPEG", "Error opening JPEG file for writing");
  } else {

    jpeg_stdio_dest(&cinfo, outfile);
  
    cinfo.image_width = width; 	// image width and height in pixels 
    cinfo.image_height = height;
    cinfo.input_components = 3;		// # of color components per pixel
    cinfo.in_color_space = JCS_RGB; 	// colorspace of input image (required)
  
    // set default compression parameters
    jpeg_set_defaults(&cinfo);
  
    // start compressor
    jpeg_start_compress(&cinfo, TRUE);
  
    row_stride = width * 3;

    while (cinfo.next_scanline < cinfo.image_height) {
      /* jpeg_write_scanlines expects an array of pointers to scanlines.
       * Here the array is only one element long, but you could pass
       * more than one scanline at a time if that's more convenient.
       */
      row_pointer[0] = & image_buffer[cinfo.next_scanline * row_stride];
      (void) jpeg_write_scanlines(&cinfo, row_pointer, 1);
    }

    // Finish compression, close output file 
    jpeg_finish_compress(&cinfo);
    fclose(outfile);
  }

  // release JPEG compression object memory
  jpeg_destroy_compress(&cinfo);

}

