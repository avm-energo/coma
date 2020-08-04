xcopy xz-utilz\src\liblzma\api\lzma ..\includes\lzma
xcopy xz-utilz\src\liblzma\api\lzma.h ..\includes\lzma\lzma.h
xcopy hidapi\hidapi\hidapi.h ..\includes\hidapi\hidapi.h
xcopy QtXlsxWriter\src ..\src
call script\buildX86.bat
call script\buildX64.bat

start /wait "C:\Program Files (x86)\Inno Setup 6\ISCC.exe" ..\iss\avm-service-x64.iss
start /wait "C:\Program Files (x86)\Inno Setup 6\ISCC.exe" ..\iss\avm-service.iss





