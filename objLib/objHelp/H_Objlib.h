///////////////////////////////////////////////////////////////////////////////////
//
// H_Objlib.h
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

// ObjLib
// IDH_OBJ_BASE                        5500
// IDH_OBJ_DPO                         5500
// IDH_OBJ_LPO                         5700
// IDH_OBJ_PPO                         5800
// IDH_OBJ_UPO                         5900

#ifndef H_OBJLIB_H
#define H_OBJLIB_H

#define IDH_OBJ_DPO_CDFEXTRACT                          5500
#define IDH_OBJ_DPO_COMBINETABLES                       5501
#define IDH_OBJ_DPO_CREATECURVE                         5502
#define IDH_OBJ_DPO_CREATEDATALABEL                     5503
#define IDH_OBJ_DPO_CREATEREAL                          5504
#define IDH_OBJ_DPO_CREATEXYARRAY                       5505
#define IDH_OBJ_DPO_CUBEEXTRACTGRID                     5506
#define IDH_OBJ_DPO_CUBEHISTOGRAM                       5507
#define IDH_OBJ_DPO_CUBEMATH                            5508
#define IDH_OBJ_DPO_CUBENORMALIZE                       5509
#define IDH_OBJ_DPO_CUBESCALETRANSFORM                  5510
#define IDH_OBJ_DPO_CUBESTATISTICS                      5511
#define IDH_OBJ_DPO_CUBETOINDEX                         5512
#define IDH_OBJ_DPO_CURVEINTERP                         5513
#define IDH_OBJ_DPO_ENTERTABLE                          5514
#define IDH_OBJ_DPO_ENTERXY                             5515
#define IDH_OBJ_DPO_GRIDEXTRACTXY                       5516
#define IDH_OBJ_DPO_GRIDHISTOGRAM                       5517
#define IDH_OBJ_DPO_GRIDINTERPOLATE                     5518
#define IDH_OBJ_DPO_GRIDMATH                            5519
#define IDH_OBJ_DPO_GRIDNORMALIZE                       5520
#define IDH_OBJ_DPO_GRIDSCALETRANSFORM                  5521
#define IDH_OBJ_DPO_GRIDSTATISTICS                      5522
#define IDH_OBJ_DPO_INTERPTABLECOLUMN                   5523
#define IDH_OBJ_DPO_READCUBEDATA                        5524
#define IDH_OBJ_DPO_READCURVEARRAY                      5525
#define IDH_OBJ_DPO_READGRIDDATA                        5526
#define IDH_OBJ_DPO_READLABELARRAY                      5527
#define IDH_OBJ_DPO_READTABLE                           5528
#define IDH_OBJ_DPO_READXY                              5529
#define IDH_OBJ_DPO_READXYARRAY                         5530
#define IDH_OBJ_DPO_REALSCALETRANSFORM                  5531
#define IDH_OBJ_DPO_REALTOTABLE                         5532
#define IDH_OBJ_DPO_SELECTCURVE                         5533
#define IDH_OBJ_DPO_SELECTXY                            5534
#define IDH_OBJ_DPO_TABLEADD                            5535
#define IDH_OBJ_DPO_TABLECOLSCALETRANSFORM              5536
#define IDH_OBJ_DPO_TABLECOLUMNCORRELATION              5537
#define IDH_OBJ_DPO_TABLECOLUMNMATH                     5538
#define IDH_OBJ_DPO_TABLECONCAT                         5539
#define IDH_OBJ_DPO_TABLEFULLCORRELATION                5540
#define IDH_OBJ_DPO_TABLEHISTOGRAM                      5541
#define IDH_OBJ_DPO_TABLEINTERPVAL                      5542
#define IDH_OBJ_DPO_TABLEQUANTILE                       5543
#define IDH_OBJ_DPO_TABLERANGEEXTRACT                   5544
#define IDH_OBJ_DPO_TABLEROWINDEXLOGIC                  5545
#define IDH_OBJ_DPO_TABLEROWSTATISTICS                  5546
#define IDH_OBJ_DPO_TABLESTATISTICS                     5547
#define IDH_OBJ_DPO_TABLETOGRID                         5548
#define IDH_OBJ_DPO_TABLETOREAL                         5549
#define IDH_OBJ_DPO_TABLETOXY                           5550
#define IDH_OBJ_DPO_TABLETRANSPOSE                      5551
#define IDH_OBJ_DPO_VIEWTABLE                           5552
#define IDH_OBJ_DPO_VIEWXY                              5553
#define IDH_OBJ_DPO_XYADDNOISE                          5554
#define IDH_OBJ_DPO_XYARRAYSCALETRANSFORM               5555
#define IDH_OBJ_DPO_XYDUALSCALETRANSFORM                5556
#define IDH_OBJ_DPO_XYFOURIER                           5557
#define IDH_OBJ_DPO_XYHISTOGRAM                         5558
#define IDH_OBJ_DPO_XYINTEGRATE                         5559
#define IDH_OBJ_DPO_XYMATH                              5560
#define IDH_OBJ_DPO_XYQUANTILE                          5561
#define IDH_OBJ_DPO_XYRANGEEXTRACT                      5562
#define IDH_OBJ_DPO_XYREDUCTION                         5563
#define IDH_OBJ_DPO_XYREMOVEDUPLICATE                   5564
#define IDH_OBJ_DPO_XYSCALETRANSFORM                    5565
#define IDH_OBJ_DPO_XYSMOOTHFILTER                      5566
#define IDH_OBJ_DPO_XYSTATISTICS                        5567
#define IDH_OBJ_DPO_XYTOCDF                             5568
#define IDH_OBJ_DPO_XYTOXYARRAY                         5569
#define IDH_OBJ_DPO_XYTRANSPOSE                         5570

#define IDH_OBJ_LPO_TABLE                               5700
#define IDH_OBJ_LPO_TABLESTATISTICS                     5701
#define IDH_OBJ_LPO_XYDATA                              5702

#define IDH_OBJ_PPO_2DANALYTIC                          5800
#define IDH_OBJ_PPO_2DMULTTABLES                        5801
#define IDH_OBJ_PPO_2DTABLEHISTOGRAM                    5802
#define IDH_OBJ_PPO_2DTABLESERIES                       5803
#define IDH_OBJ_PPO_2DXYDATA                            5804
#define IDH_OBJ_PPO_2DXYARRAY                           5805
#define IDH_OBJ_PPO_3DTABLESERIES                       5806
#define IDH_OBJ_PPO_3DXYDATA                            5807
#define IDH_OBJ_PPO_CUBECOLORBLOCK                      5808
#define IDH_OBJ_PPO_CUBEISOVOLUME                       5809
#define IDH_OBJ_PPO_CUBECOLORPOINT                      5810
#define IDH_OBJ_PPO_ENTERXYONPLOT                       5811
#define IDH_OBJ_PPO_GRIDCOLORBLOCKFILL                  5812
#define IDH_OBJ_PPO_GRIDCOLORPOINT                      5813
#define IDH_OBJ_PPO_GRIDCOLORRANGEFILL                  5814
#define IDH_OBJ_PPO_GRIDCONTOUR                         5815
#define IDH_OBJ_PPO_GRIDFISHNET                         5816
#define IDH_OBJ_PPO_NSXYLABELS                          5817
#define IDH_OBJ_PPO_NSXYZLABELS                         5818
#define IDH_OBJ_PPO_TABLEHORSETAIL                      5819
#define IDH_OBJ_PPO_XYHISTOGRAM                         5820
#define IDH_OBJ_PPO_XYHORSETAIL                         5821

#define IDH_OBJ_UPO_WRITECURVEARRAY                     5900
#define IDH_OBJ_UPO_WRITEGRID                           5901
#define IDH_OBJ_UPO_WRITELABELARRAY                     5902
#define IDH_OBJ_UPO_WRITETABLE                          5903
#define IDH_OBJ_UPO_WRITEXY                             5904
#define IDH_OBJ_UPO_WRITEXYARRAY                        5905

#endif // !H_OBJLIB_H

