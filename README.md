nSIGHTS
=======

This repository is the official open source version of nSIGHTS, the MS-Windows (32- and 64-bit) n-dimensional Statistical Inverse Graphical Hydraulic Test Simulator.

nSIGHTS (n-dimensional Statistical Inverse Graphical Hydraulic Test Simulator) is a comprehensive well test analysis software package. It provides a user-interface, a well test analysis model and many tools to analyze both field and simulated data. The well test analysis model simulates a single-phase, one-dimensional, radial/non-radial flow regime, with a borehole at the center of the modeled flow system. 

The nSIGHTS system consists of two independent MS-Windows GUI applications: nPre and nPost. The two applications differ in function, but are similar in their interface. nPre assists the user in model set-up, data pre-processing, running of the model and diagnostics of the simulation. nPost post-processes results calculated in nPre and stored in post-processing files.

Installers
----------

The "Setup_nSIGHTS" and "Setup_nSIGHTS_64" directories contain Windows installers (.msi files) for the 32-bit and 64-bit versions of nSIGHTS.  If you are interested in downloading and using nSIGHTS, but are not interested in using git to clone the repository, you can download the repository as a .zip file (button along right of site), which includes the windows installer files.

Documentation
-------------

nSIGHTS includes a large amount of documentation in Windows Help format (found as [nPre64.chm](exe/x64/Release/nPre64.chm) and [nPost64.chm](exe/x64/Release/nPost64.chm) in the exe/x64/Release directory (follow link and click "view raw" to download help files), and also available from inside the program at run time), and as design documents in the "documents" folder. 

- [The User's Manual](documents/nSIGHTS_Users_Manual_V2.41a.pdf) (follow link and click "view raw" to download pdf) is a 495-page document that includes description of most features in the nSIGHTS program (nPre, nPost, and nPreX), and  description of how to carry out basic workflows (import data, make plots, etc.).

- [The Program Design Document](documents/nSIGHTS_Program_Design_V2.41a.pdf) (follow link and click "view raw" to download pdf) is a 54-page description of the structure of the C++ source code, including listing of GUI menu items.

- [The Functional Description and Theoretical Development Document](documents/nSIGHTS_Program_Design_V2.41a_AppendixA_Theory.pdf) (follow link and click "view raw" to download pdf) is a 66-page document describing the theory and numerical implementation details used in nSIGHTS.  This document is an appendix to the Program Design Document given above. 
