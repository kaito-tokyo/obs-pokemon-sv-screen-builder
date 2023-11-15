include(FetchContent)

set(OpenCV_Version 4.8.1-5)

if(${CMAKE_BUILD_TYPE} STREQUAL Release OR ${CMAKE_BUILD_TYPE} STREQUAL RelWithDebInfo)
  set(OpenCV_BUILD_TYPE Release)
else()
  set(OpenCV_BUILD_TYPE Debug)
endif()

set(OpenCV_BASEURL
    "https://github.com/umireon/obs-pokemon-sv-screen-builder-dep-opencv/releases/download/${OpenCV_Version}")

if(APPLE)
  if(OpenCV_BUILD_TYPE STREQUAL Release)
    set(OpenCV_URL "${OpenCV_BASEURL}/opencv-macos-${OpenCV_Version}-Release.tar.gz")
    set(OpenCV_HASH SHA256=1e9aba2006bc0911ae83def4e7af39f3e217ba22ad7ee838e399d19659b3cda1)
  else()
    set(OpenCV_URL "${OpenCV_BASEURL}/opencv-macos-${OpenCV_Version}-Debug.tar.gz")
    set(OpenCV_HASH SHA256=ead66af2aa5a2042a5315217a3f8e2879d641f3d83035d6f172aa59b183d0c36)
  endif()
elseif(MSVC)
  if(OpenCV_BUILD_TYPE STREQUAL Release)
    set(OpenCV_URL "${OpenCV_BASEURL}/opencv-windows-${OpenCV_Version}-Release.zip")
    set(OpenCV_HASH SHA256=b31ab7f606a9bcc201ab3cc80f3257b45fe25f2efb1dee7ec36a41fcb7db0979)
  else()
    set(OpenCV_URL "${OpenCV_BASEURL}/opencv-windows-${OpenCV_Version}-Debug.zip")
    set(OpenCV_HASH SHA256=d61b5c03cf4ed2b5f615fa0f275c6101457b3c1273d838fbf898ec0a2aeceb02)
  endif()
else()
  if(OpenCV_BUILD_TYPE STREQUAL Release)
    set(OpenCV_URL "${OpenCV_BASEURL}/opencv-linux-${OpenCV_Version}-Release.tar.gz")
    set(OpenCV_HASH SHA256=7ecca557b1d2d26e734f3c629cdd01d06be3eec3d494dfb719aa3ad6461e1ba6)
  else()
    set(OpenCV_URL "${OpenCV_BASEURL}/opencv-linux-${OpenCV_Version}-Debug.tar.gz")
    set(OpenCV_HASH SHA256=708daf04ead3234f3cdfd71247b9fdc1b82754e72d9e86ed2f623c74cae83e37)
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
    INTERFACE ${opencv_SOURCE_DIR}/x64/vc17/staticlib/opencv_features2d480.lib
              ${opencv_SOURCE_DIR}/x64/vc17/staticlib/opencv_imgcodecs480.lib
              ${opencv_SOURCE_DIR}/x64/vc17/staticlib/opencv_imgproc480.lib
              ${opencv_SOURCE_DIR}/x64/vc17/staticlib/opencv_core480.lib
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
