add_library(VisionTextRecognizer VisionTextRecognizer.mm)

target_link_libraries(VisionTextRecognizer PRIVATE ${OpenCV_LIBRARIES})
target_include_directories(VisionTextRecognizer PRIVATE ${OpenCV_INCLUDE_DIRS})

target_link_libraries(VisionTextRecognizer PRIVATE OBS::libobs plugin-support "-framework Vision"
                                                   "-framework CoreVideo" "-framework VideoToolbox")
target_include_directories(VisionTextRecognizer PRIVATE "${CMAKE_SOURCE_DIR}/src")
