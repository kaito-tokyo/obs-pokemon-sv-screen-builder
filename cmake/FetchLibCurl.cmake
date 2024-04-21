include(FetchContent)

set(USE_SYSTEM_OPENCV OFF CACHE STRING "Use system OpenCV")

set(LibCurl_Version 8.7.1-1)

if(${CMAKE_BUILD_TYPE} STREQUAL Debug)
  set(LibCurl_BUILD_TYPE Debug)
else()
  set(LibCurl_BUILD_TYPE Release)
endif()

set(LibCurl_BASEURL "https://github.com/kaito-tokyo/kaito-tokyo-obs-dep-libcurl/releases/download/${LibCurl_Version}")

if(NOT USE_SYSTEM_OPENCV)
  if(OS_MACOS)
    if(LibCurl_BUILD_TYPE STREQUAL Debug)
      set(LibCurl_URL "${LibCurl_BASEURL}/libcurl-macos-${LibCurl_Version}-Debug.tar.gz")
      set(LibCurl_HASH SHA256=47a324c439cb1070cf63c3000f2dda6c021a9a705b3f3583fbcb09837ae8a737)
    else()
      set(LibCurl_URL "${LibCurl_BASEURL}/libcurl-macos-${LibCurl_Version}-Release.tar.gz")
      set(LibCurl_HASH SHA256=ac29d6e02e7141748e6dba55f73495803affab12886db2d7ae9ea0f29b626ae4)
    endif()
  elseif(OS_WINDOWS)
    if(LibCurl_BUILD_TYPE STREQUAL Debug)
      set(LibCurl_URL "${LibCurl_BASEURL}/libcurl-windows-${LibCurl_Version}-Debug.zip")
      set(LibCurl_HASH SHA256=56a2530c530be8a2a0e1692dce132561d9a5153dc647eeb246c63e1f03b211ac)
    else()
      set(LibCurl_URL "${LibCurl_BASEURL}/libcurl-windows-${LibCurl_Version}-Release.zip")
      set(LibCurl_HASH SHA256=63a19f95297e1bea80d036fcf635d3fb5ab8b0095e60a17342e40d449e350312)
    endif()
  endif()
endif()

if(OS_LINUX)
  find_package(CURL REQUIRED)
else()
  FetchContent_Declare(
    LibCurl
    URL ${LibCurl_URL}
    URL_HASH ${LibCurl_HASH})
  FetchContent_MakeAvailable(LibCurl)

  find_package(CURL REQUIRED)
endif()

target_link_libraries(${CMAKE_PROJECT_NAME} PRIVATE "${CURL_LIBRARIES}")
target_include_directories(${CMAKE_PROJECT_NAME} SYSTEM PRIVATE "${CURL_INCLUDE_DIRS}")
