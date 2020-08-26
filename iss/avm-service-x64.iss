; Script generated by the Inno Setup Script Wizard.
; SEE THE DOCUMENTATION FOR DETAILS ON CREATING INNO SETUP SCRIPT FILES!

#define Name "���-������"
#define GroupName "���-������"
#define EngName "AVM-Service"
#define Publisher "EvelSoft"
#define URL "http://www.avmenergo.ru"
#define ExeName "AVM-Service.exe"
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;#define Prefix "C:\Progs\out"
#define ARCH "win64"
#define QT_DIR "C:\Qt\5.14.2\msvc2017_64\bin"
;#define Redist_DIR "C:\Program Files (x86)\Microsoft Visual Studio\2019\Community\VC\Redist\MSVC\v142"
#define Redist_DIR "C:\Program Files (x86)\Microsoft Visual Studio\2017\Community\VC\Redist\MSVC\14.16.27012"
#define LIB_DIR "..\..\libs"
#define Build_DIR  "..\..\build"
#define ApplicationVersion GetFileVersion('..\..\build\win64\release\AVM-Service.exe')



;[CustomMessages]
;Version={#Version}

[Setup]
; NOTE: The value of AppId uniquely identifies this application. Do not use the same AppId value in installers for other applications.
; (To generate a new GUID, click Tools | Generate GUID inside the IDE.)
AppId={{4C4A32F2-8733-4C05-AF66-3996C08A228A}
AppName={#GroupName}
AppVerName={#ExeName} {#ApplicationVersion}
;AppVersion={#AppVersion}
;VersionInfoVersion={#AppVersion}
;AppVersion= {#GetStringFileInfo("{#Build_DIR}\{#ARCH}\release\{#ExeName}", "FileVersion")}
AppPublisher={#Publisher}
AppPublisherURL={#URL}
AppSupportURL={#URL}
AppUpdatesURL={#URL}
;DefaultDirName={autopf}\{#GroupName}
;DisableProgramGroupPage=yes
DefaultDirName={commonpf}\{#EngName}
DefaultGroupName={#EngName}
; The [Icons] "quicklaunchicon" entry uses {userappdata} but its [Tasks] entry has a proper IsAdminInstallMode Check.
UsedUserAreasWarning=no
; Uncomment the following line to run in non administrative install mode (install for current user only.)
;PrivilegesRequired=lowest
SetupIconFile=..\coma.ico
Compression=lzma
SolidCompression=yes
WizardStyle=modern
OutputBaseFilename={#EngName}-{#ApplicationVersion}-x64
OutputDir=..\output
VersionInfoVersion={#ApplicationVersion}

[Dirs]
Name: {userappdata}\{#EngName}


[Languages]
Name: "russian"; MessagesFile: "compiler:Languages\Russian.isl"

[Tasks]
Name: "desktopicon"; Description: "{cm:CreateDesktopIcon}"; GroupDescription: "{cm:AdditionalIcons}"; Flags: unchecked
Name: "quicklaunchicon"; Description: "{cm:CreateQuickLaunchIcon}"; GroupDescription: "{cm:AdditionalIcons}"; Flags: unchecked; OnlyBelowVersion: 6.1; Check: not IsAdminInstallMode

[Files]
Source: "{#Build_DIR}\{#ARCH}\release\{#ExeName}"; DestDir: "{app}"; Flags: ignoreversion
Source: "{#QT_DIR}\..\plugins\platforms\qwindows.dll"; DestDir: "{app}\platforms"; 
Source: "{#QT_DIR}\..\plugins\styles\qwindowsvistastyle.dll"; DestDir: "{app}\styles"; 
Source: "{#Build_DIR}\{#ARCH}\release\errors\*"; DestDir: "{userappdata}\{#EngName}"; Flags: ignoreversion recursesubdirs createallsubdirs
;Source: "{#Build_DIR}\{#ARCH}\release\reports\*"; DestDir: "{userappdata}\{#EngName}"; Flags: ignoreversion recursesubdirs createallsubdirs
Source: "{#Build_DIR}\{#ARCH}\release\images\*"; DestDir: "{app}\images"; Flags: ignoreversion recursesubdirs createallsubdirs
Source: "{#QT_DIR}\Qt5Core.dll"; DestDir: "{app}"; 
Source: "{#QT_DIR}\Qt5Gui.dll"; DestDir: "{app}"; 
Source: "{#QT_DIR}\Qt5Network.dll"; DestDir: "{app}"; 
Source: "{#QT_DIR}\Qt5SerialPort.dll"; DestDir: "{app}";
Source: "{#QT_DIR}\Qt5Svg.dll"; DestDir: "{app}"; 
Source: "{#QT_DIR}\Qt5Widgets.dll"; DestDir: "{app}"; 
Source: "{#LIB_DIR}\{#ARCH}\release\*.dll"; DestDir: "{app}"; 
Source: "{#Redist_DIR}\vc_redist.x64.exe"; DestDir: "{tmp}"; Flags: deleteafterinstall
; NOTE: Don't use "Flags: ignoreversion" on any shared system files


[Icons]
Name: "{group}\{#Name}"; Filename: "{app}\{#ExeName}"
;Name: "{group}\����������� ������������ ����"; Filename: "{app}\���� ����������� ������������.pdf"
Name: "{group}\������� ��������� {#Name}"; Filename: "{uninstallexe}"



[InstallDelete]
Type: files; Name: "{app}\*.dll"
Type: filesandordirs; Name: "{app}\platforms"
Type: files; Name: "{app}\{#ExeName}"
; Type: files; Name: "{app}\���� ����������� ������������.pdf"


[Run]
Filename: "{tmp}\vc_redist.x64.exe"; Parameters: "/install /quiet /norestart"; StatusMsg: ��������������� ����� MSVC2017 Redistributable...