///////////////////////////////////////////////////////////////////////////////////
//
// DFO_ReadSequenceTimesCF.cpp
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
// DESCRIPTION:
//
//
//
///////////////////////////////////////////////////////////////////////////////////

#include <sstream>

#include <genClass/C_ConvFile.h>
#include <genClass/U_Str.h>
#include <genClass/U_Msg.h>

#include <genClass/C_ConvFile.h>

#include <nsDFO/DFO_ReadSequenceTimes.h>


class SeqTimeFile :  public ConvFileC {
    private:
        int             	nextInc;
        int             	nextCount;
    public:
        DC_SequenceTimes& 	seqTimeData;

    public:
                        	SeqTimeFile(char*              	inputFile,
                            		    DC_SequenceTimes&   pd);
                        	~SeqTimeFile(){};

        void            	Init();
        void            	ReadSeqTimes();
};

SeqTimeFile :: SeqTimeFile(char*          inputFile,
                 DC_SequenceTimes&  pd)
    : ConvFileC(inputFile), seqTimeData(pd)
{
}

void SeqTimeFile :: Init()
{
    nextInc = 5;
    nextCount = 50;
    seqTimeData.Clear();
}

void  SeqTimeFile::ReadSeqTimes()
{
	// read data
    while(true)
    {
        if (CFeof())
            break;
        if (CFreadText())
		{
			char tempStr[80];
			// ID -- must be contigous
			if (!CFgetNextStr(tempStr, 80))
        		CFtextError("no ID ???");

			// add ID and start time
			seqTimeData.AddSequence(tempStr, CFgetNextDouble());

			// another field ??
			if (CFgetNextStr(tempStr, 80))
			{
				double tVal;
				if (IsValidReal(tempStr, tVal))
				{
					seqTimeData.AddEndTime(tVal);
					if (!seqTimeData.sequenceStartTimes.IsAscending())
        				CFtextError("times must be ascending");
					return;
				}
			}
		}
	}
    CFtextError("bad format -- no end time on last row");
}



bool DFO_ReadSequenceTimes::ReadSequenceTimes()
{
    SeqTimeFile cmFile(sequenceTimesFname, sequenceTimesDC);
    cmFile.CFsetMsgStuff("ReadSequenceTimes", "SeqTimeFile", 1);
    try
    {
        cmFile.Init();
		cmFile.ReadSeqTimes();
    }
    catch (ConvFileC::CFReadError re) {
        SetObjErrMsg(re.errMsg);
        return false;
    }

	{
		using namespace std;
		ostringstream ostr;
		ostr <<  "Seq read OK: " << sequenceTimesDC.Size() << " sequences " << ends;
	    GenAppInfoMsg("ReadSequenceTimes", ostr.str().c_str());
	}
    return true;
}
