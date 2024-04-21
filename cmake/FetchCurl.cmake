include(FetchContent)

set(USE_SYSTEM_CURL
    OFF
    CACHE STRING "Use system Curl")

set(Curl_Version 8.7.1-3)

set(Curl_BASEURL "https://github.com/kaito-tokyo/kaito-tokyo-obs-dep-curl/releases/download/${Curl_Version}")

if(OS_MACOS)
  if(CMAKE_BUILD_TYPE STREQUAL Debug)
    set(Curl_URL "${Curl_BASEURL}/curl-macos-${Curl_Version}-Debug.tar.gz")
    set(Curl_HASH SHA256=b4e56a00e02e22e251441497975e27c9e46bf01bdd35c9fdf11e3cdb455cab67)
  elseif(CMAKE_BUILD_TYPE STREQUAL RelWithDebInfo)
    set(Curl_URL "${Curl_BASEURL}/curl-macos-${Curl_Version}-RelWithDebInfo.tar.gz")
    set(Curl_HASH SHA256=c70cf96a75c87628a6cd78d170513b99f6f95e558f90c3b8eaae3a01e7f7dcc9)
  else()
    set(Curl_URL "${Curl_BASEURL}/curl-macos-${Curl_Version}-Release.tar.gz")
    set(Curl_HASH SHA256=f3acb4b95370822068529fe5886ae338603401c0c4c859a23b27fa3aa8ddfdf4)
  endif()
elseif(OS_WINDOWS)
  if(CMAKE_BUILD_TYPE STREQUAL Debug)
    set(Curl_URL "${Curl_BASEURL}/curl-windows-${Curl_Version}-Debug.zip")
    set(Curl_HASH SHA256=eb2d7725e5ee98d18525caa569a0ded4ec432aa1d17719aa3582344d62e8fd2f)
  elseif(CMAKE_BUILD_TYPE STREQUAL RelWithDebInfo)
    set(Curl_URL "${Curl_BASEURL}/curl-windows-${Curl_Version}-RelWithDebInfo.zip")
    set(Curl_HASH SHA256=729bd2e8ead3d86f520fb25cb2a082cfff1e39c6e1943b420e8fc3c13a435ee3)
  else()
    set(Curl_URL "${Curl_BASEURL}/curl-windows-${Curl_Version}-Release.zip")
    set(Curl_HASH SHA256=a9a6fd354246e4f0d942e36cff3ce432b6027c19289d9afd4c133a5e93ef9ff9)
  endif()
endif()

if(NOT USE_SYSTEM_CURL AND (OS_MACOS OR OS_WINDOWS))
  FetchContent_Declare(
    curl
    URL ${Curl_URL}
    URL_HASH ${Curl_HASH})
  FetchContent_MakeAvailable(curl)
endif()

find_package(CURL REQUIRED HINTS "${curl_SOURCE_DIR}/lib/cmake" /usr)
