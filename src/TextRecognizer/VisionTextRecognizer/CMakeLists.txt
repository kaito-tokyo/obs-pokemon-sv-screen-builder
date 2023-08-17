add_library(VisionTextRecognizer VisionTextRecognizer.mm)
target_link_libraries(VisionTextRecognizer PRIVATE OBS::libobs plugin-support OpenCV "-framework Vision"
                                                   "-framework CoreVideo" "-framework VideoToolbox")
target_include_directories(VisionTextRecognizer PRIVATE "${CMAKE_SOURCE_DIR}/src")
