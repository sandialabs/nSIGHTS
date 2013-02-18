///////////////////////////////////////////////////////////////////////////////////
//
// DPO_STEPWISEPS.cpp
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

#include "SaLibAfx.h"

#include <genClassPS/PS_UIBase.h>

#include <saClass/DO_SATable.h>

#include "DPO_STEPWISEPS.h"

DPO_STEPWISEPS::DPO_STEPWISEPS()
{
    listDataOK = false;
}


void DPO_STEPWISEPS::SetListsCB(void* inObj)
{
    static_cast<DPO_STEPWISEPS*>(inObj)->SetLists();
}

void DPO_STEPWISEPS::SetLists()
{
   listDataOK = indepVarObjRefUI.CheckDataObj();

    SC_StringArray varList;
    SC_StringArray obsList;
    if (listDataOK)
    {
        const DC_SATable&   indepTableDC =
            static_cast<DO_SATable*>(indepVarObjRefUI.GetDataObj())->GetSATableRef();

        indepTableDC.GetVariableIDs(varList);
        indepTableDC.GetTypeIDs(DC_SATable::saVector, obsList);
    }
    else
    {
        varList.Alloc(1);
        varList += "no variables";
        obsList.Alloc(1);
        obsList += "no obs";
    }

    dropIndexesUI.SetListText(obsList);
    forceIndexesUI.SetListText(varList);
    weightIndexUI.SetListText(varList);
    SetAllSensitive();
}

void DPO_STEPWISEPS::SetAllSensitiveCB(void* inObj)
{
    static_cast<DPO_STEPWISEPS*>(inObj)->SetAllSensitive();
}

void DPO_STEPWISEPS::SetAllSensitive()
{
    optSiginUI.SetSensitive(optBACKWARDUI.GetValue() || optSTEPWISEUI.GetValue());
    optSigoutUI.SetSensitive(optSTEPWISEUI.GetValue());

    dropFrame.SetSensitive(listDataOK && optDROPUI.GetValue());
    forceFrame.SetSensitive(listDataOK && optFORCEUI.GetValue());;
    weightFrame.SetSensitive(listDataOK && optWEIGHTUI.GetValue());;
}

void DPO_STEPWISEPS::InitMenu()
{
    DPO_SAObjBasePS::InitMenu();
    indepVarObjRefUI.AddOtherObjectCallback(SetListsCB, this);

    ArgListC args;

    args.StdTopObjectAttach(varForm.GetWidget());
    optionFrame.InitFramedUI("Basic Control", PPform.GetWidget(), args);

    args.StdFormAttach();
    optRow1Form.InitWidget(optionFrame.GetForm(), args);

    args.StdToggleSetup(2);
    optBACKWARDUI.StdToggleInit("BACKWARD", optRow1Form.GetWidget(), args);
    optBACKWARDUI.AddOtherObjectCallback(SetAllSensitiveCB, this);


    args.StdToggleSetup(25);
    optPRESSUI.StdToggleInit("PRESS", optRow1Form.GetWidget(), args);

    args.StdToggleSetup(50);
    optRANKUI.StdToggleInit("RANK", optRow1Form.GetWidget(), args);

    args.StdToggleSetup(75);
    optSTAND01UI.StdToggleInit("STAND01", optRow1Form.GetWidget(), args);

    args.StdTopObjectAttach(optRow1Form.GetWidget());;
    optRow2Form.InitWidget(optionFrame.GetForm(), args);

    args.StdToggleSetup(2);
    optSTEPWISEUI.StdToggleInit("STEPWISE", optRow2Form.GetWidget(), args);
    optSTEPWISEUI.AddOtherObjectCallback(SetAllSensitiveCB, this);

    args.StdToggleSetup(25);
    optFORCEUI.StdToggleInit("FORCE", optRow2Form.GetWidget(), args);
    optFORCEUI.AddOtherObjectCallback(SetListsCB, this);

    args.StdToggleSetup(50);
    optWEIGHTUI.StdToggleInit("WEIGHT", optRow2Form.GetWidget(), args);
    optWEIGHTUI.AddOtherObjectCallback(SetListsCB, this);

    args.StdToggleSetup(75);
    optDROPUI.StdToggleInit("DROP", optRow2Form.GetWidget(), args);
    optDROPUI.AddOtherObjectCallback(SetListsCB, this);

    args.StdTopObjectAttach(optionFrame);
    parameterForm.InitWidget(PPform.GetWidget(), args);

    args.StdSplitSetup(25);
    optSiginUI.InitFramedUI("SIGIN", parameterForm.GetWidget(), args);

    args.StdSplitMiddle(26, 50);
    optSigoutUI.InitFramedUI("SIGOUT", parameterForm.GetWidget(), args);

    args.StdSplitMiddle(51, 75);
    const char* decStrs[] = {"0" , "0.0", "0.00", "0.000", "0.0000",
                              "0.00000", "0.000000", "0.0000000",
                              "0.00000000"};
    decFormatUI.PreInitSetUp(decStrs, 9);
    decFormatUI.InitFramedUI("Decimal List Format", parameterForm.GetWidget(), args);

    args.StdSplitFinish(25);
    const char* sciStrs[] = {"0E+00" , "0.0E+00", "0.00E+00", "0.000E+00",
                            "0.0000E+00", "0.00000E+00", "0.000000E+00",
                            "0.0000000E+00", "0.00000000E+00"};

    sciFormatUI.PreInitSetUp(sciStrs, 9);
    sciFormatUI.InitFramedUI("Scientific List Format", parameterForm.GetWidget(), args);

    args.StdTopObjectAttach(parameterForm.GetWidget());
    outputFrame.InitFramedUI("Output Control", PPform.GetWidget(), args);

    args.StdFormAttach();
    outRow1Form.InitWidget(outputFrame.GetForm(), args);

    args.StdToggleSetup(2);
    outMEANUI.StdToggleInit("MEAN", outRow1Form.GetWidget(), args);

    args.StdToggleSetup(25);
    outCORRUI.StdToggleInit("CORR", outRow1Form.GetWidget(), args);

    args.StdToggleSetup(50);
    outSSCPUI.StdToggleInit("SSCP", outRow1Form.GetWidget(), args);

    args.StdToggleSetup(75);
    outINVERSEUI.StdToggleInit("INVERSE", outRow1Form.GetWidget(), args);

    args.StdTopObjectAttach(outRow1Form.GetWidget());;
    outRow2Form.InitWidget(outputFrame.GetForm(), args);

    args.StdToggleSetup(2);
    outSTEPSUI.StdToggleInit("STEPS", outRow2Form.GetWidget(), args);

    args.StdToggleSetup(25);
    outRESIDUALSUI.StdToggleInit("RESIDUALS", outRow2Form.GetWidget(), args);

    args.StdToggleSetup(50);
    outOTHERUI.StdToggleInit("OTHER", outRow2Form.GetWidget(), args);

    args.StdTopObjectAttach(outputFrame);
    args.BottomFormAttach();
    selectForm.InitWidget(PPform.GetWidget(), args);

    args.StdSplitSetup(35);
    forceFrame.InitFramedUI("FORCEd Variables", selectForm.GetWidget(), args);

    args.StdSplitMiddle(35, 70);
    weightFrame.InitFramedUI("WEIGHT Variable", selectForm.GetWidget(), args);

    args.StdSplitFinish(30);
    dropFrame.InitFramedUI("DROPped Observations", selectForm.GetWidget(), args);

    args.AllFormAttach();
    forceIndexesUI.isSingleSel = false;
    forceIndexesUI.InitWidget(forceFrame.GetForm(), args);


    weightIndexUI.InitWidget(weightFrame.GetForm(), args);

    dropIndexesUI.isSingleSel = false;
    dropIndexesUI.InitWidget(dropFrame.GetForm(), args);
}

void DPO_STEPWISEPS::MenuOpen(DPO_STEPWISE& inData)
{
    DPO_SAObjBasePS::MenuOpen(inData);
}

void DPO_STEPWISEPS::SetMenuData(DPO_STEPWISE&  inData)
{
    MenuBaseC::SetMenuData(inData);
    DPO_SAObjBasePS::SetMenuData(inData);

    optBACKWARDUI.SetValue(inData.optBACKWARD);
    optPRESSUI.SetValue(inData.optPRESS);
    optRANKUI.SetValue(inData.optRANK);
    optSTAND01UI.SetValue(inData.optSTAND01);

    optSTEPWISEUI.SetValue(inData.optSTEPWISE);
    optFORCEUI.SetValue(inData.optFORCE);
    optWEIGHTUI.SetValue(inData.optWEIGHT);
    optDROPUI.SetValue(inData.optDROP);

    optSiginUI.SetFieldValue(inData.optSigin);
    optSigoutUI.SetFieldValue(inData.optSigout);

    outMEANUI.SetValue(inData.outMEAN);
    outCORRUI.SetValue(inData.outCORR);
    outSSCPUI.SetValue(inData.outSSCP);
    outINVERSEUI.SetValue(inData.outINVERSE);
    outSTEPSUI.SetValue(inData.outSTEPS);
    outRESIDUALSUI.SetValue(inData.outRESIDUALS);
    outOTHERUI.SetValue(inData.outOTHER);

    forceIndexesUI.SetMultipleSelection(inData.forceIndexes);
    weightIndexUI.SetSingleSelection(inData.weightIndex);
    dropIndexesUI.SetMultipleSelection(inData.dropIndexes);

    decFormatUI.SetChoice(inData.decForm.digitsPrecision);
    sciFormatUI.SetChoice(inData.sciForm.digitsPrecision);

    SetLists();
}

bool DPO_STEPWISEPS::UIdataOK()
{
    DPO_SAObjBasePS::UIdataOK();
    if (optBACKWARDUI.GetValue() || optSTEPWISEUI.GetValue())
    {
        optSiginUI.FieldValueOK(allOK, "SIGIN", 0.001, 1.0);
        if (optSTEPWISEUI.GetValue())
            optSigoutUI.FieldValueOK(allOK, "SIGOUT", 0.001, 1.0);
    }
    return allOK;
}

void DPO_STEPWISEPS::GetMenuData(DPO_STEPWISE&  outData)
{
    MenuBaseC::GetMenuData(outData);
    DPO_SAObjBasePS::GetMenuData(outData);

    outData.optBACKWARD = optBACKWARDUI.GetValue();
    outData.optPRESS    = optPRESSUI.GetValue();
    outData.optRANK     = optRANKUI.GetValue();
    outData.optSTAND01  = optSTAND01UI.GetValue();

    outData.optSTEPWISE = optSTEPWISEUI.GetValue();
    outData.optFORCE    = optFORCEUI.GetValue();
    outData.optWEIGHT   = optWEIGHTUI.GetValue();
    outData.optDROP     = optDROPUI.GetValue();

    if (outData.optBACKWARD || outData.optSTEPWISE)
    {
        outData.optSigin = optSiginUI.GetFieldValue();
        if (outData.optSTEPWISE)
            outData.optSigout = optSigoutUI.GetFieldValue();
    }

    outData.outMEAN     = outMEANUI.GetValue();
    outData.outCORR     = outCORRUI.GetValue();
    outData.outSSCP     = outSSCPUI.GetValue();
    outData.outINVERSE  = outINVERSEUI.GetValue();
    outData.outSTEPS    = outSTEPSUI.GetValue();
    outData.outRESIDUALS = outRESIDUALSUI.GetValue();
    outData.outOTHER    = outOTHERUI.GetValue();

    outData.optSigin = optSiginUI.GetFieldValue();
    outData.optSigout = optSigoutUI.GetFieldValue();

    outData.decForm.digitsPrecision = decFormatUI.GetChoice();
    outData.sciForm.digitsPrecision = sciFormatUI.GetChoice();


    if (outData.optFORCE)
        forceIndexesUI.GetMultipleSelection(outData.forceIndexes);

    if (outData.optWEIGHT)
        outData.weightIndex = weightIndexUI.GetSingleSelection();

    if (outData.optDROP)
        dropIndexesUI.GetMultipleSelection(outData.dropIndexes);

}

