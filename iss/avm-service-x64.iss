; -- avm-service.iss --

; SEE THE DOCUMENTATION FOR DETAILS ON CREATING .ISS SCRIPT FILES!

#define Name "АВМ-Сервис"
#define GroupName "АВМ-Сервис"
#define EngName "АВМ-Сервис"
#define Version "0.1.3"
#define Publisher "EvelSoft"
#define URL "http://www.avmenergo.ru"
#define ExeName "AVM-Service.exe"
#define SetupName "AVM-Service-0.1.3-x64"
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
DefaultDirName={commonpf}\{#EngName}
DefaultGroupName={#GroupName}
; UninstallDisplayIcon={app}\MyProg.exe
Compression=lzma2
SolidCompression=yes
OutputDir="{#Prefix}\out\"
LicenseFile="{#Prefix}\license.txt"

OutputBaseFilename={#SetupName}

[Dirs]
Name: {userappdata}\{#EngName}

[Files]
Source: "{#Prefix}\src x64\Qt5Core.dll"; DestDir: "{app}"
Source: "{#Prefix}\src x64\Qt5Gui.dll"; DestDir: "{app}"
Source: "{#Prefix}\src x64\Qt5Network.dll"; DestDir: "{app}"
Source: "{#Prefix}\src x64\Qt5SerialPort.dll"; DestDir: "{app}"
Source: "{#Prefix}\src x64\Qt5Widgets.dll"; DestDir: "{app}"
Source: "{#Prefix}\src x64\platforms\qwindows.dll"; DestDir: "{app}\platforms"
Source: "{#Prefix}\3rdparty-x64\Qt5Xlsx.dll"; DestDir: "{app}"
Source: "{#Prefix}\3rdparty-x64\hidapi.dll"; DestDir: "{app}"
Source: "{#Prefix}\3rdparty-x64\liblzma.dll"; DestDir: "{app}"
Source: "{#Prefix}\3rdparty-x64\limereport.dll"; DestDir: "{app}"
Source: "{#Prefix}\3rdparty-x64\QtZint.dll"; DestDir: "{app}"
Source: "{#Prefix}\prg\{#ExeName}"; DestDir: "{app}"; DestName: {#ExeName}; Flags: ignoreversion
Source: "{#Prefix}\coma\ermsgs.dat"; DestDir: "{userappdata}\{#EngName}"; Flags: ignoreversion
Source: "{#Prefix}\coma\reports\*.*"; DestDir: "{userappdata}\{#EngName}"; Flags: ignoreversion
Source: "{#Prefix}\coma\images\*.*"; DestDir: "{app}\images"; Flags: ignoreversion
Source: "{#Prefix}\3rdparty-x64\vc_redist.x64.exe"; DestDir: "{tmp}"; Flags: deleteafterinstall
; Source: "{#Prefix}\coma\readme.txt"; DestDir: "{app}"

[Icons]
Name: "{group}\{#Name}"; Filename: "{app}\{#ExeName}"
;Name: "{group}\Руководство пользователя КОМА"; Filename: "{app}\КОМА Руководство пользователя.pdf"
Name: "{group}\Удалить программу {#Name}"; Filename: "{uninstallexe}"

[Run]
; add the Parameters, WorkingDir and StatusMsg as you wish, just keep here
; the conditional installation Check
Filename: "{tmp}\vc_redist.x64.exe"; Parameters: "/install /quiet /norestart"; StatusMsg: Устанавливается пакет MSVC2017 Redistributable...