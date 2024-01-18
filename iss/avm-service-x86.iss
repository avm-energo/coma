; Script generated by the Inno Setup Script Wizard.
; SEE THE DOCUMENTATION FOR DETAILS ON CREATING INNO SETUP SCRIPT FILES!
#define Name "���-������"
#define GroupName "���-������"
#define EngName "AVM-Service"
#define Publisher "AVM-Energo"
#define URL "http://www.avmenergo.ru"
#define ExeName "AVM-Service.exe"
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
#define ARCH "win32"
#define BUILD_TYPE "Release"
#define QT_DIR "C:\Qt\5.15.2\msvc2019\bin"
#define Redist_DIR "C:\Program Files (x86)\Microsoft Visual Studio\2019\Community\VC\Redist\MSVC\v142"
#define OUTPUT_DIR  "..\output"
#define ApplicationVersion GetFileVersion('..\output\win32\Release\AVM-Service\AVM-Service.exe')

[Setup]
; NOTE: The value of AppId uniquely identifies this application. Do not use the same AppId value in installers for other applications.
; (To generate a new GUID, click Tools | Generate GUID inside the IDE.)
AppId={{C7440414-7CEB-42F6-A3CD-9D4223BC7713}
AppName={#GroupName}
AppVerName={#EngName} {#ApplicationVersion}
AppPublisher={#Publisher}
AppPublisherURL={#URL}
AppSupportURL={#URL}
AppUpdatesURL={#URL}
DefaultDirName={commonpf32}\{#EngName}
DefaultGroupName="{#EngName} (x86)"
UsedUserAreasWarning=no
SetupIconFile=..\coma.ico
Compression=lzma
SolidCompression=yes
WizardStyle=modern
OutputBaseFilename={#EngName}-{#ApplicationVersion}-x86
OutputDir=..\output
VersionInfoVersion={#ApplicationVersion}
LicenseFile="..\license.txt"
InfoAfterFile="..\AVM-Service.NOTES"

[Dirs]
Name: {userappdata}\{#EngName}

[Languages]
Name: "russian"; MessagesFile: "compiler:Languages\Russian.isl"

[Tasks]
Name: "desktopicon";     Description: "{cm:CreateDesktopIcon}";     GroupDescription: "{cm:AdditionalIcons}"; Flags: unchecked
Name: "quicklaunchicon"; Description: "{cm:CreateQuickLaunchIcon}"; GroupDescription: "{cm:AdditionalIcons}"; Flags: unchecked; OnlyBelowVersion: 6.1; Check: not IsAdminInstallMode

[Files]
Source: "{#OUTPUT_DIR}\{#ARCH}\{#BUILD_TYPE}\{#EngName}\*"; DestDir: "{app}"; Excludes: "*.xml"; Flags: ignoreversion recursesubdirs
Source: "{#Redist_DIR}\vc_redist.x86.exe"; DestDir: "{tmp}"; Flags: deleteafterinstall
Source: "{#OUTPUT_DIR}\{#ARCH}\{#BUILD_TYPE}\{#EngName}\settings\*.xml"; DestDir: "{localappdata}\{#EngName}"
Source: "{#OUTPUT_DIR}\{#ARCH}\{#BUILD_TYPE}\{#EngName}\reports\*.lrxml"; DestDir: "{localappdata}\{#EngName}"

[Icons]
Name: "{group}\{#Name}"; Filename: "{app}\{#ExeName}"
Name: "{group}\������� ��������� {#Name}"; Filename: "{uninstallexe}"
;Name: "{group}\����������� ������������ ����"; Filename: "{app}\���� ����������� ������������.pdf"

[Run]
Filename: "{tmp}\vc_redist.x86.exe"; Parameters: "/install /quiet /norestart"; StatusMsg: ��������������� ����� MSVC2019 Redistributable...

[UninstallDelete]
Type: filesandordirs; Name: "{localappdata}\{#EngName}"
