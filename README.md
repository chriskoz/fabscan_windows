# Overview

This project is my fork to the fabscan windows port by zhangdong1981. (https://github.com/zhangdong1981/fabscan_windows)

I've merged the more recent FabScan100 code base with this windows port and fixed a few issues with garbage getting sent to the micro controller.

The intension is to complete/fix the logic for using multiple lasers and to add features like sweeping lasers and multiple camera.

# pre-requisites

 * Visual Studio or Express 2010 for C++
 * Qt5 Win32 with opengl for MSVC2010
 * 32-bit versions of PCL
 * OpenCV w/ all the 3rd party stuff

# compiling

 * edit weibao/weibao_3d/weibao_3d.vcxproj.user and fix &lt;QTDIR> entries to point to where you have installed Qt5
 * open weibao/weibao_3d.sln in MSVC2010
 * select Project->Properties.  Click on Configuration Properties.  In VC++ Directories, Include Directories->edit and make sure there are "include" entries for OpenCV; PCL; PCL/3rdParty/Boost, /Eigen and /FLANN; and Qt5 /include/QtMultiMedia and /MultiMediaWidgets.
 * In Library Directories ensure "lib" entries for OpenCV; PCL; PCL/3rdParty/Boost, /Eigen, /FLANN and /VTK; and Qt5.
 * In Linker->Input->Additional Dependencies ensure (no paths required) qtmain.lib, Qt5Core.lib, Qt5Gui.lib, Qt5OpenGL.lib, opengl32.lib, glu32.lib, Qt5Widgets.lib, opencv_core245.lib. opencv_highgui245.lib, opencv_imgproc245.lib, pcl_common_release.lib, pcl_features_release.lib, pcl_io_ply.release.lib, pcl_kdtree_release, pcl_octree_release.lib, pcl_search_release.lib, pcl_surface_release.lib

# post-compile

Copy the following files to the weibao_3d/win32/Release directory:

From Qt5/5.1.0/mscv2010_opengl/bin...
 * icudt51.dll
 * icuin51.dll
 * icuuc51.dll
 * Qt5Core.dll
 * Qt5Gui.dll
 * Qt5Multimedia.dll
 * Qt5MultimediaWidgets.dll
 * Qt5Network.dll
 * Qt5OpenGL.dll
 * Qt5Widgets.dll

From OpenCV/build/x86/vc10/bin...
 * opencv_core245.dll
 * opencv_highgui245.dll
 * opencv_imgproc245.dll

From PCL1.6.0/bin... 
 * pcl_common_release.dll
 * pcl_features_release.dll
 * pcl_io_ply.release.dll
 * pcl_kdtree_release.dll
 * pcl_octree_release.dll
 * pcl_search_release.dll
 * pcl_surface_release.dll

Also OpenNI.dll is required which seems to be a driver installed by the PCL 3rd party software OpenNI

These are the .dll versions of the files pointed-to in the Linker->input->Additional Dependencies list plus icu*.dlls from Qt5

TODO: document the Qt5 serial lib compile as well.