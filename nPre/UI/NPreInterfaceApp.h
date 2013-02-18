///////////////////////////////////////////////////////////////////////////////////
//
// NPreInterfaceApp.h
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
//      Dialog layout for tabbed menus.
//
///////////////////////////////////////////////////////////////////////////////////

#ifndef NPREINTERFACEAPP_H
#define NPREINTERFACEAPP_H

#include <genInterface/InterfaceCtrls.h>

#include <Var/G_Parameter.h>

/////////////////////////////////////////////////////////////////////////////
// Application specific control stuff

class TabbedFolderNodeConfig : public TabbedFolderNode1
{
public:
    TabbedFolderNodeConfig(InterfaceNode* pParent, int nIdent, NodeDescription* pDescription);

    virtual void CheckRestrictions();
    virtual CWnd* ShowNode(bool bShow = true, CRect* pRect = NULL);
};

class TabbedFolderNodeParameter : public TabbedFolderNode1
{
public:
    TabbedFolderNodeParameter(InterfaceNode* pParent, int nIdent, NodeDescription* pDescription);

    virtual bool CreateNode(CWnd* pParentWnd);
    virtual CWnd* ShowNode(bool bShow = true, CRect* pRect = NULL);
};

class TabbedFolderNodePointsParameter : public TabbedFolderNode
{
public:
    TabbedFolderNodePointsParameter(InterfaceNode* pParent, int nIdent, NodeDescription* pDescription);

    virtual void CheckRestrictions();
    virtual bool CreateNode(CWnd* pParentWnd);
    void ResetNodes(ParameterArray* pointParameters);
};

class TabbedFolderNodePointsParameterInside : public TabbedFolderNode1
{
public:
    TabbedFolderNodePointsParameterInside(InterfaceNode* pParent, int nIdent, NodeDescription* pDescription,
                                           Parameter* pParameter);

public:
    Parameter* m_pParameter;
};

class TabbedFolderNodeFitSpecification : public TabbedFolderNode
{
public:
    TabbedFolderNodeFitSpecification(InterfaceNode* pParent, int nIdent, NodeDescription* pDescription);

    virtual void CheckRestrictions();
};

class TabbedFolderNodeOptimization : public TabbedFolderNode
{
public:
    TabbedFolderNodeOptimization(InterfaceNode* pParent, int nIdent, NodeDescription* pDescription);

    virtual void CheckRestrictions();
};

class TabbedFolderNodeVary  : public TabbedFolderNode
{
public:
    TabbedFolderNodeVary(InterfaceNode* pParent, int nIdent, NodeDescription* pDescription);

    virtual CWnd* ShowNode(bool bShow = true, CRect* pRect = NULL);
    virtual void CheckRestrictions();
};

class TabbedFolderNodeSimulationOutput : public TabbedFolderNode
{
public:
    TabbedFolderNodeSimulationOutput(InterfaceNode* pParent, int nIdent, NodeDescription* pDescription);
    virtual CWnd* ShowNode(bool bShow = true, CRect* pRect = NULL);
};

class TabbedFolderNodeSequence : public TabbedFolderNode
{
private:
    SC_DoubleArray  prevTimes;
public:
    TabbedFolderNodeSequence(InterfaceNode* pParent, int nIdent, NodeDescription* pDescription);
    virtual CWnd* ShowNode(bool bShow = true, CRect* pRect = NULL);

};

class TabbedFolderNodeSampling : public TabbedFolderNode
{
public:
    TabbedFolderNodeSampling(InterfaceNode* pParent, int nIdent, NodeDescription* pDescription);

    virtual CWnd*   ShowNode(bool bShow = true, CRect* pRect = NULL);

    virtual void Show(int nIdent);

    virtual void CheckRestrictions();
};

class TabbedFolderNodeSamplingCorrelations : public TabbedFolderNode
{
public:
    TabbedFolderNodeSamplingCorrelations(InterfaceNode* pParent, int nIdent, NodeDescription* pDescription);

    virtual void CheckRestrictions();
};

class TabbedFolderNodeOutputFileSetup : public TabbedFolderNode
{
public:
    TabbedFolderNodeOutputFileSetup(InterfaceNode* pParent, int nIdent, NodeDescription* pDescription);

    virtual void CheckRestrictions();

    virtual bool CreateNode(CWnd* pParentWnd);
    virtual CWnd* ShowNode(bool bShow = true, CRect* pRect = NULL);
};

class TabbedFolderPlotSetup: public TabbedFolderNode
{
public:
    TabbedFolderPlotSetup(InterfaceNode* pParent, int nIdent, NodeDescription* pDescription);

    virtual void CheckRestrictions();
};


class OutlookBar2 : public OutlookBar
{
public:
    OutlookBar2(InterfaceNode* pParent, int nIdent, NodeDescription* pDescription);

    virtual InterfaceNode* SelectNode(int nNode);
    virtual void CheckRestrictions();
};

#endif // _INTERFACEAPP_H_

