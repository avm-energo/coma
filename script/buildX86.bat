call "C:\Program Files (x86)\Microsoft Visual Studio\2017\Community\VC\Auxiliary\Build\vcvarsall.bat" x86
call "C:\Program Files (x86)\Microsoft Visual Studio\2017\Community\Common7\Tools\vsdevcmd\ext\vcvars.bat"
start /wait MSBuild "%cd%\xz-utilz\windows\vs2017\liblzma_dll.vcxproj" /property:Configuration=Release;Platform=win32;OutDir=..\..\..\..\libs\win32\release
start /wait MSBuild "%cd%\hidapi\windows\hidapi.vcxproj" /property:Configuration=Release;Platform=win32;OutDir=..\..\..\libs\win32\release
cd LimeReport && start /wait "" nmake clean
start /wait "" "C:\Qt\5.14.2\msvc2017\bin\qmake.exe" -recursive
start /wait "" nmake sub-3rdparty sub-limereport 
cd ..
xcopy LimeReport\build\5.14.2\win32\release\lib\*.* ..\libs\win32\release /H /Y /C /R /S /I
xcopy LimeReport\build\5.14.2\win32\release\lib\include ..\includes\limereport /I /Y /S
cd QtXlsxWriter && start /wait "" nmake clean
start /wait "" "C:\Qt\5.14.2\msvc2017\bin\qmake.exe" -recursive
start /wait "" nmake sub-src
xcopy lib\Qt5Xlsx.*  ..\..\libs\win32\release /H /Y /C /R /S /I
cd ..
start /wait "" nmake clean
start /wait "" "C:\Qt\5.14.2\msvc2017_64\bin\qmake.exe" AVTUK_ETH_Version-f
start /wait nmake
start /wait "" "C:\Program Files (x86)\Inno Setup 6\ISCC.exe" iss\avm-service.iss