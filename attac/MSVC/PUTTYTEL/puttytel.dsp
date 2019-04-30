# Microsoft Developer Studio Project File - Name="puttytel" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=puttytel - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "puttytel.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "puttytel.mak" CFG="puttytel - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "puttytel - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "puttytel - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "puttytel - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /YX /FD /c
# ADD CPP /nologo /MT /W3 /GX /O2 /D "P_FREE" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "WIN32S_COMPAT" /D "_NO_OLDNAMES" /D "NO_MULTIMON" /D "ATTAC_CODE" /D NUMBERRELEASE=4 /D NUMBERMAJOR=1 /D NUMBERMINOR=11 /D RELEASE=4.1.11 /D REGINARELEASE="Rexx Version 3.3 (Apr 05 2004 13:59:00)" /YX /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x809 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /machine:I386
# ADD LINK32 advapi32.lib comctl32.lib comdlg32.lib gdi32.lib imm32.lib shell32.lib user32.lib winmm.lib winspool.lib ws2_32.lib ..\..\..\releaseDlls\libexpat.lib ..\..\..\releaseDlls\regina.lib ..\..\..\TWCommon\Release\TWCommon.lib ..\..\..\attacv4Dll\Release\ATTACv4.lib /nologo /subsystem:windows /machine:I386 /out:"..\..\attac.exe"
# SUBTRACT LINK32 /pdb:none

!ELSEIF  "$(CFG)" == "puttytel - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /YX /FD /GZ /c
# ADD CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /D "MEMWATCH" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "WIN32S_COMPAT" /D "_NO_OLDNAMES" /D "NO_MULTIMON" /D "ATTAC_CODE" /D NUMBERRELEASE=4 /D NUMBERMAJOR=1 /D NUMBERMINOR=11 /D RELEASE=4.1.11 /D REGINARELEASE="Rexx Version 3.3 (Apr 05 2004 13:59:00)" /YX /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x809 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept
# ADD LINK32 comctl32.lib imm32.lib winmm.lib ws2_32.lib ..\..\..\releaseDlls\libexpat.lib ..\..\..\releaseDlls\regina.lib ..\..\..\TWCommon\Debug\TWCommon.lib ..\..\..\attacv4Dll\Debug\ATTACv4.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /debug /machine:I386 /out:"..\..\attac.exe" /pdbtype:sept
# SUBTRACT LINK32 /pdb:none

!ENDIF 

# Begin Target

# Name "puttytel - Win32 Release"
# Name "puttytel - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Group "attac_src"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\attac_src\attacGUI.c
# End Source File
# Begin Source File

SOURCE=..\..\attac_src\attacUtils.c
# End Source File
# Begin Source File

SOURCE=..\..\attac_src\autoupdate.c
# End Source File
# Begin Source File

SOURCE=..\..\attac_src\bubbleFinder.c
# End Source File
# Begin Source File

SOURCE=..\..\attac_src\cim.c
# End Source File
# Begin Source File

SOURCE=..\..\attac_src\encrypt.c
# End Source File
# Begin Source File

SOURCE=..\..\attac_src\encryptScripts.c
# End Source File
# Begin Source File

SOURCE=..\..\attac_src\hash.c
# End Source File
# Begin Source File

SOURCE=..\..\attac_src\info.c
# End Source File
# Begin Source File

SOURCE=..\..\attac_src\linklist.c
# End Source File
# Begin Source File

SOURCE=..\..\attac_src\map.c
# End Source File
# Begin Source File

SOURCE=..\..\attac_src\mapqueue.c
# End Source File
# Begin Source File

SOURCE=..\..\attac_src\parsedatathread.c
# End Source File
# Begin Source File

SOURCE=..\..\attac_src\queue.c
# End Source File
# Begin Source File

SOURCE=..\..\attac_src\remoteConnection.c
# End Source File
# Begin Source File

SOURCE=..\..\attac_src\rexx.c
# End Source File
# Begin Source File

SOURCE=..\..\attac_src\shortestPath.c
# End Source File
# Begin Source File

SOURCE=..\..\attac_src\statusthread.c
# End Source File
# Begin Source File

SOURCE=..\..\attac_src\structs.c
# End Source File
# Begin Source File

SOURCE=..\..\attac_src\twx.c
# End Source File
# Begin Source File

SOURCE=..\..\attac_src\twxSync.c
# End Source File
# Begin Source File

SOURCE=..\..\attac_src\util.c
# End Source File
# Begin Source File

SOURCE=..\..\attac_src\writeMemory.c
# End Source File
# Begin Source File

SOURCE=..\..\attac_src\xml.c
# End Source File
# Begin Source File

SOURCE=..\..\attac_src\ztm.c
# End Source File
# End Group
# Begin Group "memwatch_src"

# PROP Default_Filter ""
# Begin Source File

SOURCE="..\..\..\3PP\MemWatch-2.7.1\memwatch.c"
# End Source File
# End Group
# Begin Source File

SOURCE=..\..\.\be_nossh.c

!IF  "$(CFG)" == "puttytel - Win32 Release"

!ELSEIF  "$(CFG)" == "puttytel - Win32 Debug"

# ADD CPP /Zi

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\.\cmdline.c
# End Source File
# Begin Source File

SOURCE=..\..\.\config.c
# End Source File
# Begin Source File

SOURCE=..\..\.\dialog.c
# End Source File
# Begin Source File

SOURCE=..\..\.\ldisc.c
# End Source File
# Begin Source File

SOURCE=..\..\.\ldiscucs.c
# End Source File
# Begin Source File

SOURCE=..\..\.\logging.c
# End Source File
# Begin Source File

SOURCE=..\..\.\misc.c
# End Source File
# Begin Source File

SOURCE=..\..\.\nocproxy.c
# End Source File
# Begin Source File

SOURCE=..\..\.\pproxy.c
# End Source File
# Begin Source File

SOURCE=..\..\.\printing.c
# End Source File
# Begin Source File

SOURCE=..\..\.\proxy.c
# End Source File
# Begin Source File

SOURCE=..\..\.\raw.c
# End Source File
# Begin Source File

SOURCE=..\..\.\rlogin.c
# End Source File
# Begin Source File

SOURCE=..\..\.\settings.c
# End Source File
# Begin Source File

SOURCE=..\..\.\sizetip.c
# End Source File
# Begin Source File

SOURCE=..\..\.\telnet.c
# End Source File
# Begin Source File

SOURCE=..\..\.\terminal.c
# End Source File
# Begin Source File

SOURCE=..\..\.\tree234.c
# End Source File
# Begin Source File

SOURCE=..\..\.\unicode.c
# End Source File
# Begin Source File

SOURCE=..\..\.\version.c
# End Source File
# Begin Source File

SOURCE=..\..\.\wcwidth.c
# End Source File
# Begin Source File

SOURCE=..\..\.\wincfg.c
# End Source File
# Begin Source File

SOURCE=..\..\.\winctrls.c
# End Source File
# Begin Source File

SOURCE=..\..\.\windefs.c
# End Source File
# Begin Source File

SOURCE=..\..\.\windlg.c
# End Source File
# Begin Source File

SOURCE=..\..\.\window.c
# End Source File
# Begin Source File

SOURCE=..\..\.\winmisc.c
# End Source File
# Begin Source File

SOURCE=..\..\.\winnet.c
# End Source File
# Begin Source File

SOURCE=..\..\.\winstore.c
# End Source File
# Begin Source File

SOURCE=..\..\.\winutils.c
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Group "attac_hdr"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\attac_src\attacGlobal.h
# End Source File
# Begin Source File

SOURCE=..\..\attac_src\attacGUI.h
# End Source File
# Begin Source File

SOURCE=..\..\attac_src\attacUtils.h
# End Source File
# Begin Source File

SOURCE=..\..\attac_src\autoupdate.h
# End Source File
# Begin Source File

SOURCE=..\..\attac_src\bubbleFinder.h
# End Source File
# Begin Source File

SOURCE=..\..\attac_src\cim.h
# End Source File
# Begin Source File

SOURCE=..\..\attac_src\encrypt.h
# End Source File
# Begin Source File

SOURCE=..\..\attac_src\hash.h
# End Source File
# Begin Source File

SOURCE=..\..\attac_src\info.h
# End Source File
# Begin Source File

SOURCE=..\..\attac_src\linklist.h
# End Source File
# Begin Source File

SOURCE=..\..\attac_src\map.h
# End Source File
# Begin Source File

SOURCE=..\..\attac_src\mapqueue.h
# End Source File
# Begin Source File

SOURCE=..\..\attac_src\parsedatathread.h
# End Source File
# Begin Source File

SOURCE=..\..\attac_src\queue.h
# End Source File
# Begin Source File

SOURCE=..\..\attac_src\remoteConnection.h
# End Source File
# Begin Source File

SOURCE=..\..\attac_src\resource.h
# End Source File
# Begin Source File

SOURCE=..\..\attac_src\rexx.h
# End Source File
# Begin Source File

SOURCE=..\..\attac_src\shortestPath.h
# End Source File
# Begin Source File

SOURCE=..\..\attac_src\statusthread.h
# End Source File
# Begin Source File

SOURCE=..\..\attac_src\structs.h
# End Source File
# Begin Source File

SOURCE=..\..\attac_src\twx.h
# End Source File
# Begin Source File

SOURCE=..\..\attac_src\twxSync.h
# End Source File
# Begin Source File

SOURCE=..\..\attac_src\util.h
# End Source File
# Begin Source File

SOURCE=..\..\attac_src\win_res.h
# End Source File
# Begin Source File

SOURCE=..\..\attac_src\writeMemory.h
# End Source File
# Begin Source File

SOURCE=..\..\attac_src\xml.h
# End Source File
# Begin Source File

SOURCE=..\..\attac_src\ztm.h
# End Source File
# End Group
# Begin Group "memwatch_hdr"

# PROP Default_Filter ""
# Begin Source File

SOURCE="..\..\MemWatch-2.7.1\memwatch.h"
# End Source File
# End Group
# Begin Source File

SOURCE=..\..\charset\charset.h
# End Source File
# Begin Source File

SOURCE=..\..\.\dialog.h
# End Source File
# Begin Source File

SOURCE=..\..\.\int64.h
# End Source File
# Begin Source File

SOURCE=..\..\.\ldisc.h
# End Source File
# Begin Source File

SOURCE=..\..\mac\macstuff.h
# End Source File
# Begin Source File

SOURCE=..\..\.\misc.h
# End Source File
# Begin Source File

SOURCE=..\..\.\network.h
# End Source File
# Begin Source File

SOURCE=..\..\.\proxy.h
# End Source File
# Begin Source File

SOURCE=..\..\.\putty.h
# End Source File
# Begin Source File

SOURCE=..\..\.\puttymem.h
# End Source File
# Begin Source File

SOURCE=..\..\.\puttyps.h
# End Source File
# Begin Source File

SOURCE=..\..\.\ssh.h
# End Source File
# Begin Source File

SOURCE=..\..\.\storage.h
# End Source File
# Begin Source File

SOURCE=..\..\.\terminal.h
# End Source File
# Begin Source File

SOURCE=..\..\.\tree234.h
# End Source File
# Begin Source File

SOURCE=..\..\unix\unix.h
# End Source File
# Begin Source File

SOURCE=..\..\.\win_res.h
# End Source File
# Begin Source File

SOURCE=..\..\.\winhelp.h
# End Source File
# Begin Source File

SOURCE=..\..\.\winstuff.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# Begin Group "attac_res"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\attac_src\attac.ico
# End Source File
# Begin Source File

SOURCE=..\..\attac_src\openfolder.ico
# End Source File
# Begin Source File

SOURCE=..\..\attac_src\poweredByRexx.bmp
# End Source File
# Begin Source File

SOURCE=..\..\attac_src\win_res.rc
# End Source File
# End Group
# Begin Source File

SOURCE=..\..\.\putty.ico
# End Source File
# Begin Source File

SOURCE=..\..\.\puttycfg.ico
# End Source File
# End Group
# Begin Source File

SOURCE=..\..\attac_src\attac.mft
# End Source File
# End Target
# End Project
