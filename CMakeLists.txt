cmake_minimum_required(VERSION 3.2 FATAL_ERROR)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

find_package( OpenCV REQUIRED )
find_package( Threads )

set(EXEC_NAME "qrtest")

file(GLOB SRC ${PROJECT_SOURCE_DIR}/src/*.cpp)

add_executable(${EXEC_NAME}  ${SRC})    

target_include_directories(${EXEC_NAME} PRIVATE ${PROJECT_SOURCE_DIR}/include )

target_link_libraries(${EXEC_NAME} ${OpenCV_LIBS} zbar AirspaceCommon Threads::Threads)

add_subdirectory(${PROJECT_SOURCE_DIR}/test)