; Script generated by the Inno Setup Script Wizard.
; SEE THE DOCUMENTATION FOR DETAILS ON CREATING INNO SETUP SCRIPT FILES!

[Setup]
AppName=ATTAC
AppVerName=ATTAC v4.1.11
AppVersion=4.1.11
AppPublisherURL=http://www.tw-attac.com
AppSupportURL=http://www.tw-attac.com/support.html
AppUpdatesURL=http://www.tw-attac.com/download.html
DefaultDirName={pf}\ATTAC-v4.1.11
UninstallDisplayIcon={app}\attac.exe
DefaultGroupName=ATTAC v4.1.11
AllowNoIcons=yes

;delete all installation files
[InstallDelete]
Type: files; Name: "{sys}\regina.dll"
Type: files; Name: "{sys}\ATTACv4.dll"

[Tasks]
Name: "desktopicon"; Description: "Create a &desktop icon"; GroupDescription: "Additional icons:"; MinVersion: 4,4
Name: "quicklaunchicon"; Description: "Create a &Quick Launch icon"; GroupDescription: "Additional icons:"; MinVersion: 4,4; Flags: unchecked

[Files]
; begin VC system files
; (Note: Scroll to the right to see the full lines!)
Source: "D:\Data\home\zbeckerman\tradewars\vcredist.exe\asycfilt.dll"; DestDir: "{sys}"; Flags: restartreplace uninsneveruninstall sharedfile
Source: "D:\Data\home\zbeckerman\tradewars\vcredist.exe\atla.dll";     DestName: "atl.dll"; DestDir: "{sys}"; MinVersion: 4, 0; Flags: restartreplace uninsneveruninstall sharedfile regserver
Source: "D:\Data\home\zbeckerman\tradewars\vcredist.exe\atlu.dll";     DestName: "atl.dll"; DestDir: "{sys}"; MinVersion: 0, 4; Flags: restartreplace uninsneveruninstall sharedfile regserver
Source: "D:\Data\home\zbeckerman\tradewars\vcredist.exe\comcat.dll";   DestDir: "{sys}"; Flags: restartreplace uninsneveruninstall sharedfile regserver
Source: "D:\Data\home\zbeckerman\tradewars\vcredist.exe\mfc42.dll";    DestDir: "{sys}"; Flags: restartreplace uninsneveruninstall sharedfile regserver
Source: "D:\Data\home\zbeckerman\tradewars\vcredist.exe\mfc42u.dll";   DestDir: "{sys}"; MinVersion: 0, 4; Flags: restartreplace uninsneveruninstall sharedfile regserver
Source: "D:\Data\home\zbeckerman\tradewars\vcredist.exe\msvcp60.dll";  DestDir: "{sys}"; Flags: restartreplace uninsneveruninstall sharedfile
Source: "D:\Data\home\zbeckerman\tradewars\vcredist.exe\msvcirt.dll";  DestDir: "{sys}"; Flags: restartreplace uninsneveruninstall sharedfile
Source: "D:\Data\home\zbeckerman\tradewars\vcredist.exe\msvcrt.dll";   DestDir: "{sys}"; Flags: restartreplace uninsneveruninstall sharedfile
Source: "D:\Data\home\zbeckerman\tradewars\vcredist.exe\oleaut32.dll"; DestDir: "{sys}"; Flags: restartreplace uninsneveruninstall sharedfile regserver
Source: "D:\Data\home\zbeckerman\tradewars\vcredist.exe\olepro32.dll"; DestDir: "{sys}"; Flags: restartreplace uninsneveruninstall sharedfile regserver
Source: "D:\Data\home\zbeckerman\tradewars\vcredist.exe\stdole2.tlb";  DestDir: "{sys}"; Flags: restartreplace uninsneveruninstall sharedfile regtypelib
; end VC system files

Source: "D:\Data\home\zbeckerman\tradewars\attac\attac.exe"; DestDir: "{app}"; Flags: ignoreversion
Source: "D:\Data\home\zbeckerman\tradewars\attac\scripts\StandardExplorer.erx"; DestDir: "{app}\scripts"; Flags: ignoreversion
Source: "D:\Data\home\zbeckerman\tradewars\attac\scripts\StandardBuyDump.erx"; DestDir: "{app}\scripts"; Flags: ignoreversion
Source: "D:\Data\home\zbeckerman\tradewars\attac\scripts\StandardBuyPlanetaryShields.erx"; DestDir: "{app}\scripts"; Flags: ignoreversion
Source: "D:\Data\home\zbeckerman\tradewars\attac\scripts\StandardColonizer.erx"; DestDir: "{app}\scripts"; Flags: ignoreversion
Source: "D:\Data\home\zbeckerman\tradewars\attac\scripts\StandardEscapeSector.erx"; DestDir: "{app}\scripts"; Flags: ignoreversion
Source: "D:\Data\home\zbeckerman\tradewars\attac\scripts\StandardEtherProbe.erx"; DestDir: "{app}\scripts"; Flags: ignoreversion
Source: "D:\Data\home\zbeckerman\tradewars\attac\scripts\StandardMegaJettison.erx"; DestDir: "{app}\scripts"; Flags: ignoreversion
Source: "D:\Data\home\zbeckerman\tradewars\attac\scripts\StandardMover.erx"; DestDir: "{app}\scripts"; Flags: ignoreversion
Source: "D:\Data\home\zbeckerman\tradewars\attac\scripts\StandardMoveMe.erx"; DestDir: "{app}\scripts"; Flags: ignoreversion
Source: "D:\Data\home\zbeckerman\tradewars\attac\scripts\StandardPairPort.erx"; DestDir: "{app}\scripts"; Flags: ignoreversion
Source: "D:\Data\home\zbeckerman\tradewars\attac\scripts\StandardPhotonAndMove.erx"; DestDir: "{app}\scripts"; Flags: ignoreversion
Source: "D:\Data\home\zbeckerman\tradewars\attac\scripts\StandardSellMove.erx"; DestDir: "{app}\scripts"; Flags: ignoreversion
Source: "D:\Data\home\zbeckerman\tradewars\attac\scripts\StandardSellTransport.erx"; DestDir: "{app}\scripts"; Flags: ignoreversion
Source: "D:\Data\home\zbeckerman\tradewars\attac\scripts\StandardTradeAtEveryPort.erx"; DestDir: "{app}\scripts"; Flags: ignoreversion
Source: "D:\Data\home\zbeckerman\tradewars\attac\scripts\StandardWorldSSM.erx"; DestDir: "{app}\scripts"; Flags: ignoreversion
Source: "D:\Data\home\zbeckerman\tradewars\attac\scripts\StandardTEditPortsToAmount.erx"; DestDir: "{app}\scripts"; Flags: ignoreversion
Source: "D:\Data\home\zbeckerman\tradewars\attac\scripts\StandardTEditAddToClosedGame.erx"; DestDir: "{app}\scripts"; Flags: ignoreversion
Source: "D:\Data\home\zbeckerman\tradewars\attac\scripts\StandardMineDisrupter.erx"; DestDir: "{app}\scripts"; Flags: ignoreversion
Source: "D:\Data\home\zbeckerman\tradewars\attac\scripts\StandardBustPlanets.erx"; DestDir: "{app}\scripts"; Flags: ignoreversion
Source: "D:\Data\home\zbeckerman\tradewars\attac\scripts\StandardDumpToPort.erx"; DestDir: "{app}\scripts"; Flags: ignoreversion
Source: "D:\Data\home\zbeckerman\tradewars\attac\scripts\library.erx"; DestDir: "{app}\scripts"; Flags: ignoreversion
Source: "D:\Data\home\zbeckerman\tradewars\attac\license.txt"; DestDir: "{app}"; Flags: ignoreversion
Source: "D:\Data\home\zbeckerman\tradewars\releaseDlls\libexpat.dll"; DestDir: "{app}";
Source: "D:\Data\home\zbeckerman\tradewars\releaseDlls\zlib1.dll"; DestDir: "{app}";
Source: "D:\Data\home\zbeckerman\tradewars\releaseDlls\TWCommon.dll"; DestDir: "{app}";
Source: "D:\Data\home\zbeckerman\tradewars\releaseDlls\ATTACv4.dll"; DestDir: "{app}";
Source: "D:\Data\home\zbeckerman\tradewars\releaseDlls\regina.dll"; DestDir: "{app}";

[Dirs]
Name: "{app}\datafiles"
Name: "{app}\scripts"

[Files]
Source: "datafiles\tradewarsNotes.txt"; DestDir: "{app}\datafiles"; Flags: onlyifdoesntexist uninsneveruninstall

[Icons]
Name: "{group}\ATTAC"; Filename: "{app}\attac.exe"; WorkingDir: "{app}"
Name: "{userdesktop}\ATTAC"; Filename: "{app}\attac.exe"; MinVersion: 4,4; Tasks: desktopicon; WorkingDir: "{app}"
Name: "{userappdata}\Microsoft\Internet Explorer\Quick Launch\ATTAC"; Filename: "{app}\attac.exe"; MinVersion: 4,4; Tasks: quicklaunchicon; WorkingDir: "{app}"

[Run]
Filename: "{app}\attac.exe"; Description: "Launch ATTAC"; Flags: nowait postinstall skipifsilent

