# Microsoft Developer Studio Project File - Name="MSC" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

CFG=MSC - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "MSC.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "MSC.mak" CFG="MSC - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "MSC - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "MSC - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "MSC - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "MSC_EXPORTS" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /MD /W3 /GX /O2 /I "../include/msc" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_USRDLL" /D "MSC_EXPORTS" /D "_UNICODE" /D "UNICODE" /Yu"stdafx.h" /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x804 /d "NDEBUG"
# ADD RSC /l 0x804 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /entry:"DllMain" /dll /machine:I386
# SUBTRACT LINK32 /nodefaultlib

!ELSEIF  "$(CFG)" == "MSC - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 2
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "MSC_EXPORTS" /Yu"stdafx.h" /FD /GZ /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /I "../include/msc" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_USRDLL" /D "MSC_EXPORTS" /D "_UNICODE" /D "UNICODE" /D "_WINDLL" /D "_AFXDLL" /Yu"stdafx.h" /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x804 /d "_DEBUG"
# ADD RSC /l 0x804 /d "_DEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /debug /machine:I386 /pdbtype:sept
# ADD LINK32 /nologo /dll /debug /machine:I386 /pdbtype:sept

!ENDIF 

# Begin Target

# Name "MSC - Win32 Release"
# Name "MSC - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\CMSCMessageQueue.cpp
# End Source File
# Begin Source File

SOURCE=.\Confusion.cpp
# End Source File
# Begin Source File

SOURCE=.\CPipe.cpp
# End Source File
# Begin Source File

SOURCE=.\CXCredit.cpp
# End Source File
# Begin Source File

SOURCE=.\CXMessage.cpp
# End Source File
# Begin Source File

SOURCE=.\CXMSCDispatcher.cpp
# End Source File
# Begin Source File

SOURCE=.\CXMSCProxy.cpp
# End Source File
# Begin Source File

SOURCE=.\CXMSCProxyImpl.cpp
# End Source File
# Begin Source File

SOURCE=.\CXMSCProxyThread.cpp
# End Source File
# Begin Source File

SOURCE=.\MSC.cpp
# End Source File
# Begin Source File

SOURCE=.\MSC.def
# End Source File
# Begin Source File

SOURCE=.\MSCHandler.cpp
# End Source File
# Begin Source File

SOURCE=.\MSCListener.cpp
# End Source File
# Begin Source File

SOURCE=.\MSCManager.cpp
# End Source File
# Begin Source File

SOURCE=.\Profile.cpp
# End Source File
# Begin Source File

SOURCE=.\RSA.cpp
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=.\SHA_1.cpp
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=.\SMSCMessageMap.cpp
# End Source File
# Begin Source File

SOURCE=.\SMSCSubscriberList.cpp
# End Source File
# Begin Source File

SOURCE=.\SMSCSubscriberQueue.cpp
# End Source File
# Begin Source File

SOURCE=.\SPipe.cpp
# End Source File
# Begin Source File

SOURCE=.\StdAfx.cpp
# ADD CPP /Yc
# End Source File
# Begin Source File

SOURCE=.\Subscriber.cpp
# End Source File
# Begin Source File

SOURCE=.\XBuffer.cpp
# End Source File
# Begin Source File

SOURCE=.\XMessageBuffer.cpp
# End Source File
# Begin Source File

SOURCE=.\XMutex.cpp
# End Source File
# Begin Source File

SOURCE=.\XSynchronization.cpp
# End Source File
# Begin Source File

SOURCE=.\XThread.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\CMSCMessageQueue.h
# End Source File
# Begin Source File

SOURCE=.\Confusion.h
# End Source File
# Begin Source File

SOURCE=.\CPipe.h
# End Source File
# Begin Source File

SOURCE=.\CXCredit.h
# End Source File
# Begin Source File

SOURCE=.\CXMessage.h
# End Source File
# Begin Source File

SOURCE=.\CXMSCDispatcher.h
# End Source File
# Begin Source File

SOURCE=.\CXMSCProxy.h
# End Source File
# Begin Source File

SOURCE=.\CXMSCProxyImpl.h
# End Source File
# Begin Source File

SOURCE=.\CXMSCProxyThread.h
# End Source File
# Begin Source File

SOURCE=.\MSC.h
# End Source File
# Begin Source File

SOURCE=.\MSCHandler.h
# End Source File
# Begin Source File

SOURCE=.\MSCListener.h
# End Source File
# Begin Source File

SOURCE=.\MSCManager.h
# End Source File
# Begin Source File

SOURCE=.\Profile.h
# End Source File
# Begin Source File

SOURCE=.\RSA.h
# End Source File
# Begin Source File

SOURCE=.\SHA_1.h
# End Source File
# Begin Source File

SOURCE=.\SMSCMessageMap.h
# End Source File
# Begin Source File

SOURCE=.\SMSCSubscriberList.h
# End Source File
# Begin Source File

SOURCE=.\SMSCSubscriberQueue.h
# End Source File
# Begin Source File

SOURCE=.\SPipe.h
# End Source File
# Begin Source File

SOURCE=.\StdAfx.h
# End Source File
# Begin Source File

SOURCE=.\Subscriber.h
# End Source File
# Begin Source File

SOURCE=.\XBuffer.h
# End Source File
# Begin Source File

SOURCE=.\XCredit.h
# End Source File
# Begin Source File

SOURCE=.\XMessageBuffer.h
# End Source File
# Begin Source File

SOURCE=.\XMMSC.h
# End Source File
# Begin Source File

SOURCE=.\XMutex.h
# End Source File
# Begin Source File

SOURCE=.\XSynchronization.h
# End Source File
# Begin Source File

SOURCE=.\XThread.h
# End Source File
# End Group
# Begin Group "../include/msc"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\include\msc\MSCDef.h
# End Source File
# Begin Source File

SOURCE=..\include\msc\XMessage.h
# End Source File
# Begin Source File

SOURCE=..\include\msc\XMSCDispatcher.h
# End Source File
# Begin Source File

SOURCE=..\include\msc\XMSCProxy.h
# End Source File
# End Group
# End Target
# End Project
