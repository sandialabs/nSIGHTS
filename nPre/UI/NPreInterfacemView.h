///////////////////////////////////////////////////////////////////////////////////
//
// NPreInterfacemView.h
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
//      Tabbed menu and dialog layout.
//
///////////////////////////////////////////////////////////////////////////////////

#ifndef NPREINTERFACEMVIEW_H
#define NPREINTERFACEMVIEW_H

#include <genInterface/InterfacemView.h>
#include <App/nPreRoot.h>

class NPreMViewNode : public MViewNode
{
public:
    NPreMViewNode(InterfaceNode* pParent, int nIdent, NodeDescription* pDescription, nPreRoot& projectRoot);
    virtual ~NPreMViewNode();

protected:
    virtual void    BeforeShow(bool bShow);
    virtual void    AfterShow(bool bShow);
};

class FieldDataMViewNode : public NPreMViewNode
{
public:
    FieldDataMViewNode(InterfaceNode* pParent, int nIdent, NodeDescription* pDescription);

};

class FitSpecMViewNode : public NPreMViewNode
{
public:
    FitSpecMViewNode(InterfaceNode* pParent, int nIdent, NodeDescription* pDescription);

protected:
    virtual void    AfterShow(bool bShow);

};

class SequenceMViewNode : public NPreMViewNode
{
public:
    SequenceMViewNode(InterfaceNode* pParent, int nIdent, NodeDescription* pDescription);
};


class ProcessingSetupMViewNode : public NPreMViewNode
{
public:
    ProcessingSetupMViewNode(InterfaceNode* pParent, int nIdent, NodeDescription* pDescription);
};


#endif // _INTERFACEMVIEW_H_

