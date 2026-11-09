cmake_minimum_required(VERSION 3.14)

set(CMAKE_CXX_STANDARD 20)

set(CMAKE_CXX_STANDARD_REQUIRED ON)

file(GLOB EXAMPLES_SOURCES "*.cpp")

foreach(SOURCE_FILE ${EXAMPLES_SOURCES})
    get_filename_component(EXAMPLE_NAME ${SOURCE_FILE} NAME_WE)
    
    add_executable(${EXAMPLE_NAME} ${SOURCE_FILE})
    
    target_link_libraries(${EXAMPLE_NAME} PRIVATE bmplib)
    
    message(STATUS "Добавлен пример: ${EXAMPLE_NAME}")
endforeach()