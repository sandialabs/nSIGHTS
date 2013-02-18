#nPreX
#Makefile for linux cluster

##############################################################
#	OBJECT FILES

AFX = StdAfx.o	

ALLOC = A_DPO_BlendColorMap.o	       \
A_DPO_LinColorMap.o		       \
A_DPO_MergeColorMap.o		       \
A_DPO_PenSet.o			       \
A_DPO_ReadColorMap.o		       \
A_UPO_WriteColorMap.o		       \
A_DPO_BarometricCompensation.o	       \
A_DPO_BasicTimeExtract.o	       \
A_DPO_BasicSequenceFit.o	       \
A_DPO_Derivative.o		       \
A_DPO_ExtractSequenceInterval.o        \
A_DPO_PulseNormalize.o		       \
A_DPO_ReadMiniTroll.o		       \
A_DPO_TimeProcess.o		       \
A_DPO_CDFExtract.o		       \
A_DPO_CreateCurve.o		       \
A_DPO_CreateXYArray.o		       \
A_DPO_CreateReal.o		       \
A_DPO_CubeToIndex.o		       \
A_DPO_CubeScaleTransform.o	       \
A_DPO_CubeStatistics.o		       \
A_DPO_CubeMath.o		       \
A_DPO_CubeExtractGrid.o		       \
A_DPO_CubeNormalize.o		       \
A_DPO_CubeHistogram.o		       \
A_DPO_CurveInterp.o		       \
A_DPO_EnterTable.o		       \
A_DPO_EnterXY.o			       \
A_DPO_GridMath.o		       \
A_DPO_GridScaleTransform.o	       \
A_DPO_GridHistogram.o		       \
A_DPO_GridNormalize.o		       \
A_DPO_GridExtractXY.o		       \
A_DPO_GridStatistics.o		       \
A_DPO_ReadCurveArray.o		       \
A_DPO_ReadLabelArray.o		       \
A_DPO_ReadGridData.o		       \
A_DPO_ReadXYDataArray.o		       \
A_DPO_ReadXY.o			       \
A_DPO_RealToTable.o		       \
A_DPO_RealScaleTransform.o	       \
A_DPO_ReadTable.o		       \
A_DPO_SelectCurve.o		       \
A_DPO_SelectXY.o		       \
A_DPO_TableFullCorrelation.o	       \
A_DPO_TableHistogram.o		       \
A_DPO_TableRowIndexLogic.o	       \
A_DPO_TableRangeExtract.o	       \
A_DPO_TableQuantile.o		       \
A_DPO_TableInterpVal.o		       \
A_DPO_TableStatistics.o		       \
A_DPO_TableRowStatistics.o	       \
A_DPO_TableToReal.o		       \
A_DPO_TableColumnMath.o		       \
A_DPO_TableColumnCorrelation.o	       \
A_DPO_TableToXY.o		       \
A_DPO_TableColScaleTransform.o	       \
A_DPO_TableAdd.o		       \
A_DPO_ViewGrid.o		       \
A_DPO_ViewTable.o		       \
A_DPO_ViewXY.o			       \
A_DPO_XYAddNoise.o		       \
A_DPO_XYDualScaleTransform.o	       \
A_DPO_XYFourier.o		       \
A_DPO_XYHistogram.o		       \
A_DPO_XYIntegrate.o		       \
A_DPO_XYMath.o			       \
A_DPO_XYQuantile.o		       \
A_DPO_XYReduction.o		       \
A_DPO_XYRangeExtract.o		       \
A_DPO_XYRemoveDuplicate.o	       \
A_DPO_XYScaleTransform.o	       \
A_DPO_XYSmoothFilter.o		       \
A_DPO_XYStatistics.o		       \
A_DPO_XYToCDF.o			       \
A_DPO_XYTranspose.o		       \
A_UPO_WriteXYDataArray.o	       \
A_UPO_WriteCurveArray.o		       \
A_UPO_WriteXY.o			       \
A_UPO_WriteTable.o		       \
A_DPO_CompositeFit.o		       \
A_DPO_SelectRTRangeCube.o	       \
A_DPO_SelectRTRangeGrid.o	       \
A_DPO_SingleFit.o		       
						       
APP = AppFuncObj.o	\
nPreFile.o		\
nPreRoot.o		\
AppPages.o					       
						       
					       
VAR = C_CurveFile.o	\
C_DataCapture.o		\
C_FileOutput.o		\
C_Parameter.o		\
C_Sequence.o		\
C_TestZoneCurve.o	\
C_Units.o		\
E_Parameter.o		\
G_CalcParameter.o	\
G_Control.o		\
G_CurveFiles.o		\
G_DataCapture.o		\
G_OptOutput.o		\
G_OutputFiles.o		\
G_Parameter.o		\
G_PreConfigFile.o	\
G_ProfileOutput.o	\
G_RangeOutput.o		\
G_Sequence.o		\
G_XYOutput.o		
			
CMDLINE = G_VersionX.o	\
MainMPI.o		\
RunControl.o		\
Collect.o
			
RUN = RunCommon.o

SIM =C_SimCore.o	\
C_WorkCommon.o		\
C_WorkDataCapture.o	\
C_WorkParameter.o	\
C_WorkSequence.o	\
C_DynamicTS.o		\
C_SingleWell.o			   \
C_SingleWell1D.o		   \
C_SingleWell1DLinear.o		   \
C_SingleWell1DNonLinear.o	   \
C_SingleWell1DNonLinearGas.o	   \
C_SingleWell1DNonLinearLiquid.o	   \
C_SingleWell2D.o		   \
C_SingleWell2DConfined.o	   \
C_SingleWell2DUnconfined.o	   \
C_StaticCondensation.o		   \
G_ParameterCurveOutput.o	   \
G_SimulatorRadiiOutput.o	   
 				   
NPRE_OBJ = $(AFX) $(ALLOC) $(APP) $(VAR) $(CMDLINE) $(RUN) $(SIM) 

##############################################################
#	VARIABLES

NPRE_IN_DIR = $(APPDEV_DIR)/AppDev/nPre
NPRE_AFX_DIR = $(NPRE_IN_DIR)
NPRE_APP_DIR = $(NPRE_IN_DIR)/App
NPRE_VAR_DIR = $(NPRE_IN_DIR)/Var
NPRE_CMDLINE_DIR = $(NPRE_IN_DIR)/CmdLine
NPRE_RUN_DIR = $(NPRE_IN_DIR)/Run
NPRE_SIM_DIR = $(NPRE_IN_DIR)/Sim

NPRE_DIR = $(NPRE_AFX_DIR) $(NPRE_IN_DIR) \
$(NPRE_APP_DIR) $(NPRE_VAR_DIR)	\
$(NPRE_CMDLINE_DIR) $(NPRE_RUN_DIR) \
$(NPRE_SIM_DIR) 

NPRE_OUTPUT_DIR = $(APPDEV_DIR)/obj/nPreX_Release

vpath %.o = $(NPRE_OUTPUT_DIR)
vpath %.cpp = $(NPRE_DIR)\
:$(GEN_IN_DIR)/genAlloc\
:$(OBJ_IN_DIR)/objAlloc\
:$(OS_IN_DIR)/osAlloc\
:$(NS_IN_DIR)/nsAlloc

##############################################################
#	compile source code

#each command line is sent to shell separately - consequently, 
#for cd to work, it needs to be on same line, separated by a semi-colon

#% is a pattern search - allows only one file compiled out of list
#$^ is the name of all prerequistites - due to pattern search, only one file 
#$@ is the name of the target

#$(NPRE_OBJ) : %.o: %.cpp
#	@$(CC) $(CFLAGS) -c $^
#	@mv -f $@ $(NPRE_OUTPUT_DIR)				      


$(AFX) : %.o: $(NPRE_AFX_DIR)/%.cpp 
	@$(CC) $(CFLAGS) -c $^
	@mv -f $@ $(NPRE_OUTPUT_DIR)				      

$(ALLOC) : %.o: %.cpp 
	@$(CC) $(CFLAGS) -c $^
	@mv -f $@ $(NPRE_OUTPUT_DIR)				      

$(APP) : %.o: $(NPRE_APP_DIR)/%.cpp 
	@$(CC) $(CFLAGS) -c $^
	@mv -f $@ $(NPRE_OUTPUT_DIR)				      

$(VAR) : %.o: $(NPRE_VAR_DIR)/%.cpp 
	@$(CC) $(CFLAGS) -c $^
	@mv -f $@ $(NPRE_OUTPUT_DIR)				      

$(CMDLINE) : %.o: $(NPRE_CMDLINE_DIR)/%.cpp 
	@$(CC) $(CFLAGS) -c $^
	@mv -f $@ $(NPRE_OUTPUT_DIR)				      

$(RUN) : %.o: $(NPRE_RUN_DIR)/%.cpp 
	@$(CC) $(CFLAGS) -c $^
	@mv -f $@ $(NPRE_OUTPUT_DIR)

$(SIM) : %.o: $(NPRE_SIM_DIR)/%.cpp 
	@$(CC) $(CFLAGS) -c $^
	@mv -f $@ $(NPRE_OUTPUT_DIR)

