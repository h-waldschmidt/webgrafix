cmake_minimum_required(VERSION 3.19)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

project(webgrafix LANGUAGES C CXX)

include(CheckIPOSupported)

check_ipo_supported(RESULT supported OUTPUT error)

if(supported AND CMAKE_BUILD_TYPE STREQUAL "Release")
    message(STATUS "IPO / LTO enabled")
    set(CMAKE_INTERPROCEDURAL_OPTIMIZATION TRUE)
else()
    message(STATUS "IPO / LTO not enabled: <${error}>")
endif()

# an alternative would be to use -Weverything
# set(CMAKE_CXX_FLAGS_DEBUG "-Wall -Wextra")

# sometimes it can't hurt to use "-Wall -Wextra" for release
# NOTE: This is reduntant, as the default is already "-O3"
set(CMAKE_CXX_FLAGS_RELEASE "-O3")

set(DAWN_FETCH_DEPENDENCIES ON)
add_subdirectory(third_party/dawn)

add_subdirectory(src)
