add_library(WinRTTextRecognizer WinRTTextRecognizer.cpp)
set_target_properties(
  WinRTTextRecognizer
  PROPERTIES CXX_STANDARD 17
             CXX_STANDARD_REQUIRED ON
             CXX_EXTENSIONS OFF
             VS_GLOBAL_CppWinRTOptimized true
             VS_GLOBAL_CppWinRTRootNamespaceAutoMerge true)
target_link_libraries(WinRTTextRecognizer PRIVATE OBS::libobs plugin-support OpenCV)
target_include_directories(WinRTTextRecognizer PRIVATE "${CMAKE_SOURCE_DIR}/src")
