add_library(URLSessionClient URLSessionClient.mm)
target_link_libraries(URLSessionClient PRIVATE OBS::libobs plugin-support OpenCV "-framework Vision"
                                               "-framework CoreVideo" "-framework VideoToolbox")
target_include_directories(URLSessionClient PRIVATE "${CMAKE_SOURCE_DIR}/src")
