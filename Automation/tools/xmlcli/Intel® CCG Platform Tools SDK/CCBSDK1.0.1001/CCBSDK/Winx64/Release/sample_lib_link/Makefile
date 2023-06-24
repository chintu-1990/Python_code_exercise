all:  TestHWAPI TestTopology

TestHWAPI:
	cl  /c  /D PACKAGE_SPECS=\"PackageSpecsExt.h\"  /D "WIN32" /D _WIN32_ /D "_LIB" /D "WINVER=0x5000" /D "_WIN32_WINNT" /D _AMD64_  /I ..\..\..\Inc ../../../Src/TestHWAPI.c
	cl  /c  /D PACKAGE_SPECS=\"PackageSpecsExt.h\"  /D "WIN32" /D _WIN32_ /D "_LIB" /D "WINVER=0x5000" /D "_WIN32_WINNT" /D _AMD64_  /I ..\..\..\Inc ../../../Src/OptionHandlers.c
	cl  /c  /D PACKAGE_SPECS=\"PackageSpecsExt.h\"  /D "WIN32" /D _WIN32_ /D "_LIB" /D "WINVER=0x5000" /D "_WIN32_WINNT" /D _AMD64_  /I ..\..\..\Inc ../../../Src/OptionUtilities.c

	LINK   TestHWAPI.obj OptionHandlers.obj OptionUtilities.obj /LIBPATH:..\..\..\lib\Winx64\Release  OSInterface.lib ContainerLib.lib Debug.lib HWAPILib.lib  advapi32.lib 

TestTopology:
	cl  /c  /D PACKAGE_SPECS=\"PackageSpecsExt.h\"  /D "WIN32" /D _WIN32_ /D "_LIB" /D "WINVER=0x5000" /D "_WIN32_WINNT" /D _AMD64_  /I ..\..\..\Inc ../../../Src/TestTopology.c

	LINK   TestTopology.obj /LIBPATH:..\..\..\lib\Winx64\Release  OSInterface.lib ContainerLib.lib Debug.lib HWAPILib.lib Topology.lib advapi32.lib 

clean:
	del  *.obj *.exe