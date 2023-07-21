cmake_minimum_required(VERSION 3.16...3.26)

include("${CMAKE_CURRENT_SOURCE_DIR}/cmake/common/bootstrap.cmake" NO_POLICY_SCOPE)

project(${_name} VERSION ${_version})

option(ENABLE_FRONTEND_API "Use obs-frontend-api for UI functionality" OFF)
option(ENABLE_QT "Use Qt functionality" OFF)

include(compilerconfig)
include(defaults)
include(helpers)

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
          src/ScreenBuilder/ActionHandler.cpp
          src/ScreenBuilder/Automaton.cpp
          src/modules/DefaultLayoutCreatedDialog.cpp
          src/modules/EntityCropper.cpp
          src/modules/Logger.cpp
          src/modules/OpponentRankExtractor.cpp
          src/modules/OpponentRankExtractorGenerated.cpp
          src/modules/MyRankExtractor.cpp
          src/modules/MyRankExtractorGenerated.cpp
          src/modules/SceneDetector.cpp
          src/modules/SceneDetectorTextRecognizerGenerated.cpp
          src/modules/SelectionRecognizer.cpp
          src/modules/SelectionRecognizerGenerated.cpp
          src/modules/PokemonRecognizer.cpp
          src/modules/PokemonRecognizerGenerated.cpp
          src/update-checker/UpdateDialog.cpp
          src/update-checker/update-checker.cpp)

if(APPLE)
  target_sources(${CMAKE_PROJECT_NAME} PRIVATE src/TextRecognizer/VisionTextRecognizer/VisionTextRecognizer.mm)
  target_link_libraries(${CMAKE_PROJECT_NAME} PRIVATE "-framework Vision" "-framework CoreVideo"
                                                      "-framework VideoToolbox")
endif()

include(cmake/BuildMyOpenCV.cmake)
target_link_libraries(${CMAKE_PROJECT_NAME} PRIVATE OpenCV)

include(cmake/BuildMyCurl.cmake)
target_link_libraries(${CMAKE_PROJECT_NAME} PRIVATE libcurl)

target_include_directories(${CMAKE_PROJECT_NAME} PRIVATE ${CMAKE_SOURCE_DIR}/vendor/nlohmann-json/include)
target_include_directories(${CMAKE_PROJECT_NAME} PRIVATE ${CMAKE_SOURCE_DIR}/vendor/nameof/include)

if(MSVC)
  set(WinRTTextRecognizer_DIR "${CMAKE_SOURCE_DIR}/src/TextRecognizer/WinRTTextRecognizer")
  include_external_msproject(WinRTTextRecognizer "${WinRTTextRecognizer_DIR}/WinRTTextRecognizer.vcxproj")
  target_link_libraries(
    ${CMAKE_PROJECT_NAME}
    PRIVATE "${WinRTTextRecognizer_DIR}/x64/${CMAKE_BUILD_TYPE}/WinRTTextRecognizer/WinRTTextRecognizer.lib")
  add_dependencies(${CMAKE_PROJECT_NAME} WinRTTextRecognizer)
endif()

set_target_properties_plugin(${CMAKE_PROJECT_NAME} PROPERTIES OUTPUT_NAME ${_name})
