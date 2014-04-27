# Microsoft Developer Studio Project File - Name="Layers32" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=Layers32 - Win32 Debug DLL
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "Layers32.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "Layers32.mak" CFG="Layers32 - Win32 Debug DLL"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "Layers32 - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "Layers32 - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE "Layers32 - Win32 Debug DLL" (based on "Win32 (x86) Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "Layers32 - Win32 Release"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 6
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_AFXDLL" /Yu"stdafx.h" /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x419 /d "NDEBUG" /d "_AFXDLL"
# ADD RSC /l 0x419 /d "NDEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /machine:I386
# ADD LINK32 fftw.lib htmlhelp.lib /nologo /subsystem:windows /machine:I386 /out:"CrazyLayers.exe"

!ELSEIF  "$(CFG)" == "Layers32 - Win32 Debug"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 6
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /Yu"stdafx.h" /FD /GZ /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_AFXDLL" /FR /Yu"stdafx.h" /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x419 /d "_DEBUG" /d "_AFXDLL"
# ADD RSC /l 0x419 /d "_DEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept
# ADD LINK32 fftw.lib htmlhelp.lib /nologo /subsystem:windows /debug /machine:I386 /out:"CrazyLayers.exe" /pdbtype:sept

!ELSEIF  "$(CFG)" == "Layers32 - Win32 Debug DLL"

# PROP BASE Use_MFC 5
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Layers32___Win32_Debug_DLL"
# PROP BASE Intermediate_Dir "Layers32___Win32_Debug_DLL"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 6
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Layers32___Win32_Debug_DLL"
# PROP Intermediate_Dir "Layers32___Win32_Debug_DLL"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /FR /Yu"stdafx.h" /FD /GZ /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_AFXDLL" /FR /Yu"stdafx.h" /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x419 /d "_DEBUG"
# ADD RSC /l 0x419 /d "_DEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 fftw.lib /nologo /subsystem:windows /debug /machine:I386 /out:"CrazyLayers.exe" /pdbtype:sept
# ADD LINK32 fftw.lib htmlhelp.lib /nologo /subsystem:windows /dll /debug /machine:I386 /out:"../jkdb/CrazyLayers.dll" /pdbtype:sept
# SUBTRACT LINK32 /pdb:none

!ENDIF 

# Begin Target

# Name "Layers32 - Win32 Release"
# Name "Layers32 - Win32 Debug"
# Name "Layers32 - Win32 Debug DLL"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\Ampl.cpp
# End Source File
# Begin Source File

SOURCE=.\AtomParams.cpp
# End Source File
# Begin Source File

SOURCE=.\AtomsArray.cpp
# End Source File
# Begin Source File

SOURCE=.\blt.cpp
# End Source File
# Begin Source File

SOURCE=.\ChildFrm.cpp
# End Source File
# Begin Source File

SOURCE=.\ChildView.cpp
# End Source File
# Begin Source File

SOURCE=.\DialogAniz.cpp
# End Source File
# Begin Source File

SOURCE=.\DialogAtoms.cpp
# End Source File
# Begin Source File

SOURCE=.\DialogChess.cpp
# End Source File
# Begin Source File

SOURCE=.\DialogCircle.cpp
# End Source File
# Begin Source File

SOURCE=.\DialogConstanta.cpp
# End Source File
# Begin Source File

SOURCE=.\DialogDefokus.cpp
# End Source File
# Begin Source File

SOURCE=.\DialogEx.cpp
# End Source File
# Begin Source File

SOURCE=.\DialogFractal.cpp
# End Source File
# Begin Source File

SOURCE=.\DialogInfo.cpp
# End Source File
# Begin Source File

SOURCE=.\DialogKlin.cpp
# End Source File
# Begin Source File

SOURCE=.\DialogKvazi.cpp
# End Source File
# Begin Source File

SOURCE=.\DialogKvazi2.cpp
# End Source File
# Begin Source File

SOURCE=.\DialogNameL2DB.cpp
# End Source File
# Begin Source File

SOURCE=.\DialogNoise.cpp
# End Source File
# Begin Source File

SOURCE=.\DialogObob.cpp
# End Source File
# Begin Source File

SOURCE=.\DialogPath.cpp
# End Source File
# Begin Source File

SOURCE=.\DialogRect.cpp
# End Source File
# Begin Source File

SOURCE=.\DialogSP2Type.cpp
# End Source File
# Begin Source File

SOURCE=.\DialogTurbo.cpp
# End Source File
# Begin Source File

SOURCE=.\DialogWave.cpp
# End Source File
# Begin Source File

SOURCE=.\Dib.cpp
# End Source File
# Begin Source File

SOURCE=.\fparams.cpp
# End Source File
# Begin Source File

SOURCE=.\LatDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\LatGen.cpp
# End Source File
# Begin Source File

SOURCE=.\LatProgr.cpp
# End Source File
# Begin Source File

SOURCE=.\Layers32.cpp
# End Source File
# Begin Source File

SOURCE=.\Layers32.rc
# End Source File
# Begin Source File

SOURCE=.\MainFrm.cpp
# End Source File
# Begin Source File

SOURCE=.\mend.cpp
# End Source File
# Begin Source File

SOURCE=.\ModelFrame.cpp
# End Source File
# Begin Source File

SOURCE=.\ModelFrameLayers.cpp
# End Source File
# Begin Source File

SOURCE=.\MyStatic.cpp
# End Source File
# Begin Source File

SOURCE=.\ObjectFrame.cpp
# End Source File
# Begin Source File

SOURCE=.\ObjectFrameCernic.cpp
# End Source File
# Begin Source File

SOURCE=.\ObjectFrameDefokus.cpp
# End Source File
# Begin Source File

SOURCE=.\SpectrumFrame.cpp
# End Source File
# Begin Source File

SOURCE=.\SpectrumFrameLayers.cpp
# End Source File
# Begin Source File

SOURCE=.\StdAfx.cpp
# ADD CPP /Yc"stdafx.h"
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\Ampl.h
# End Source File
# Begin Source File

SOURCE=.\AtomParams.h
# End Source File
# Begin Source File

SOURCE=.\AtomsArray.h
# End Source File
# Begin Source File

SOURCE=.\blt.h
# End Source File
# Begin Source File

SOURCE=.\ChildFrm.h
# End Source File
# Begin Source File

SOURCE=.\ChildView.h
# End Source File
# Begin Source File

SOURCE=.\DialogAniz.h
# End Source File
# Begin Source File

SOURCE=.\DialogAtoms.h
# End Source File
# Begin Source File

SOURCE=.\DialogChess.h
# End Source File
# Begin Source File

SOURCE=.\DialogCircle.h
# End Source File
# Begin Source File

SOURCE=.\DialogConstanta.h
# End Source File
# Begin Source File

SOURCE=.\DialogDefokus.h
# End Source File
# Begin Source File

SOURCE=.\DialogEx.h
# End Source File
# Begin Source File

SOURCE=.\DialogFractal.h
# End Source File
# Begin Source File

SOURCE=.\DialogInfo.h
# End Source File
# Begin Source File

SOURCE=.\DialogKlin.h
# End Source File
# Begin Source File

SOURCE=.\DialogKvazi.h
# End Source File
# Begin Source File

SOURCE=.\DialogKvazi2.h
# End Source File
# Begin Source File

SOURCE=.\DialogNameL2DB.h
# End Source File
# Begin Source File

SOURCE=.\DialogNoise.h
# End Source File
# Begin Source File

SOURCE=.\DialogObob.h
# End Source File
# Begin Source File

SOURCE=.\DialogPath.h
# End Source File
# Begin Source File

SOURCE=.\DialogRect.h
# End Source File
# Begin Source File

SOURCE=.\DialogSP2Type.h
# End Source File
# Begin Source File

SOURCE=.\DialogTurbo.h
# End Source File
# Begin Source File

SOURCE=.\DialogWave.h
# End Source File
# Begin Source File

SOURCE=.\Dib.h
# End Source File
# Begin Source File

SOURCE=.\fparams.h
# End Source File
# Begin Source File

SOURCE=.\htmlhelp.h
# End Source File
# Begin Source File

SOURCE=.\Interfaces.h
# End Source File
# Begin Source File

SOURCE=.\LatDlg.h
# End Source File
# Begin Source File

SOURCE=.\LatGen.h
# End Source File
# Begin Source File

SOURCE=.\LatProgr.h
# End Source File
# Begin Source File

SOURCE=.\Layers32.h
# End Source File
# Begin Source File

SOURCE=.\MainFrm.h
# End Source File
# Begin Source File

SOURCE=.\mend.h
# End Source File
# Begin Source File

SOURCE=.\ModelFrame.h
# End Source File
# Begin Source File

SOURCE=.\ModelFrameLayers.h
# End Source File
# Begin Source File

SOURCE=.\MyImage.h
# End Source File
# Begin Source File

SOURCE=.\MyStatic.h
# End Source File
# Begin Source File

SOURCE=.\ObjectFrame.h
# End Source File
# Begin Source File

SOURCE=.\ObjectFrameCernic.h
# End Source File
# Begin Source File

SOURCE=.\ObjectFrameDefokus.h
# End Source File
# Begin Source File

SOURCE=.\other.h
# End Source File
# Begin Source File

SOURCE=.\Resource.h
# End Source File
# Begin Source File

SOURCE=.\SpectrumFrame.h
# End Source File
# Begin Source File

SOURCE=.\SpectrumFrameLayers.h
# End Source File
# Begin Source File

SOURCE=.\StdAfx.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=.\bitmap1.bmp
# End Source File
# Begin Source File

SOURCE=.\res\bitmap1.bmp
# End Source File
# Begin Source File

SOURCE=.\bmp00001.bmp
# End Source File
# Begin Source File

SOURCE=.\res\bmp00001.bmp
# End Source File
# Begin Source File

SOURCE=.\bmp00002.bmp
# End Source File
# Begin Source File

SOURCE=.\res\bmp00002.bmp
# End Source File
# Begin Source File

SOURCE=.\bmp00003.bmp
# End Source File
# Begin Source File

SOURCE=.\res\bmp00003.bmp
# End Source File
# Begin Source File

SOURCE=.\bmp00004.bmp
# End Source File
# Begin Source File

SOURCE=.\res\bmp00004.bmp
# End Source File
# Begin Source File

SOURCE=.\bmp00005.bmp
# End Source File
# Begin Source File

SOURCE=.\res\bmp00005.bmp
# End Source File
# Begin Source File

SOURCE=.\bmp00006.bmp
# End Source File
# Begin Source File

SOURCE=.\res\bmp00006.bmp
# End Source File
# Begin Source File

SOURCE=.\bmp00007.bmp
# End Source File
# Begin Source File

SOURCE=.\bmp00008.bmp
# End Source File
# Begin Source File

SOURCE=.\bmp00009.bmp
# End Source File
# Begin Source File

SOURCE=.\bmp00010.bmp
# End Source File
# Begin Source File

SOURCE=.\bmp00011.bmp
# End Source File
# Begin Source File

SOURCE=.\bmp00012.bmp
# End Source File
# Begin Source File

SOURCE=.\bmp00013.bmp
# End Source File
# Begin Source File

SOURCE=.\bmp00014.bmp
# End Source File
# Begin Source File

SOURCE=.\bmp00015.bmp
# End Source File
# Begin Source File

SOURCE=.\bmp00016.bmp
# End Source File
# Begin Source File

SOURCE=.\bmp00017.bmp
# End Source File
# Begin Source File

SOURCE=.\bmp00018.bmp
# End Source File
# Begin Source File

SOURCE=.\bmp00019.bmp
# End Source File
# Begin Source File

SOURCE=.\bmp00020.bmp
# End Source File
# Begin Source File

SOURCE=.\res\bmp_a3.bmp
# End Source File
# Begin Source File

SOURCE=.\res\bmp_form.bmp
# End Source File
# Begin Source File

SOURCE=.\res\icon1.ico
# End Source File
# Begin Source File

SOURCE=.\res\Layers32.ico
# End Source File
# Begin Source File

SOURCE=.\res\Layers32.rc2
# End Source File
# Begin Source File

SOURCE=.\res\Toolbar.bmp
# End Source File
# End Group
# Begin Source File

SOURCE=.\comments.txt
# End Source File
# Begin Source File

SOURCE=.\ReadMe.txt
# End Source File
# End Target
# End Project
