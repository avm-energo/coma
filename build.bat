############# LZMA #############
start /wait "C:\Program Files (x86)\Microsoft Visual Studio\2019\Community\MSBuild\Current\Bin\MSBuild.exe" xz-utilz\windows\vs2017\xz_win.sln /property:Configuration=Release;Platform=x64;OutDir=..\..\..\..\libs\win64\release >> logs\lzma64.log
start /wait "C:\Program Files (x86)\Microsoft Visual Studio\2019\Community\MSBuild\Current\Bin\MSBuild.exe" xz-utilz\windows\vs2017\xz_win.sln /property:Configuration=Release;Platform=win32;OutDir=..\..\..\..\libs\win32\release >> logs\lzma32.log
xcopy xz-utilz\src\liblzma\api\lzma ..\includes\lzma
xcopy xz-utilz\src\liblzma\api\lzma.h ..\includes\lzma\lzma.h
############# HIDAPI #############
start /wait "C:\Program Files (x86)\Microsoft Visual Studio\2017\Community\MSBuild\Current\Bin\MSBuild.exe" hidapi\windows\hidapi.vcxproj /property:Configuration=Release;Platform=x64;OutDir=..\..\..\libs\win64\release >> logs\hidapi64.log
start /wait "C:\Program Files (x86)\Microsoft Visual Studio\2017\Community\MSBuild\Current\Bin\MSBuild.exe" hidapi\windows\hidapi.vcxproj /property:Configuration=Release;Platform=win32;OutDir=..\..\..\libs\win32\release >> logs\hidapi32.log
xcopy hidapi\hidapi\hidapi.h ..\includes\hidapi\hidapi.h
############# LimeReport #############
############# x64/amd64/win64 #############
call script\limeX64.bat
############# x86/win32 #############
call script\limeX86.bat
############# QtXlsxWriter #############
############# x64/amd64/win64 #############
call script\xlsxX64.bat
############# x86/win32 #############
call script\xlsxX86.bat



set QTDIR=C:\Qt\5.14.2\msvc2017_64\
#set PATH=%QTDIR%\bin;"C:\Program Files (x86)\Microsoft Visual Studio\2019\Community\VC\Auxiliary\Build\";"C:\Program Files (x86)\Inno Setup 6"
start /wait "C:\Program Files (x86)\Inno Setup 6\ISCC.exe" ..\iss\avm-service.iss >> logs\setup32.log
start /wait "C:\Program Files (x86)\Inno Setup 6\ISCC.exe" ..\iss\avm-service-x64.iss >> logs\setup64.log




