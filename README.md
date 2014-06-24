nSIGHTS
=======

This repository is the official open source version of nSIGHTS, the MS-Windows (32- and 64-bit) n-dimensional Statistical Inverse Graphical Hydraulic Test Simulator.

nSIGHTS (n-dimensional Statistical Inverse Graphical Hydraulic Test Simulator) is a comprehensive well test analysis software package. It provides a user-interface, a well test analysis model and many tools to analyze both field and simulated data. The well test analysis model simulates a single-phase, one-dimensional, radial/non-radial flow regime, with a borehole at the center of the modeled flow system. 

The nSIGHTS system consists of two independent MS-Windows GUI applications: nPre and nPost. The two applications differ in function, but are similar in their interface. nPre assists the user in model set-up, data pre-processing, running of the model and diagnostics of the simulation. nPost post-processes results calculated in nPre and stored in post-processing files.

Installers
----------

The "Setup_nSIGHTS" and "Setup_nSIGHTS_64" directories contain Windows installers (.msi files) for the 32-bit and 64-bit versions of nSIGHTS.  If you are interested in downloading and using nSIGHTS, but are not interested in using git to clone the repository, you can download the repository as a .zip file (button along right of site), which includes the windows installer files.

The 32-bit version of nSIGHTS requires Windows XP SP2 or newer. The 64-bit version of nSIGHTS requires Windows Vistas or newer.

Documentation
-------------

nSIGHTS includes a large amount of documentation in Windows Help format (found as [nPre64.chm](exe/x64/Release/nPre64.chm) and [nPost64.chm](exe/x64/Release/nPost64.chm) in the exe/x64/Release directory (follow link and click "view raw" to download help files), and also available from inside the program at run time), and as design documents in the "documents" folder. 

- [The User's Manual](documents/nSIGHTS_Users_Manual_V2.41a.pdf) (follow link and click "view raw" to download pdf) is a 495-page document that includes description of most features in the nSIGHTS program (nPre, nPost, and nPreX), and includes a step-by-step tutorial for setting up a problem (illustrating optimization, perturbation approach, and plotting of results).

- [The Program Design Document](documents/nSIGHTS_Program_Design_V2.41a.pdf) (follow link and click "view raw" to download pdf) is a 54-page description of the structure of the C++ source code, including listing of GUI menu items.

- [The Functional Description and Theoretical Development Document](documents/nSIGHTS_Program_Design_V2.41a_AppendixA_Theory.pdf) (follow link and click "view raw" to download pdf) is a 66-page document describing the theory and numerical implementation details used in nSIGHTS.  This document is an appendix to the Program Design Document given above. 

Build Instructions
-----------------

To build nSIGHTS from source requires at least:

- Microsoft Visual Studio 2010. It may work with newer versions (VS2012 or VS2013), but has not been tried. There was work involved in the transition from VS2008 to VS2010, so it is probably not trivial.  "nSights64.sln" is the main file with all the VS2010 configuration, and pointers to the separate subprojects.  When you first load this file, VS2010 scans all the source files and creates an SQL database.

- MPICH binary installers (http://www.mpich.org/downloads/). You will need both 32-bit and 64-bit versions to compile the respective versions of nSIGHTS.  Install these into their default locations, and the nSIGHTS build process should work ("C:\Program Files\MPICH2" and "C:\Program Files (x86)\MPICH2").

- LAPACK/BLAS for Windows (http://icl.cs.utk.edu/lapack-for-windows/lapack/index.html). Get the MinGW-built versions of the libraries (both 32-bit and 64-bit). Extract them into the directories "C:\repos\mingw-x86-blas-lapack" and "C:\repos\mingw-x64-blas-lapack" so the nSIGHTS build process can find them.

- MinGW (http://www.mingw.org/). You will want make sure the package/options you install at least includes gfortran 4.8 compiler (provides libgcc_s_ds2-1.dll, libgfortran-3.dll, and libquadmath-0.dll). The build should find these if they are installed into "C:\MinGW\bin"

- Microsoft Help Compiler (http://www.helpandmanual.com/downloads_mscomp.html). This provides hhc.exe, needed to rebuild the html documentation.

- Time and patience. It takes a while to build (>1 hour), once you get all your configuration properly setup. Gettin g everything setup is definitely non-trivial, and there may be important steps or settings which have not mentioned (please contact us if you find something we failed to mention or include).  Our intention is to make this easy to use, and _possible_ to build.  It won't be easy to build.

There are two primary targets for building "Setup_nSIGHTS" and "Setup_nSIGHTS_64bit". The other targets are libraries used to construct the nPre.exe, nPreX.exe, and nPost.exe binaries that get rolled into msi installers by building the two primary targets.  There are Win32 and x64 versions of each library, as well as debug and release versions. The primary targets only build the release versions, though.  Sometimes building requires a few tries to get something to work, this may be due to improper specification of dependencies.   

