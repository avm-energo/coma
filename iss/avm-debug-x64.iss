; Script generated by the Inno Setup Script Wizard.
; SEE THE DOCUMENTATION FOR DETAILS ON CREATING INNO SETUP SCRIPT FILES!
#define Name "���-�������"
#define GroupName "���-�������"
#define EngName "AVM-Debug"
#define Publisher "EvelSoft"
#define URL "http://www.avmenergo.ru"
#define ExeName "AVM-Debug.exe"
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
#define ARCH "win64"
#define BUILD_TYPE "Release"
#define QT_DIR "C:\Qt\5.15.2\msvc2019_64\bin"
#define Redist_DIR "C:\Program Files (x86)\Microsoft Visual Studio\2019\Community\VC\Redist\MSVC\v142"
#define OUTPUT_DIR  "..\output"
#define ApplicationVersion GetFileVersion('..\output\win64\Release\AVM-Debug\AVM-Debug.exe')

[Setup]
; NOTE: The value of AppId uniquely identifies this application. Do not use the same AppId value in installers for other applications.
; (To generate a new GUID, click Tools | Generate GUID inside the IDE.)
AppId={{4C4A32F2-8733-4C05-AF66-3996C08A228A}
AppName={#GroupName}
AppVerName={#EngName} {#ApplicationVersion}
AppPublisher={#Publisher}
AppPublisherURL={#URL}
AppSupportURL={#URL}
AppUpdatesURL={#URL}
DefaultDirName={commonpf64}\{#EngName}
DefaultGroupName="{#EngName} (x64)"
UsedUserAreasWarning=no
SetupIconFile=..\coma.ico
Compression=lzma
ChangesAssociations=yes
SolidCompression=yes
WizardStyle=modern
OutputBaseFilename={#EngName}-{#ApplicationVersion}-x64
OutputDir=..\output
VersionInfoVersion={#ApplicationVersion}
LicenseFile="..\license.txt"
InfoAfterFile="..\AVM-Debug.NOTES"

[Registry]
Root: HKCR; Subkey: ".swj";                           ValueData: "{#EngName}";          Flags: uninsdeletevalue;    ValueType: string;  ValueName: ""
Root: HKCR; Subkey: "{#EngName}";                     ValueData: "Program {#EngName}";  Flags: uninsdeletekey;      ValueType: string;  ValueName: ""
Root: HKCR; Subkey: "{#EngName}\DefaultIcon";         ValueData: "{app}\{#ExeName},0";                              ValueType: string;  ValueName: ""
Root: HKCR; Subkey: "{#EngName}\shell\open\command";  ValueData: """{app}\{#ExeName}"" ""%1""";                     ValueType: string;  ValueName: ""

[Dirs]
Name: {userappdata}\{#EngName}

[Languages]
Name: "russian"; MessagesFile: "compiler:Languages\Russian.isl"

[Tasks]
Name: "desktopicon";     Description: "{cm:CreateDesktopIcon}";     GroupDescription: "{cm:AdditionalIcons}"; Flags: unchecked
Name: "quicklaunchicon"; Description: "{cm:CreateQuickLaunchIcon}"; GroupDescription: "{cm:AdditionalIcons}"; Flags: unchecked; OnlyBelowVersion: 6.1; Check: not IsAdminInstallMode

[Files]
Source: "{#OUTPUT_DIR}\{#ARCH}\{#BUILD_TYPE}\{#EngName}\*"; DestDir: "{app}"; Excludes: "*.xml"; Flags: ignoreversion recursesubdirs 
Source: "{#Redist_DIR}\vc_redist.x64.exe"; DestDir: "{tmp}"; Flags: deleteafterinstall
Source: "{#OUTPUT_DIR}\{#ARCH}\{#BUILD_TYPE}\{#EngName}\settings\*.xml"; DestDir: "{localappdata}\{#EngName}"

[Icons]
Name: "{group}\{#Name}"; Filename: "{app}\{#ExeName}"
Name: "{group}\������� ��������� {#Name}"; Filename: "{uninstallexe}"
;Name: "{group}\����������� ������������ ����"; Filename: "{app}\���� ����������� ������������.pdf"

[Run]
Filename: "{tmp}\vc_redist.x64.exe"; Parameters: "/install /quiet /norestart"; StatusMsg: ��������������� ����� MSVC2019 Redistributable...

[UninstallDelete]
Type: filesandordirs; Name: "{localappdata}\{#EngName}"
