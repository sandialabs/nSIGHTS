///////////////////////////////////////////////////////////////////////////////////
//
// C_Geometry.h
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
// DESCRIPTION: Platform independent and application independent base claseess and
//              support classes.
//
///////////////////////////////////////////////////////////////////////////////////

#ifndef C_GEOMETRY_H
#define C_GEOMETRY_H

#include <genClass/C_Common.h>
#include <genClass/SC_IntArray.h>

class GeomMap {
public:
  SC_IntArray     nassocGeom;         // contains number of secondary geometry data
                                      // associated with primary geometry
  SC_IntArray     assocGeomStartIndx; // contains start indx in map
  SC_IntArray     assocGeomMap;       // actual map

private:
  int             maxAssocMap;        // assumes all set with SetAssoc

public:
  GeomMap(){maxAssocMap = 0;}
  GeomMap(const GeomMap& a);
  ~GeomMap(){};

  // ******************* operators ******************
  GeomMap&        operator= (const GeomMap& a);  // copy operator

  //  ******************** memory *******************
  void            Alloc(int nprimaryGeom,
                        int nassocGeomIndx);
  void            Alloc(int nprimaryGeom);  // resizable map

  void            AllocForAdd(int nprimaryGeom,
                              int maxAssoc);      // for use with AddAssoc()

  void            AllocForCount(int nprimaryGeom);  // count then alloc, then addassoc
  void            AllocFromCount();  // after AllocForCount before addassoc

  //  ******************** size access *******************

  int             Size()              const {return nassocGeom.Size();}
  int             MapSize()   const {return assocGeomMap.Size();}
  int             MaxAssoc()  const {return maxAssocMap;}
  int                             GetnAssoc() const {return nassocGeom.GetCount();}

  //  ******************** main data access ****************
  void            GetAssocGeom(int            primaryGeomIndx,
                               SC_IntArray&   assocGeomIndexes) const;
  // assocGeomIndexes can be modified

  void            GetBufferedAssocGeom(int            primaryGeomIndx,
                                       SC_IntArray&   assocGeomIndexes) const;
  // assocGeomIndexes MUST NOT be modified

  void            SetAssocGeom(int                  primaryGeomIndx,
                               const SC_IntArray&   assocGeomIndexes);

  void            SetAssocGeom(const SC_IntMatrix&   assocGeomIndexes);

  void            AddAssocGeom(int  primaryGeomIndx,
                               int  assocIndex);  // adds to next
  void            AddAssocGeom(int  primaryGeomIndx,
                               const SC_IntArray& assocIndex);  // adds to next

  void            AddCompress();  // removes unused after add

  // in place replacement
  void            UpdateAssocGeom(int                  primaryGeomIndx,
                                  const SC_IntArray&   assocGeomIndexes);

  void            ReverseOrder(int primaryGeomIndx);
  // reverses order in assocGeomMap

  //  reverse mapping
  void            MakeAssocReverseMap(const SC_IntArray&   primaryReverseMap,
                                      SC_IntArray&         assocReverseMap,
                                      int                  maxAssocGeom);

  void            MakePrimaryReverseMap(SC_IntArray&         primaryReverseMap,
                                        const SC_IntArray&   assocReverseMap);

  void            ApplyReverseMap(const SC_IntArray&   primaryReverseMap,
                                  const SC_IntArray&   assocReverseMap);

  void            DeAlloc();

private:
  void            LocalCopy(const GeomMap& a);
};


#endif // C_GEOMETRY_H

