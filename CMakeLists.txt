cmake_minimum_required(VERSION 3.10)

if (NOT CMAKE_BUILD_TYPE)
    message(STATUS "No build type selected, default to MinSizeRel")
    set(CMAKE_BUILD_TYPE "MinSizeRel" CACHE STRING "Build type")
endif()

project("c4ev3/EV3-API"
        VERSION 1.0.0
        LANGUAGES C
        DESCRIPTION "EV3-API for Programming the LEGO Mindstorms EV3 in C")

# add compiler options
add_compile_options(-fno-strict-aliasing -fwrapv)
add_compile_options(-Wall -Wextra -Wpointer-sign -Wno-unused-parameter)
add_compile_options(-fdata-sections -ffunction-sections)
add_compile_options(-Wl,--gc-sections)

########################################################################
# LIBRARY

# allow deactivation of timers
option(DISABLE_TIMERS "Disable timers" OFF)
set_property(CACHE DISABLE_TIMERS
             PROPERTY HELPSTRING
             "Disable the ev3_timer module. This will prevent periodic syscall interruptions caused by the SIGALRM signal.")

# add sources
FILE(GLOB_RECURSE EV3API_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/API/*.c)
FILE(GLOB_RECURSE EV3API_HEADERS ${CMAKE_CURRENT_SOURCE_DIR}/API/*.h)
FILE(GLOB_RECURSE EV3API_BITMAPS ${CMAKE_CURRENT_SOURCE_DIR}/API/*.xbm)

# define library
add_library(ev3api STATIC ${EV3API_SOURCES} ${EV3API_HEADERS} ${EV3API_BITMAPS})
add_library(c4ev3::ev3api ALIAS ev3api)

# and its include directories
target_include_directories(ev3api PUBLIC $<INSTALL_INTERFACE:include>
                                         $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/API>)
# and its macros
if (DISABLE_TIMERS)
    target_compile_definitions(c4ev3::ev3api PUBLIC -DDISABLE_TIMERS=1)
endif()
