; -- PerDe.iss --

; SEE THE DOCUMENTATION FOR DETAILS ON CREATING .ISS SCRIPT FILES!

[CustomMessages]
Version=2.0.0a

[Languages]
Name: Russian; MessagesFile: "compiler:Languages\Russian.isl" 

[Setup]
UsePreviousLanguage=No
AppName="COMA"
AppVersion="2.0.0a"
DefaultDirName={pf}\Coma
DefaultGroupName=КОМА
; UninstallDisplayIcon={app}\MyProg.exe
Compression=lzma2
SolidCompression=yes
OutputDir="out\"
LicenseFile="coma-xp\license.txt"
InfoBeforeFile="coma-xp\coma.txt"
OutputBaseFilename="coma-2.0.0a-xp"

[Files]
Source: "coma-xp\*.dll"; DestDir: "{app}"; Flags: onlyifdoesntexist
Source: "coma-xp\plugins\sqldrivers\qsqlmysql.dll"; DestDir: "{app}\plugins\sqldrivers"; Flags: onlyifdoesntexist
Source: "coma-xp\platforms\qwindows.dll"; DestDir: "{app}\platforms"; Flags: onlyifdoesntexist
Source: "coma-xp\COMA.exe"; DestDir: "{app}"; Flags: replacesameversion
Source: "coma-xp\ermsgs.dat"; DestDir: "{app}\errors"
; Source: "iss\PerDe x86\ОтКад РП.pdf"; DestDir: "{app}"

[Icons]
Name: "{group}\КОМА"; Filename: "{app}\COMA.exe"
; Name: "{group}\Руководство пользователя"; Filename: "{app}\ОтКад РП.pdf"
Name: "{group}\Удалить приложение"; Filename: "{uninstallexe}"