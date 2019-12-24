cmake_minimum_required( VERSION 2.8 )
project( delaunay )

# 添加c++ 11标准支持
set( CMAKE_CXX_FLAGS "-std=c++11" )

# 寻找OpenCV库
find_package( OpenCV REQUIRED )
# 添加头文件
include_directories( ${OpenCV_INCLUDE_DIRS} )

add_executable( delaunay delaunay.cpp )
add_executable( delaunay2 delaunay2.cpp )

#add_executable( Mat Mat_learn.cpp )
# 链接OpenCV库
target_link_libraries( delaunay ${OpenCV_LIBS} )

target_link_libraries( delaunay2 ${OpenCV_LIBS} )