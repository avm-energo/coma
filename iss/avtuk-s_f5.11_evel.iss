; -- avtuk-s_f5.11 x64.iss --

; SEE THE DOCUMENTATION FOR DETAILS ON CREATING .ISS SCRIPT FILES!

#define Name "АВТУК-Сервис ПР"
#define GroupName "АВТУК-Сервис"
#define EngName "AVTUK-S"
#define Version "2.2.267"
#define Publisher "AVMEnergo"
#define URL "http://www.avmenergo.ru"
#define ExeName "avtuks-F.exe"
#define SetupName "avtuks-F-2.2.267"
#define Prefix "D:\Progs\out"

[CustomMessages]
Version={#Version}

[Languages]
Name: Russian; MessagesFile: "compiler:Languages\Russian.isl" 

[InstallDelete]
Type: files; Name: "{app}\*.dll"
Type: filesandordirs; Name: "{app}\platforms"
Type: files; Name: "{app}\{#ExeName}"
; Type: files; Name: "{app}\КОМА Руководство пользователя.pdf"

[Setup]
UsePreviousLanguage=No
AppName={#Name}
AppVersion={#Version}
AppPublisher={#Publisher}
AppPublisherURL={#URL}
AppSupportURL={#URL}
AppUpdatesURL={#URL}
DefaultDirName={pf}\{#EngName}
DefaultGroupName={#GroupName}
; UninstallDisplayIcon={app}\MyProg.exe
Compression=lzma2
SolidCompression=yes
OutputDir="{#Prefix}\out\"
LicenseFile="{#Prefix}\coma\license.txt"

OutputBaseFilename={#SetupName}

[Dirs]
Name: {userappdata}\{#EngName}

[Files]
Source: "{#Prefix}\3rdparty-x86\hidapi.dll"; DestDir: "{app}"
Source: "{#Prefix}\3rdparty-x86\liblzma.dll"; DestDir: "{app}"
Source: "{#Prefix}\qt5.11.1-x86\*.dll"; DestDir: "{app}"
Source: "{#Prefix}\qt5.11.1-x86\platforms\qwindows.dll"; DestDir: "{app}\platforms"
Source: "{#Prefix}\coma\{#ExeName}"; DestDir: "{app}"; DestName: {#ExeName}; Flags: ignoreversion
Source: "{#Prefix}\coma\ermsgs.dat"; DestDir: "{userappdata}\{#EngName}"; Flags: ignoreversion
Source: "{#Prefix}\coma\reports\*.*"; DestDir: "{userappdata}\{#EngName}"; Flags: ignoreversion
Source: "{#Prefix}\coma\images\*.*"; DestDir: "{app}\images"; Flags: ignoreversion
Source: "{#Prefix}\common\vc2017\vc_redist.x86.exe"; DestDir: "{tmp}"; Flags: deleteafterinstall
; Source: "coma\pdf\КОМА Руководство пользователя.pdf"; DestDir: "{app}" 

[Icons]
Name: "{group}\{#Name}"; Filename: "{app}\{#ExeName}"
;Name: "{group}\Руководство пользователя КОМА"; Filename: "{app}\КОМА Руководство пользователя.pdf"
Name: "{group}\Удалить программу {#Name}"; Filename: "{uninstallexe}"

[Run]
; add the Parameters, WorkingDir and StatusMsg as you wish, just keep here
; the conditional installation Check
Filename: "{tmp}\vc_redist.x64.exe"; Parameters: "/install /quiet /norestart"