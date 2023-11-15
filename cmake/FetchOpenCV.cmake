include(FetchContent)

set(OpenCV_Version 4.8.1-6)

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
    set(OpenCV_HASH SHA256=4dbabdc3bce8d698f798f3d48556c03f2ab3a2eb8d181f80d83349437e0331dd)
  else()
    set(OpenCV_URL "${OpenCV_BASEURL}/opencv-macos-${OpenCV_Version}-Debug.tar.gz")
    set(OpenCV_HASH SHA256=513747f41615a00e52744788af25fadcf5742e119441d9007c05826e35707417)
  endif()
elseif(MSVC)
  if(OpenCV_BUILD_TYPE STREQUAL Release)
    set(OpenCV_URL "${OpenCV_BASEURL}/opencv-windows-${OpenCV_Version}-Release.zip")
    set(OpenCV_HASH SHA256=b6da6b02616f1e8921e939467d32924426354b98b12a38d27e54b6c69d6da3f1)
  else()
    set(OpenCV_URL "${OpenCV_BASEURL}/opencv-windows-${OpenCV_Version}-Debug.zip")
    set(OpenCV_HASH SHA256=b003f479a67660f454206399b21d46041ccb00b944c9c400ea0cf7b4e35431e6)
  endif()
else()
  if(OpenCV_BUILD_TYPE STREQUAL Release)
    set(OpenCV_URL "${OpenCV_BASEURL}/opencv-linux-${OpenCV_Version}-Release.tar.gz")
    set(OpenCV_HASH SHA256=89281a1aad4f15c45459d6feacb2e69c2341154db46e3a756cd969ca77fffbbf)
  else()
    set(OpenCV_URL "${OpenCV_BASEURL}/opencv-linux-${OpenCV_Version}-Debug.tar.gz")
    set(OpenCV_HASH SHA256=4a7375f478ba520903246409e36fab1487407f833600c8a1ce4eaddc0bc6e876)
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
    INTERFACE ${opencv_SOURCE_DIR}/x64/vc17/staticlib/opencv_features2d481.lib
              ${opencv_SOURCE_DIR}/x64/vc17/staticlib/opencv_imgcodecs481.lib
              ${opencv_SOURCE_DIR}/x64/vc17/staticlib/opencv_imgproc481.lib
              ${opencv_SOURCE_DIR}/x64/vc17/staticlib/opencv_core481.lib
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
