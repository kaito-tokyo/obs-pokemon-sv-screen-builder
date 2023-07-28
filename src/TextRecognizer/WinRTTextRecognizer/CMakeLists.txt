add_library(WinRTTextRecognizer pch.cpp pch.h textRecognizerImpl.cpp)
set_target_properties(
  WinRTTextRecognizer
  PROPERTIES CXX_STANDARD 17
             CXX_STANDARD_REQUIRED ON
             CXX_EXTENSIONS OFF
             VS_GLOBAL_CppWinRTOptimized true
             VS_GLOBAL_CppWinRTRootNamespaceAutoMerge true)
