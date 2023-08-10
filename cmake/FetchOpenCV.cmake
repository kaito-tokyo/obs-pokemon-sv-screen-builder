include(FetchContent)

set(OpenCV_Version 4.8.0-5)

if(${CMAKE_BUILD_TYPE} STREQUAL Release OR ${CMAKE_BUILD_TYPE} STREQUAL RelWithDebInfo)
  set(OpenCV_BUILD_TYPE Release)
else()
  set(OpenCV_BUILD_TYPE Debug)
endif()

if(APPLE)
  if(OpenCV_BUILD_TYPE STREQUAL Release)
    FetchContent_Declare(
      opencv
      URL "https://github.com/umireon/obs-pokemon-sv-screen-builder-dep-opencv/releases/download/${OpenCV_Version}/opencv-macos-Release.tar.gz"
      URL_HASH MD5=339615d443085584b73f28853554380d)
  else()
    FetchContent_Declare(
      opencv
      URL "https://github.com/umireon/obs-pokemon-sv-screen-builder-dep-opencv/releases/download/${OpenCV_Version}/opencv-macos-Debug.tar.gz"
      URL_HASH MD5=b1e2405d2fdaa972efeabc78d5e9d0d2)
  endif()

  add_library(OpenCV INTERFACE)
  FetchContent_MakeAvailable(opencv)
  target_link_libraries(
    OpenCV
    INTERFACE ${opencv_SOURCE_DIR}/lib/libopencv_features2d.a ${opencv_SOURCE_DIR}/lib/libopencv_imgcodecs.a
              ${opencv_SOURCE_DIR}/lib/libopencv_imgproc.a ${opencv_SOURCE_DIR}/lib/libopencv_core.a
              ${opencv_SOURCE_DIR}/lib/opencv4/3rdparty/liblibpng.a ${opencv_SOURCE_DIR}/lib/opencv4/3rdparty/libzlib.a)
  target_include_directories(OpenCV INTERFACE ${opencv_SOURCE_DIR}/include/opencv4)
elseif(MSVC)

  if(OpenCV_BUILD_TYPE STREQUAL Release)
    FetchContent_Declare(
      OpenCV
      URL "https://github.com/umireon/obs-pokemon-sv-screen-builder-dep-opencv/releases/download/${OpenCV_Version}/opencv-windows-Release.tar.gz"
      URL_HASH MD5=af1e75900b70a50baef24cbade6d3342)
  else()
    FetchContent_Declare(
      OpenCV
      URL "https://github.com/umireon/obs-pokemon-sv-screen-builder-dep-opencv/releases/download/${OpenCV_Version}/opencv-windows-Debug.tar.gz"
      URL_HASH MD5=461287f75c0703af8ad7154d17e9dea6)
  endif()

  add_library(OpenCV INTERFACE)
  FetchContent_MakeAvailable(opencv)
  target_link_libraries(
    OpenCV
    INTERFACE ${opencv_SOURCE_DIR}/x64/vc17/staticlib/opencv_features2d480.lib
              ${opencv_SOURCE_DIR}/x64/vc17/staticlib/opencv_imgcodecs.lib
              ${opencv_SOURCE_DIR}/x64/vc17/staticlib/opencv_imgproc.lib
              ${opencv_SOURCE_DIR}/x64/vc17/staticlib/opencv_core.lib
              ${opencv_SOURCE_DIR}/x64/vc17/staticlib/libpng.lib
              ${opencv_SOURCE_DIR}/x64/vc17/staticlib/zlib.lib)
  target_include_directories(OpenCV INTERFACE ${opencv_SOURCE_DIR}/include)
else()
  if(OpenCV_BUILD_TYPE STREQUAL Release)
    FetchContent_Declare(
      OpenCV
      URL "https://github.com/umireon/obs-pokemon-sv-screen-builder-dep-opencv/releases/download/${OpenCV_Version}/opencv-linux-Release.tar.gz"
      URL_HASH MD5=aa2fa6338d3c22bad64494f834d107b9)
  else()
    FetchContent_Declare(
      OpenCV
      URL "https://github.com/umireon/obs-pokemon-sv-screen-builder-dep-opencv/releases/download/${OpenCV_Version}/opencv-linux-Debug.tar.gz"
      URL_HASH MD5=2da52ad39dd6270b530f7db3c1577aaa)
  endif()

  add_library(OpenCV INTERFACE)
  FetchContent_MakeAvailable(opencv)
  target_link_libraries(
    OpenCV
    INTERFACE ${opencv_SOURCE_DIR}/lib/libopencv_features2d.a ${opencv_SOURCE_DIR}/lib/libopencv_imgcodecs.a
              ${opencv_SOURCE_DIR}/lib/libopencv_imgproc.a ${opencv_SOURCE_DIR}/lib/libopencv_core.a
              ${opencv_SOURCE_DIR}/lib/opencv4/3rdparty/liblibpng.a ${opencv_SOURCE_DIR}/lib/opencv4/3rdparty/libzlib.a)
  target_include_directories(OpenCV INTERFACE ${opencv_SOURCE_DIR}/include/opencv4)
endif()
