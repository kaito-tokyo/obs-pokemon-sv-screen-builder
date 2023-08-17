cmake_minimum_required(VERSION 3.16...3.26)

include("${CMAKE_CURRENT_SOURCE_DIR}/cmake/common/bootstrap.cmake" NO_POLICY_SCOPE)

project(${_name} VERSION ${_version})

option(ENABLE_FRONTEND_API "Use obs-frontend-api for UI functionality" OFF)
option(ENABLE_QT "Use Qt functionality" OFF)

include(compilerconfig)
include(defaults)
include(helpers)

if(MSVC)
  find_program(CCACHE_EXE ccache)
  if(CCACHE_EXE)
    file(COPY_FILE ${CCACHE_EXE} ${CMAKE_BINARY_DIR}/cl.exe)
    set(CMAKE_VS_GLOBALS CLToolExe=cl.exe CLToolPath=${CMAKE_BINARY_DIR} TrackFileAccess=false UseMultiToolTask=true
                         DebugInformationFormat=OldStyle)
  endif()
endif()

add_library(${CMAKE_PROJECT_NAME} MODULE)

find_package(libobs REQUIRED)
target_link_libraries(${CMAKE_PROJECT_NAME} PRIVATE OBS::libobs)

if(ENABLE_FRONTEND_API)
  find_package(obs-frontend-api REQUIRED)
  target_link_libraries(${CMAKE_PROJECT_NAME} PRIVATE OBS::obs-frontend-api)
endif()

if(ENABLE_QT)
  find_qt(COMPONENTS Widgets Core)
  target_link_libraries(${CMAKE_PROJECT_NAME} PRIVATE Qt::Core Qt::Widgets)
  target_compile_options(
    ${CMAKE_PROJECT_NAME} PRIVATE $<$<C_COMPILER_ID:Clang,AppleClang>:-Wno-quoted-include-in-framework-header
                                  -Wno-comma>)
  set_target_properties(
    ${CMAKE_PROJECT_NAME}
    PROPERTIES AUTOMOC ON
               AUTOUIC ON
               AUTORCC ON)
endif()

target_sources(
  ${CMAKE_PROJECT_NAME}
  PRIVATE src/plugin-main.c
          src/screen-source.cpp
          src/factory.cpp
          src/ScreenBuilder/ActionHandler.cpp
          src/ScreenBuilder/StateMachine.cpp
          src/UpdateChecker/UpdateDialog.cpp
          src/UpdateChecker/UpdateChecker.cpp)

include(cmake/FetchOpenCV.cmake)
target_link_libraries(${CMAKE_PROJECT_NAME} PRIVATE OpenCV)

# TextRecognizer
if(APPLE)
  target_sources(${CMAKE_PROJECT_NAME} PRIVATE src/TextRecognizer/VisionTextRecognizer/VisionTextRecognizer.mm
                                               src/UpdateChecker/URLSessionClient/URLSessionClient.mm)
  target_link_libraries(${CMAKE_PROJECT_NAME} PRIVATE "-framework Vision" "-framework CoreVideo"
                                                      "-framework VideoToolbox")
elseif(MSVC)
  add_subdirectory(src/TextRecognizer/WinRTTextRecognizer)
  target_link_libraries(${CMAKE_PROJECT_NAME} PRIVATE WinRTTextRecognizer)
endif()

# UpdateChecker
if(APPLE)
  add_subdirectory(src/UpdateChecker/URLSessionClient)
  target_link_libraries(${CMAKE_PROJECT_NAME} PRIVATE URLSessionClient)
elseif(UNIX)
  add_subdirectory(src/UpdateChecker/CurlClient)
  target_link_libraries(${CMAKE_PROJECT_NAME} PRIVATE CurlClient)
elseif(MSVC)
  add_subdirectory(src/UpdateChecker/WinRTHttpClient)
  target_link_libraries(${CMAKE_PROJECT_NAME} PRIVATE WinRTHttpClient)
endif()

target_include_directories(${CMAKE_PROJECT_NAME} PRIVATE ${CMAKE_SOURCE_DIR}/vendor/nlohmann-json/include)
target_include_directories(${CMAKE_PROJECT_NAME} PRIVATE ${CMAKE_SOURCE_DIR}/vendor/nameof/include)

set_target_properties_plugin(${CMAKE_PROJECT_NAME} PROPERTIES OUTPUT_NAME ${_name})
