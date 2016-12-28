CMAKE_MINIMUM_REQUIRED (VERSION 2.6)

PROJECT( gogo )

SET (CURRENT_DIR ${CMAKE_SOURCE_DIR})
LINK_DIRECTORIES( ${CMAKE_SOURCE_DIR}/lib/sdl/lib		 )
LINK_DIRECTORIES( ${CMAKE_SOURCE_DIR}/lib/sdl_mixer/lib )
LINK_DIRECTORIES( ${CMAKE_SOURCE_DIR}/lib/sdl_image/lib )
if(UNIX)
	CHECK_CXX_COMPILER_FLAG("-std=c++11" COMPILER_SUPPORTS_CXX11)
	CHECK_CXX_COMPILER_FLAG("-std=c++0x" COMPILER_SUPPORTS_CXX0X)

	find_package (Threads)
	add_definitions("-DLINUX -O2 -s -Wall -Dlinux")

	if(COMPILER_SUPPORTS_CXX11)
		set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
	elseif(COMPILER_SUPPORTS_CXX0X)
		set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++0x")
	else()
	    message(STATUS "The compiler ${CMAKE_CXX_COMPILER} has no
			C++11 support. Please use a different C++ compiler.")
	endif()
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ")
endif(UNIX)

FILE( GLOB_RECURSE base
	
	 )
FILE( GLOB_RECURSE impl
	inc/*.h
	src/*.cpp
	src/*.c
	 )
FILE( GLOB_RECURSE test
	
	 )
#MakeGroup( "base" "base" "CURRENT_DIR" )
INCLUDE_DIRECTORIES ( ${CMAKE_SOURCE_DIR}/lib/sdl/include)
INCLUDE_DIRECTORIES ( ${CMAKE_SOURCE_DIR}/lib/sdl_mixer/include)
INCLUDE_DIRECTORIES ( ${CMAKE_SOURCE_DIR}/lib/sdl_image/include)
INCLUDE_DIRECTORIES ( ${CMAKE_SOURCE_DIR}/inc)

ADD_EXECUTABLE( gogo
	${impl}
	)

TARGET_LINK_LIBRARIES( gogo SDL)
TARGET_LINK_LIBRARIES( gogo SDLmain)
TARGET_LINK_LIBRARIES( gogo SDL_image)
TARGET_LINK_LIBRARIES( gogo SDL_mixer)