include(FetchContent)

set(USE_SYSTEM_CURL
    OFF
    CACHE STRING "Use system Curl")

set(Curl_Version 8.7.1-4)

set(Curl_BASEURL "https://github.com/kaito-tokyo/kaito-tokyo-obs-dep-curl/releases/download/${Curl_Version}")

if(OS_MACOS)
  if(CMAKE_BUILD_TYPE STREQUAL Debug)
    set(Curl_URL "${Curl_BASEURL}/curl-macos-${Curl_Version}-Debug.tar.gz")
    set(Curl_HASH SHA256=d8b8777a6e00be2f645f9039e40adeb66467ee2127a3b7756313b92890d4f887)
  elseif(CMAKE_BUILD_TYPE STREQUAL RelWithDebInfo)
    set(Curl_URL "${Curl_BASEURL}/curl-macos-${Curl_Version}-RelWithDebInfo.tar.gz")
    set(Curl_HASH SHA256=f3b92726b0b368bbc0f50e28d2fbfcc75faa5406a49e8675d29b52a9e0a64c4c)
  else()
    set(Curl_URL "${Curl_BASEURL}/curl-macos-${Curl_Version}-Release.tar.gz")
    set(Curl_HASH SHA256=d3ab033f6a1e01e282c4e8c764d6fc99ace0cecbb4b68f6495f84356809967ac)
  endif()
elseif(OS_WINDOWS)
  if(CMAKE_BUILD_TYPE STREQUAL Debug)
    set(Curl_URL "${Curl_BASEURL}/curl-windows-${Curl_Version}-Debug.zip")
    set(Curl_HASH SHA256=04385be061c0b0598e0a42a8b380ded9854eae04df02ee5226f0e57915e1126c)
  elseif(CMAKE_BUILD_TYPE STREQUAL RelWithDebInfo)
    set(Curl_URL "${Curl_BASEURL}/curl-windows-${Curl_Version}-RelWithDebInfo.zip")
    set(Curl_HASH SHA256=06d51772f759e368f8eecfbdbd7196d20709af18869708094211afa873a1b00f)
  else()
    set(Curl_URL "${Curl_BASEURL}/curl-windows-${Curl_Version}-Release.zip")
    set(Curl_HASH SHA256=630aef8cf2425884d2cd7c50fbd10094a25984f9db88bcd219746fab415b29c9)
  endif()
endif()

if(NOT USE_SYSTEM_CURL AND (OS_MACOS OR OS_WINDOWS))
  FetchContent_Declare(
    curl
    URL ${Curl_URL}
    URL_HASH ${Curl_HASH})
  FetchContent_MakeAvailable(curl)
  find_package(CURL REQUIRED HINTS "${curl_SOURCE_DIR}/lib/cmake")
else()
  find_package(CURL REQUIRED)
endif()
