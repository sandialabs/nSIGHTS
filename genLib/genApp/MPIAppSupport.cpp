///////////////////////////////////////////////////////////////////////////////////
//
// MPIAppSupport.cpp
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
// DESCRIPTION: Code which defines and implements the basic framework user-interface (UI)
//              and provides OS platform specific implementation of common facilities
//              such as copy/paste and configuration file I/O.
//
///////////////////////////////////////////////////////////////////////////////////

#ifdef ADCONSOLEAPP

#include <iostream>

#include <genClass/SC_IntArray.h>
#include <genClass/SC_BoolArray.h>
#include <genClass/SC_StringArray.h>
#include <genClass/SC_RealFormat.h>

#include <genClass/U_Msg.h>
#include <genClass/U_Str.h>
#include <genApp/G_Version.h>
#include <genClass/C_MPISupport.h>

#include <genApp/DebugUtil.h>

#include <genApp/MPIAppSupport.h>

//undefs required by mpich2 - kluge recommended by them!
#undef SEEK_SET
#undef SEEK_END
#undef SEEK_CUR
#include <mpi.h>

namespace MPIAppSupport {

    const int mpiTag_NextRun     = 89;
    const int mpiTag_RunComplete = 99;
    const int mpiTag_AllComplete = 79;
    const int mpiTag_ProcName    = 69;

    enum {maxNameLen = MPI_MAX_PROCESSOR_NAME + 1};
    static char processorName[maxNameLen];
    static  int maxMPIRank = 1;

    void CheckMPI(int mpiRetVal, const char* opDesc)
    {
        if (mpiRetVal == MPI_SUCCESS)
            return;
        NodeFile::mpiDebugRun = true;
        const char* errStr = "unknown MPI error";
        switch (mpiRetVal) {
        case MPI_ERR_COMM  : {errStr = "MPI_ERR_COMM - invalid communicator"; break;}
        case MPI_ERR_TYPE  : {errStr = "MPI_ERR_TYPE - invalid data type"; break;}
        case MPI_ERR_COUNT : {errStr = "MPI_ERR_COUNT - invalid count argument"; break;}
        case MPI_ERR_TAG   : {errStr = "MPI_ERR_TAG - invalid tag argument"; break;}
        case MPI_ERR_RANK  : {errStr = "MPI_ERR_RANK - invalid rank argument"; break;}
        default : ;
        }
        GenAppInfoMsg("CheckMPI", errStr);
        GenAppInternalError(opDesc);
    }

    static SC_IntArray indexCount;


    static void MPIMasterSampling(int nSlaves, int maxMPIRank)
    {
        int runIndx;
        indexCount.AllocAndFill(nSlaves, 0);
        //initialize & start first set of processes
        for (int i = 0; i  < nSlaves; i++)
            {
                char tempStr[80];
                ConcatInt(i+1, "starting slave ", tempStr, 3, 80);
                GenAppInfoMsg("MPIMasterSampling", tempStr);
                // start run on each slave
                runIndx = i;
                CheckMPI(MPI_Send(&runIndx, 1, MPI_INT, i + 1, mpiTag_NextRun, MPI_COMM_WORLD), "Starting slave");
                indexCount[i]++;
            }

        int currProcComplete;
        int currRank;
        int nRuns = MPIGetNCases();

        MPI_Status status;
        while (runIndx < (nRuns - 1))
            {
                // get next slave with run completed
                CheckMPI(MPI_Recv(&currProcComplete, 1, MPI_INT, MPI_ANY_SOURCE, mpiTag_RunComplete, MPI_COMM_WORLD, &status), "Get next available slave");

                currRank = status.MPI_SOURCE;
                char tempStr[80];
                ConcatInt(currRank, "next run on slave ", tempStr, 3, 80);
                GenAppInfoMsg("MPIMasterSampling", tempStr);

                // start next run on available slave
                runIndx++;
                CheckMPI(MPI_Send(&runIndx, 1, MPI_INT, currRank, mpiTag_NextRun, MPI_COMM_WORLD), "Send case index to slave");
                indexCount[currRank - 1]++;
            }

        GenAppInfoMsg("MPIMasterSampling", "all runs sent, waiting on last");

        //finish last simulation on each slave
        SC_BoolArray finished(maxMPIRank, false);
        bool allFinished = false;

        // -1 indicates all done ...
        runIndx = -1;
        while (!allFinished)
            {
                // get next slave with run completed
                CheckMPI(MPI_Recv(&currProcComplete, 1, MPI_INT, MPI_ANY_SOURCE, mpiTag_RunComplete, MPI_COMM_WORLD, &status), "Getting slave case complete");
                currRank = status.MPI_SOURCE;

                // send final signal to complete simulation on slave
                CheckMPI(MPI_Send(&runIndx, 1, MPI_INT, currRank, mpiTag_NextRun, MPI_COMM_WORLD), "Sending -1 case to next slave");

                // set slave as finished
                finished[currRank - 1] = true;

                // check for all done
                allFinished = true;
                for (int i = 0; i < nSlaves; i++)
                    {
                        if (!finished[i])
                            allFinished = false;
                    }
            }

        GenAppInfoMsg("MPIMasterSampling", "all slaves told to complete");

        //receive signal that complete simulation has completed on each slave
        // note: slaves will spin until all slaves are complete

        allFinished = false;
        finished.FillToAlloc(false);
        while (!allFinished)
            {
                // get next slave with all completed
                CheckMPI(MPI_Recv(&currProcComplete, 1, MPI_INT, MPI_ANY_SOURCE, mpiTag_AllComplete, MPI_COMM_WORLD, &status), "Get slave all complete");
                currRank = status.MPI_SOURCE;

                // set slave as finished
                finished[currRank - 1] = true;

                // check for all done
                allFinished = true;
                for (int i = 0; i < nSlaves; i++)
                    {
                        if (!finished[i])
                            allFinished = false;
                    }
            }

        GenAppInfoMsg("MPIMasterSampling", "all slaves complete");

    }


    static void MPIMasterRun(const char* MPIappID, int maxMPIRank)
    {
        // all msgs for master ...
        NodeFile::mpiDebugRun = true;

        SC_StringArray machineNames;
        machineNames.SetStringLen(maxNameLen);
        machineNames.Alloc(maxMPIRank);
        machineNames.SetString(processorName, 0);

        // get slave processor name
        MPI_Status status;
        for (int i = 1; i < maxMPIRank; i++)
            {
                CheckMPI(MPI_Recv(processorName, maxNameLen, MPI_CHAR, i, mpiTag_ProcName, MPI_COMM_WORLD, &status), "Master get slave names");

                int currRank = status.MPI_SOURCE;
                if (currRank != i)
                    GenAppInternalError("Unexpected slave rank on slave processor name");

                machineNames.SetString(processorName, i);
            }

        time_t stTime;
        bool stTimeOK = false;
        char timeLab[80];
        const char* timeFormat = "%x %X";
        if (time(&stTime) != -1)
            {
#ifdef MSCVS2005
                tm tmOut;
                localtime_s(&tmOut, &stTime);
                strftime(timeLab, 80, timeFormat, &tmOut);
#else
                strftime(timeLab, 80, timeFormat, localtime(&stTime));
#endif
                GenAppInfoMsg("Master start time", timeLab);
                stTimeOK = true;
            }

        bool isOptRun = MPIOptimizationRun();
        int nRuns;
        if (isOptRun)
            {
                GenAppInfoMsg("MPI Run","Optimization only");
            }
        else
            {
                nRuns = MPIGetNCases();
                char nrunstr[10];
                IntToString(nRuns, nrunstr, 10);
                GenAppInfoMsg("Number of runs", nrunstr);
            }
        int nSlaves = maxMPIRank - 1;
        if (nRuns < nSlaves)
            nSlaves = nRuns;

        MPIMasterInit(nSlaves);

        if (isOptRun)
            {
                MPIRunOptimizationMaster(nSlaves);
            }
        else
            {

                MPIMasterSampling(nSlaves, maxMPIRank);

            }

        GenAppInfoMsg(MPIappID, "master run complete");

        MPIMasterCleanup(nSlaves);

        if (!isOptRun)
            {
                using namespace std;
                cout << endl << "Slave Summary" << endl;
                SC_IntArray processorCount(maxMPIRank, 0);
                for (int i = 0; i < nSlaves; i++)
                    {
                        cout << "Slave " << i + 1 << " processed " << indexCount[i] << " runs" << endl;
                        processorCount[machineNames.SearchForKey(machineNames[i + 1])] += indexCount[i];
                    }

                cout << endl << "Machine Summary" << endl;
                for (int i = 0; i < maxMPIRank; i++)
                    if (processorCount[i] > 0)
                        cout << "Machine " << machineNames[i] << " processed " << processorCount[i] << " runs" << endl;
                cout << endl;
            }

        GenAppInfoMsg(MPIappID, "all cases completed OK");

        time_t endTime;
        if (stTimeOK && (time(&endTime) != -1))
            {
                // write start time again
                GenAppInfoMsg("Master start time", timeLab);

                // and end time
#ifdef MSCVS2005
                tm tmOut;
                localtime_s(&tmOut, &endTime);
                strftime(timeLab, 80, timeFormat, &tmOut);
#else
                strftime(timeLab, 80, timeFormat, localtime(&endTime));
#endif
                GenAppInfoMsg("Master end time", timeLab);

                double deltaMin = difftime(endTime, stTime); // / 60.0;
                SC_DecFormat minConv(2);
                minConv.RealToString(deltaMin, timeLab, 80);

                GenAppInfoMsg("Elapsed seconds", timeLab);
            }
    }

    static void MPISlaveSampling(int slaveRank)
    {
        MPI_Status status;
        int indx;
        while (true)
            {
                // get index of case to run
                CheckMPI(MPI_Recv(&indx, 1, MPI_INT, 0, mpiTag_NextRun, MPI_COMM_WORLD, &status), "Slave get case index");

                // all done indicator ?
                if (indx < 0)
                    break;

#ifndef DEBUGACTIVE
                std::cout << slaveRank  << ": Case#" << indx << std::endl;
#endif
                MPIRunOneCase(indx);

                //send message that run is complete
                int complete = 1;
                CheckMPI(MPI_Send(&complete, 1, MPI_INT, 0, mpiTag_RunComplete, MPI_COMM_WORLD), "Slave send case complete");
            }
    }


    static void MPISlaveRun(int slaveRank)
    {
        // start by sending processor name to master
        CheckMPI(MPI_Send(processorName, maxNameLen, MPI_CHAR, 0, mpiTag_ProcName, MPI_COMM_WORLD), "Slave send name");

        MPISlaveInit(slaveRank);

        if (MPIOptimizationRun())
            MPIRunOptimizationSlave(slaveRank);
        else
            MPISlaveSampling(slaveRank);

        MPISlaveCleanup();

        std::cout << slaveRank  << ": completed OK" << std::endl;

        //send message that all is complete
        int complete = 1;
        CheckMPI(MPI_Send(&complete, 1, MPI_INT, 0, mpiTag_AllComplete, MPI_COMM_WORLD), "Slave send all complete");
    }

    int MPIMain(int argc, char *argv[], const char* appID)
    {
        MPI_Init(&argc, &argv);
        int rank;
        MPI_Comm_rank( MPI_COMM_WORLD, &rank);
        //      int maxMPIRank;
        MPI_Comm_size( MPI_COMM_WORLD, &maxMPIRank);

        // set processor name
        int nameLen;
        MPI_Get_processor_name(processorName, &nameLen);
        processorName[nameLen] = '\0';

        NodeFile::mpiNodeRank = rank;
        char rankstr[10];
        IntToString(rank, rankstr, 10);

        // turn on start up messages
        NodeFile::mpiDebugRun = true;

        if (rank == 0)
            {
                GenAppInfoMsg(appVersionGlobals::versionID, appVersionGlobals::versionDate);
                if (appVersionGlobals::isBeta)
                    GenAppWarningMsg(appVersionGlobals::versionID, "is not Qualified!!");

                char msg[10];
                IntToString(maxMPIRank, msg, 10);
                GenAppInfoMsg("Number of processes", msg);
                GenAppInfoMsg("Master processor", processorName);

                if (maxMPIRank < 2)
                    GenAppInternalError("Must have at least two processes (first process is master).");
            }
        else
            {
                GenAppInfoMsg("Slave processor", processorName);
            }

        // turn off start up messages
        NodeFile::mpiDebugRun = false;
        MPIAppInit(argc, argv, rank > 0);

        if (rank == 0)
            {
                MPIMasterRun(appID, maxMPIRank);
            }
        else
            {
                MPISlaveRun(rank);
            }
        GenAppInfoMsg("MPI simulation", "complete");

        MPI_Barrier(MPI_COMM_WORLD);
        MPI_Finalize();
        exit(1);
        return 0;
    }

    int MPIGetNmaxMPIRank()
    {
        return maxMPIRank;
    }

    void MPIOptStopSlaves(int nSlaves)
    {
        int nCount = nSlaves;
        int fitIndex = -1;
        //      int runIndex = -1;
        int complete;
        int mpiTag_FitIndex = 101;
        int mpiTag_RunComplete = 122;
        int slaveRank;
        MPI_Status status;
        while (nCount >= 1){
            CheckMPI(MPI_Recv(&complete, 1, MPI_INT, MPI_ANY_SOURCE, mpiTag_RunComplete, MPI_COMM_WORLD, &status), "Slave send case complete");
            if (complete){
                slaveRank = status.MPI_SOURCE;
                CheckMPI(MPI_Send(&fitIndex, 1, MPI_INT, slaveRank, mpiTag_FitIndex, MPI_COMM_WORLD), "Send stop command to slaves");
                std::cout<<"In Function MPIOptStopSlaves: sending -1 fitIndex to slave "<<slaveRank<<std::endl;
            }
            nCount--;
        }
    }


};

#endif