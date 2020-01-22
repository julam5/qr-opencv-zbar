cmake_minimum_required(VERSION 3.2 FATAL_ERROR)


find_package( OpenCV REQUIRED )

add_executable(qrtest  ${PROJECT_SOURCE_DIR}/src/main.cpp ${PROJECT_SOURCE_DIR}/src/qrzbar.cpp)

target_include_directories(qrtest PRIVATE ${PROJECT_SOURCE_DIR}/include )


target_link_libraries(qrtest ${OpenCV_LIBS} zbar)