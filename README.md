| OS | Status | Download |
|----------|--------|----------|
| Windows x64 | [![](http://172.16.31.73:27015/app/rest/builds/buildType:AVMService_CiAVMServiceWindowsBuild_BuildX64/statusIcon.svg)](http://172.16.31.73:27015/buildConfiguration/AVMService_CiAVMServiceWindowsBuild_BuildX64) | [Download](file://///Fserver/av-tuk/SOFT/AVTUK-S/CI/) |
| Windows x86 | [![](http://172.16.31.73:27015/app/rest/builds/buildType:AVMService_CiAVMServiceWindowsBuild_BuildX86/statusIcon.svg)](http://172.16.31.73:27015/buildConfiguration/AVMService_CiAVMServiceWindowsBuild_BuildX86) | [Download](file://///Fserver/av-tuk/SOFT/AVTUK-S/CI/) |



Requires:  

* conan
  * boost/dynamic_bitset
* LimeReport - submodule
* hidapi - submodule
* ctti - inside project tree because project was abandoned
* qcustomplot - same as ctti

How to initialize conan, just run command in repository root folder

```
$ conan install . -g qmake
```

