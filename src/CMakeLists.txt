find_package(
  Qt5
  COMPONENTS 
  Core
  Widgets
  OpenGL
  REQUIRED)

SET(CMAKE_AUTOMOC ON)
SET(CMAKE_AUTORCC ON)
SET(CMAKE_AUTOUIC ON)
SET(CXX_STANDARD 11)

# aux_source_directory("${CMAKE_CURRENT_SOURCE_DIR}" SOURCE)
set(SOURCE main.cpp MainWindow.cpp EasyGLWidget.cpp GLADWidget.cpp GLEWWidget.cpp)
add_executable(${PROJECT_NAME} ${SOURCE})
target_include_directories(${PROJECT_NAME} PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/../thirdparty/glew/include")
target_link_libraries(${PROJECT_NAME} PRIVATE Qt5::Widgets Qt5::OpenGL EasyGL glad glew)
message("${CMAKE_CURRENT_SOURCE_DIR}/../thirdparty/glew/include")