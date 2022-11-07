cmake_minimum_required(VERSION 2.8)
project( DisplayImage )
add_definitions("-std=c++11 -g")
set(CMAKE_EXPORT_COMPILE_COMMANDS true)

set(HEADER_DIR src/include)
set(SRC_DIR src)
set(SOURCES ${SRC_DIR}/vec3.cpp 
	${SRC_DIR}/main.cpp 
	# ${HEADER_DIR}/camera.h
	# ${HEADER_DIR}/color.h
	# ${HEADER_DIR}/defuse.h
	# ${HEADER_DIR}/hittable.h
	# ${HEADER_DIR}/hittable_list.h
	# ${HEADER_DIR}/inf_plane.h
	# ${HEADER_DIR}/material.h
	# ${HEADER_DIR}/ray.h
	# ${HEADER_DIR}/rtweek.h
	# ${HEADER_DIR}/sphere.h
	# ${HEADER_DIR}/vec3.h
	)

include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
conan_basic_setup()

add_executable( DisplayImage ${SOURCES} )
target_include_directories( DisplayImage PRIVATE ${HEADER_DIR})

target_link_libraries(DisplayImage ${CONAN_LIBS})
