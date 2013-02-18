///////////////////////////////////////////////////////////////////////////////////
//
// InterfaceDesc.h
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
// DESCRIPTION: Windows specific code for implementing Outlook style
//              application with dialogs and object trees.
//
///////////////////////////////////////////////////////////////////////////////////

#ifndef INTERFACEDESC_H
#define INTERFACEDESC_H

#include <genListClass/C_ListFactory.h>

struct NodeDescription
{
    char*               m_szName;
    UINT                m_ImageID;
    CRuntimeClass*      m_rtDialog;
    NodeDescription*    m_pNextLevel;
    ListFactory*        m_pListFactory;

    NodeDescription* GetNext();
    bool IsLast();
};

inline NodeDescription* NodeDescription::GetNext()
{
    return m_pNextLevel;
}
inline bool NodeDescription::IsLast()
{
    return m_szName == NULL;
}

class NodeDescriptionRuntime : public NodeDescription
{
public:
    NodeDescriptionRuntime();
    NodeDescriptionRuntime(char* szName,
                           UINT ImageID,
                           CRuntimeClass* rtDialog,
                           NodeDescription* pNextLevel,
                           ListFactory* pListFactory = NULL);
};

inline NodeDescriptionRuntime::NodeDescriptionRuntime()
{
    m_szName = NULL;
}
inline NodeDescriptionRuntime::NodeDescriptionRuntime(char* szName,
                                                      UINT ImageID,
                                                      CRuntimeClass* rtDialog,
                                                      NodeDescription* pNextLevel,
                                                      ListFactory* pListFactory)
{
    m_szName = szName;
    m_ImageID = ImageID;
    m_rtDialog = rtDialog;
    m_pNextLevel = pNextLevel;
    m_pListFactory = pListFactory;
}

class NodeDescriptionInterator
{
public:
    NodeDescriptionInterator(NodeDescription* pDescription);
    void First();
    void Next();
    bool IsLast();
    NodeDescription* Get();
protected:
    NodeDescription* m_pFirst;
    NodeDescription* m_pCurrent;
};

inline NodeDescriptionInterator::NodeDescriptionInterator(NodeDescription* pDescription) :
    m_pFirst(pDescription),
    m_pCurrent(NULL)
{
}
inline void NodeDescriptionInterator::First()
{
    m_pCurrent = m_pFirst;
}
inline void NodeDescriptionInterator::Next()
{
    m_pCurrent++;
}
inline bool NodeDescriptionInterator::IsLast()
{
    return m_pCurrent == NULL || m_pCurrent->m_szName == NULL;
}
inline NodeDescription* NodeDescriptionInterator::Get()
{
    ASSERT(m_pCurrent != NULL && !IsLast());
    return m_pCurrent;
}

extern NodeDescription nodesRoot[];

#endif // INTERFACEDESC_H

