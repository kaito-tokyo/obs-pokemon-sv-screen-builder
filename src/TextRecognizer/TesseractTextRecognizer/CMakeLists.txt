add_library(TesseractTextRecognizer TesseractTextRecognizer.cpp)

target_link_libraries(VisionTextRecognizer PRIVATE OBS::libobs plugin-support ${OpenCV_LIBRARIES})
target_include_directories(VisionTextRecognizer PRIVATE "${CMAKE_SOURCE_DIR}/src" ${OpenCV_INCLUDE_DIRS})
