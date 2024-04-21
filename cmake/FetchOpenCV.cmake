include(FetchContent)

set(OpenCV_Version 4.9.0-4)

if(${CMAKE_BUILD_TYPE} STREQUAL Release OR ${CMAKE_BUILD_TYPE} STREQUAL RelWithDebInfo)
  set(OpenCV_BUILD_TYPE Release)
else()
  set(OpenCV_BUILD_TYPE Debug)
endif()

set(OpenCV_BASEURL "https://github.com/kaito-tokyo/kaito-tokyo-obs-dep-opencv/releases/download/${OpenCV_Version}")

if(APPLE)
  if(OpenCV_BUILD_TYPE STREQUAL Debug)
    set(OpenCV_URL "${OpenCV_BASEURL}/opencv-macos-${OpenCV_Version}-Debug.tar.gz")
    set(OpenCV_HASH SHA256=a5fe97692232088c112b32f64ddb97f85bc5b2435f7b9b1012efd4ced1f02ca3)
  else()
    set(OpenCV_URL "${OpenCV_BASEURL}/opencv-macos-${OpenCV_Version}-Release.tar.gz")
    set(OpenCV_HASH SHA256=3f01c2248ebb3fd7d25e17de7e56672ed45a0c140b61a25da88f8716abed0367)
  endif()
elseif(MSVC)
  if(OpenCV_BUILD_TYPE STREQUAL Debug)
    set(OpenCV_URL "${OpenCV_BASEURL}/opencv-windows-${OpenCV_Version}-Debug.zip")
    set(OpenCV_HASH SHA256=e5d1126856df983277c229c604b2a3afae0c5c27f94fad1c94bf076d77b44ea6)
  else()
    set(OpenCV_URL "${OpenCV_BASEURL}/opencv-windows-${OpenCV_Version}-Release.zip")
    set(OpenCV_HASH SHA256=8f38a041bdfc0ae7164075ccf12394e543cec6f28cf4a105e4324d0a2a630346)
  endif()
else()
  if(OpenCV_BUILD_TYPE STREQUAL Debug)
    set(OpenCV_URL "${OpenCV_BASEURL}/opencv-linux-${OpenCV_Version}-Debug.tar.gz")
    set(OpenCV_HASH SHA256=dc8ed538dd818ba1240158b307a79f7337a89b4971f5ff05561c99bb064e1543)
  else()
    set(OpenCV_URL "${OpenCV_BASEURL}/opencv-linux-${OpenCV_Version}-Release.tar.gz")
    set(OpenCV_HASH SHA256=8f2ece673f38af3b0aab223ec00b2c5320e00dd9ad36b779397f5305aeb51060)
  endif()
endif()

FetchContent_Declare(
  OpenCV
  URL ${OpenCV_URL}
  URL_HASH ${OpenCV_HASH})
FetchContent_MakeAvailable(opencv)

add_library(OpenCV INTERFACE)
if(MSVC)
  target_link_libraries(
    OpenCV
    INTERFACE ${opencv_SOURCE_DIR}/x64/vc17/staticlib/opencv_features2d490.lib
              ${opencv_SOURCE_DIR}/x64/vc17/staticlib/opencv_imgcodecs490.lib
              ${opencv_SOURCE_DIR}/x64/vc17/staticlib/opencv_imgproc490.lib
              ${opencv_SOURCE_DIR}/x64/vc17/staticlib/opencv_core490.lib
              ${opencv_SOURCE_DIR}/x64/vc17/staticlib/libpng.lib
              ${opencv_SOURCE_DIR}/x64/vc17/staticlib/zlib.lib)
  target_include_directories(OpenCV SYSTEM INTERFACE ${opencv_SOURCE_DIR}/include)
else()
  target_link_libraries(
    OpenCV
    INTERFACE ${opencv_SOURCE_DIR}/lib/libopencv_features2d.a ${opencv_SOURCE_DIR}/lib/libopencv_imgcodecs.a
              ${opencv_SOURCE_DIR}/lib/libopencv_imgproc.a ${opencv_SOURCE_DIR}/lib/libopencv_core.a
              ${opencv_SOURCE_DIR}/lib/opencv4/3rdparty/liblibpng.a ${opencv_SOURCE_DIR}/lib/opencv4/3rdparty/libzlib.a)
  target_include_directories(OpenCV SYSTEM INTERFACE ${opencv_SOURCE_DIR}/include/opencv4)
endif()
