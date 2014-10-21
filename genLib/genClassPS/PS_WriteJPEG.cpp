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

#include "jpge.h"  // jpeg-compressor header (single source file - no library)

void PS_WriteJPEG(const char* fileName, unsigned char* pBitmapData, 
	              int nX, int nY, int nQuality)
{
	const int nChan = 3;  // RGB
	int orig_buff_size = nChan*nX*nY;
	unsigned char* pBitmapDataSwap = new unsigned char[orig_buff_size];

	// Swapping lines in the buffer. OpenGL writes from lower line 
	// going up but jpeg-compressor library works from top down
	for (int j = 0; j < nY; j++) { // line counter
		for (int i = 0; i < nX; i++) { // column counter
			for (int k = 0; k < nChan; k++) { // channel counter
				pBitmapDataSwap[(j*nX+i)*nChan+k] = pBitmapData[((nY-1-j)*nX+i)*nChan+k];
			}
		}
	}

	// length of buffer after compression
	int comp_size = orig_buff_size; 
	
	// allocate buffer hopefully big enough for compression results
	unsigned char* compressedData = new unsigned char[orig_buff_size];  
	
	jpge::params params;	
    params.m_quality = nQuality;
	
	bool success = jpge::compress_image_to_jpeg_file_in_memory(compressedData, comp_size, 
		                                                       nX, nY, 3, pBitmapDataSwap, params);

	if(!success){
		GenAppWarningMsg("PS_WriteJPEG", "Error compressing JPEG data");
	} else {
	  CFile file;
 	    if (!file.Open(fileName,CFile::modeWrite | CFile::modeCreate)){
            GenAppWarningMsg("PS_WriteJPEG", "Error opening JPEG file for writing");
	    } else {
           file.Write(compressedData, comp_size);
		}
	}

	delete [] pBitmapDataSwap;
	delete [] compressedData;
}

