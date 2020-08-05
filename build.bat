cd %~dp0

xcopy xz-utilz\src\liblzma\api\lzma ..\includes\lzma /I /Y /S
xcopy xz-utilz\src\liblzma\api\lzma.h ..\includes\lzma\lzma.h* /Y
xcopy hidapi\hidapi\hidapi.h ..\includes\hidapi\hidapi.h* /Y
xcopy QtXlsxWriter\src ..\src /I /Y /S
xcopy QtXlsxWriter\include ..\include\QtXlsx /I /Y /S
start /wait script\buildX86.bat
start /wait script\buildX64.bat

start /wait "" "C:\Program Files (x86)\Inno Setup 5\ISCC.exe" iss\avm-service-x64.iss
start /wait "" "C:\Program Files (x86)\Inno Setup 5\ISCC.exe" iss\avm-service.iss





