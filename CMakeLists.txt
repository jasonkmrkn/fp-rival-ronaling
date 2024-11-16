cmake_minimum_required (VERSION 3.8)

project ("final-project")

# Set C++ Standart
set (CMAKE_CXX_STANDARD 20)
set (CMAKE_CXX_STANDARD_REQUIRED True)

# Find OpenCV package
find_package (OpenCV REQUIRED)
find_package(Threads REQUIRED)

include_directories (
	${OpenCV_INCLUDE_DIRS}
	"include/"
)

# Set Sources
set ( SOURCES 
	"src/final-project.cpp" 
	"src/qrcode.cpp"
	"src/objDetect.cpp"
	"include/qrcode.h"
	"include/objDetect.h"
	"include/communicate.h"
	"src/communicate.cpp"
)

# Add source to this project's executable.
add_executable (final-project ${SOURCES})

# TODO: Add tests and install targets if needed.

target_link_libraries (final-project 
	${OpenCV_LIBS}
)
