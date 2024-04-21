include(FetchContent)

set(USE_SYSTEM_OPENCV
    OFF
    CACHE STRING "Use system OpenCV")

set(OpenCV_Version 4.9.0-4)

if(${CMAKE_BUILD_TYPE} STREQUAL Debug)
  set(OpenCV_BUILD_TYPE Debug)
else()
  set(OpenCV_BUILD_TYPE Release)
endif()

set(OpenCV_BASEURL "https://github.com/kaito-tokyo/kaito-tokyo-obs-dep-opencv/releases/download/${OpenCV_Version}")

if(OS_LINUX)
  if(OpenCV_BUILD_TYPE STREQUAL Debug)
    set(OpenCV_URL "${OpenCV_BASEURL}/opencv-linux-${OpenCV_Version}-Debug.tar.gz")
    set(OpenCV_HASH SHA256=dc8ed538dd818ba1240158b307a79f7337a89b4971f5ff05561c99bb064e1543)
  else()
    set(OpenCV_URL "${OpenCV_BASEURL}/opencv-linux-${OpenCV_Version}-Release.tar.gz")
    set(OpenCV_HASH SHA256=8f2ece673f38af3b0aab223ec00b2c5320e00dd9ad36b779397f5305aeb51060)
  endif()
elseif(OS_MACOS)
  if(OpenCV_BUILD_TYPE STREQUAL Debug)
    set(OpenCV_URL "${OpenCV_BASEURL}/opencv-macos-${OpenCV_Version}-Debug.tar.gz")
    set(OpenCV_HASH SHA256=a5fe97692232088c112b32f64ddb97f85bc5b2435f7b9b1012efd4ced1f02ca3)
  else()
    set(OpenCV_URL "${OpenCV_BASEURL}/opencv-macos-${OpenCV_Version}-Release.tar.gz")
    set(OpenCV_HASH SHA256=3f01c2248ebb3fd7d25e17de7e56672ed45a0c140b61a25da88f8716abed0367)
  endif()
elseif(OS_WINDOWS)
  if(OpenCV_BUILD_TYPE STREQUAL Debug)
    set(OpenCV_URL "${OpenCV_BASEURL}/opencv-windows-${OpenCV_Version}-Debug.zip")
    set(OpenCV_HASH SHA256=e5d1126856df983277c229c604b2a3afae0c5c27f94fad1c94bf076d77b44ea6)
  else()
    set(OpenCV_URL "${OpenCV_BASEURL}/opencv-windows-${OpenCV_Version}-Release.zip")
    set(OpenCV_HASH SHA256=8f38a041bdfc0ae7164075ccf12394e543cec6f28cf4a105e4324d0a2a630346)
  endif()
endif()

if(NOT USE_SYSTEM_OPENCV)
  FetchContent_Declare(
    opencv
    URL ${OpenCV_URL}
    URL_HASH ${OpenCV_HASH})
  FetchContent_MakeAvailable(opencv)
endif()

set(OpenCV_STATIC ON)
find_package(OpenCV REQUIRED COMPONENTS core imgproc imgcodecs features2d HINTS "${opencv_SOURCE_DIR}")
