; -- avtuk-s_f x86.iss --

; SEE THE DOCUMENTATION FOR DETAILS ON CREATING .ISS SCRIPT FILES!

#define Name "АВМ-Сервис"
#define GroupName "АВМ-Сервис"
#define EngName "AVM-Service"
#define Version "0.0.40"
#define Publisher "EvelSoft"
#define URL "http://www.avmenergo.ru"
#define ExeName "AVM-Service.exe"
#define SetupName "AVM-Service-0.0.40-x86"
#define Prefix "D:\out-5.14_x32"

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
Source: "{#Prefix}\coma\*.dll"; DestDir: "{app}"
Source: "{#Prefix}\coma\platforms\qwindows.dll"; DestDir: "{app}\platforms"
Source: "{#Prefix}\coma\{#ExeName}"; DestDir: "{app}"; DestName: {#ExeName}; Flags: ignoreversion
Source: "{#Prefix}\coma\ermsgs.dat"; DestDir: "{userappdata}\{#EngName}"; Flags: ignoreversion
Source: "{#Prefix}\coma\reports\*.*"; DestDir: "{userappdata}\{#EngName}"; Flags: ignoreversion
Source: "{#Prefix}\coma\images\*.*"; DestDir: "{app}\images"; Flags: ignoreversion
Source: "{#Prefix}\vc_redist.x86.exe"; DestDir: "{tmp}"; Flags: deleteafterinstall
Source: "{#Prefix}\coma\readme.txt"; DestDir: "{app}"

[Icons]
Name: "{group}\{#Name}"; Filename: "{app}\{#ExeName}"
;Name: "{group}\Руководство пользователя КОМА"; Filename: "{app}\КОМА Руководство пользователя.pdf"
Name: "{group}\Удалить программу {#Name}"; Filename: "{uninstallexe}"

[Run]
Filename: "{tmp}\vc_redist.x86.exe"; Parameters: "/install /quiet /norestart"; StatusMsg: Устанавливается пакет MSVC2017 Redistributable...