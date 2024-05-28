set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/thirdparty)

add_subdirectory(EasyGL)
add_subdirectory(glad/cmake)

execute_process(COMMAND make -C ${CMAKE_CURRENT_SOURCE_DIR}/glew extensions)
add_subdirectory(glew/build/cmake)