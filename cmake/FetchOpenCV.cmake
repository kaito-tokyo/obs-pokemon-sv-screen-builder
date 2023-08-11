include(FetchContent)

set(OpenCV_Version 4.8.1-3)

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
      URL_HASH MD5=283e70722b32aae912250f04fceb2fd1)
  else()
    FetchContent_Declare(
      opencv
      URL "https://github.com/umireon/obs-pokemon-sv-screen-builder-dep-opencv/releases/download/${OpenCV_Version}/opencv-macos-Debug.tar.gz"
      URL_HASH MD5=0fe5e26e2135e40e72d91822c833d6f3)
  endif()

  add_library(OpenCV INTERFACE)
  FetchContent_MakeAvailable(opencv)
  target_link_libraries(
    OpenCV
    INTERFACE ${opencv_SOURCE_DIR}/lib/libopencv_features2d.a ${opencv_SOURCE_DIR}/lib/libopencv_imgcodecs.a
              ${opencv_SOURCE_DIR}/lib/libopencv_imgproc.a ${opencv_SOURCE_DIR}/lib/libopencv_core.a
              ${opencv_SOURCE_DIR}/lib/opencv4/3rdparty/liblibpng.a ${opencv_SOURCE_DIR}/lib/opencv4/3rdparty/libzlib.a)
  target_include_directories(OpenCV SYSTEM INTERFACE ${opencv_SOURCE_DIR}/include/opencv4)
elseif(MSVC)

  if(OpenCV_BUILD_TYPE STREQUAL Release)
    FetchContent_Declare(
      OpenCV
      URL "https://github.com/umireon/obs-pokemon-sv-screen-builder-dep-opencv/releases/download/${OpenCV_Version}/opencv-windows-Release.tar.gz"
      URL_HASH MD5=0d9293c46da215df12a7c01dec5de7c1)
  else()
    FetchContent_Declare(
      OpenCV
      URL "https://github.com/umireon/obs-pokemon-sv-screen-builder-dep-opencv/releases/download/${OpenCV_Version}/opencv-windows-Debug.tar.gz"
      URL_HASH MD5=0f7ba51d49f448fe611d91553b8f706c)
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
  target_include_directories(OpenCV SYSTEM INTERFACE ${opencv_SOURCE_DIR}/include)
else()
  if(OpenCV_BUILD_TYPE STREQUAL Release)
    FetchContent_Declare(
      OpenCV
      URL "https://github.com/umireon/obs-pokemon-sv-screen-builder-dep-opencv/releases/download/${OpenCV_Version}/opencv-linux-Release.zip"
      URL_HASH MD5=460fc72a81d793ae2c03c97731fe3206)
  else()
    FetchContent_Declare(
      OpenCV
      URL "https://github.com/umireon/obs-pokemon-sv-screen-builder-dep-opencv/releases/download/${OpenCV_Version}/opencv-linux-Debug.zip"
      URL_HASH MD5=3ae4d598e08d2f94de2c2c574c730258)
  endif()

  add_library(OpenCV INTERFACE)
  FetchContent_MakeAvailable(opencv)
  target_link_libraries(
    OpenCV
    INTERFACE ${opencv_SOURCE_DIR}/lib/libopencv_features2d.a ${opencv_SOURCE_DIR}/lib/libopencv_imgcodecs.a
              ${opencv_SOURCE_DIR}/lib/libopencv_imgproc.a ${opencv_SOURCE_DIR}/lib/libopencv_core.a
              ${opencv_SOURCE_DIR}/lib/opencv4/3rdparty/liblibpng.a ${opencv_SOURCE_DIR}/lib/opencv4/3rdparty/libzlib.a)
  target_include_directories(OpenCV SYSTEM INTERFACE ${opencv_SOURCE_DIR}/include/opencv4)
endif()
