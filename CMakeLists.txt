cmake_minimum_required(VERSION 3.2 FATAL_ERROR)


find_package( OpenCV REQUIRED )

add_executable(qrtest  camtest.cpp qrzbar.cpp)

target_include_directories(qrtest PRIVATE ${PROJECT_SOURCE_DIR}/lib )


target_link_libraries(qrtest ${OpenCV_LIBS} zbar)