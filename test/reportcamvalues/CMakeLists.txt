cmake_minimum_required(VERSION 3.2 FATAL_ERROR)

find_package( OpenCV REQUIRED )
find_package( Threads )

set(EXEC_NAME "reportcamvalues")

file(GLOB SRC ${PROJECT_SOURCE_DIR}/test/reportcamvalues/*.cpp)

add_executable(${EXEC_NAME}  ${SRC})    

#target_include_directories(${EXEC_NAME} PRIVATE ${PROJECT_SOURCE_DIR}/include )

target_link_libraries(${EXEC_NAME} AirspaceCommon Threads::Threads)
