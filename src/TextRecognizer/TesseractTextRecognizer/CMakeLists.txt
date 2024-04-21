add_library(TesseractTextRecognizer TesseractTextRecognizer.cpp)

target_link_libraries(TesseractTextRecognizer PRIVATE OBS::libobs plugin-support ${OpenCV_LIBS})
target_include_directories(TesseractTextRecognizer PRIVATE "${CMAKE_SOURCE_DIR}/src" ${OpenCV_INCLUDE_DIRS})
