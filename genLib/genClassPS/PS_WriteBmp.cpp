///////////////////////////////////////////////////////////////////////////////////
//
// PS_WriteBmp.cpp
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
//              Writes plot bitmaps in BMP format
//
///////////////////////////////////////////////////////////////////////////////////

#include "GenLibAfx.h"

#include "PS_WriteBmp.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


static int NumColorEntries(int nBitsPerPixel, int nCompression, DWORD biClrUsed)
{
    int nColors = 0;
    switch (nBitsPerPixel) {
    case 1:
        nColors = 2;
        break;
    case 2:
        nColors = 4;
        break;   // winCE only
    case 4:
        nColors = 16;
        break;
    case 8:
        nColors = 256;
        break;
    case 24:
        nColors = 0;
        break;
    case 16:
    case 32:
#ifdef _WIN32_WCE
        nColors = 3;     // I've found that PocketPCs need this regardless of BI_RGB or BI_BITFIELDS
#else
        if (nCompression == BI_BITFIELDS)
            nColors = 3; // 16 or 32 bpp have 3 colors(masks) in the color table if bitfield compression
        else
            nColors = 0; // 16 or 32 bpp have no color table if no bitfield compression
#endif
        break;

    default:
        ASSERT(FALSE);
    }

    // If biClrUsed is provided, and it is a legal value, use it
    if (biClrUsed > 0 && biClrUsed <= (DWORD)nColors) {
        return biClrUsed;
    }

    return nColors;
}

void PS_WriteBmp(const char* fileName,
                 BITMAPINFOHEADER& bmInfoHeader,
                 unsigned char *pBitmapData)
{
    CFile file;
    if (!file.Open(fileName, CFile::modeWrite|CFile::modeCreate)) {
        return;
    }

    BITMAPFILEHEADER hdr;

    int iColorTableSize = NumColorEntries(bmInfoHeader.biBitCount, bmInfoHeader.biCompression, bmInfoHeader.biClrUsed);
    DWORD dwBitmapInfoSize = sizeof(BITMAPINFO) + (iColorTableSize - 1)*sizeof(RGBQUAD);
    DWORD dwFileHeaderSize = dwBitmapInfoSize + sizeof(hdr);

    // Fill in the fields of the file header
    hdr.bfType       = 0x4D42; // BITMAP_FILEMARKER 'BM'
    hdr.bfSize       = dwFileHeaderSize + bmInfoHeader.biSizeImage;
    hdr.bfReserved1  = 0;
    hdr.bfReserved2  = 0;
    hdr.bfOffBits    = dwFileHeaderSize;

    // Write the file header
    file.Write(&hdr, sizeof(hdr));

    // Write the DIB header
    file.Write(&bmInfoHeader, dwBitmapInfoSize);

    // Write DIB bits
    file.Write(pBitmapData, bmInfoHeader.biSizeImage);
}

