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
  find_package(Qt6 COMPONENTS Widgets Core)
  target_link_libraries(${CMAKE_PROJECT_NAME} PRIVATE Qt6::Core Qt6::Widgets)
  target_compile_options(
    ${CMAKE_PROJECT_NAME} PRIVATE $<$<C_COMPILER_ID:Clang,AppleClang>:-Wno-quoted-include-in-framework-header
                                  -Wno-comma>)
  set_target_properties(
    ${CMAKE_PROJECT_NAME}
    PROPERTIES AUTOMOC ON
               AUTOUIC ON
               AUTORCC ON)
endif()

target_sources(${CMAKE_PROJECT_NAME} PRIVATE src/plugin-main.c src/screen-source.cpp src/factory.cpp
                                             src/ScreenBuilder/ActionHandler.cpp src/ScreenBuilder/StateMachine.cpp)

include(cmake/FetchOpenCV.cmake)
target_link_libraries(${CMAKE_PROJECT_NAME} PRIVATE ${OpenCV_LIBS})
target_include_directories(${CMAKE_PROJECT_NAME} PRIVATE ${OpenCV_INCLUDE_DIRS})

include(cmake/FetchCurl.cmake)
target_link_libraries(${CMAKE_PROJECT_NAME} PRIVATE CURL::libcurl)

# TextRecognizer
if(APPLE)
  add_subdirectory(src/TextRecognizer/VisionTextRecognizer)
  target_link_libraries(${CMAKE_PROJECT_NAME} PRIVATE VisionTextRecognizer)
elseif(UNIX)
  add_subdirectory(src/TextRecognizer/TesseractTextRecognizer)
  target_link_libraries(${CMAKE_PROJECT_NAME} PRIVATE TesseractTextRecognizer)
elseif(MSVC)
  add_subdirectory(src/TextRecognizer/WinRTTextRecognizer)
  target_link_libraries(${CMAKE_PROJECT_NAME} PRIVATE WinRTTextRecognizer)
endif()

target_include_directories(${CMAKE_PROJECT_NAME} PRIVATE ${CMAKE_SOURCE_DIR}/vendor/nlohmann-json/include)
set_target_properties_plugin(${CMAKE_PROJECT_NAME} PROPERTIES OUTPUT_NAME ${_name})
