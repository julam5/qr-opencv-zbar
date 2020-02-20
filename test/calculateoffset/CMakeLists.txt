cmake_minimum_required(VERSION 3.2 FATAL_ERROR)

find_package( OpenCV REQUIRED )
find_package( Threads )

set(EXEC_NAME "calculateoffset")

file(GLOB SRC ${PROJECT_SOURCE_DIR}/test/calculateoffset/*.cpp)

add_executable(${EXEC_NAME}  ${SRC})    

#target_include_directories(${EXEC_NAME} PRIVATE ${PROJECT_SOURCE_DIR}/include )

target_link_libraries(${EXEC_NAME} AirspaceCommon Threads::Threads)
