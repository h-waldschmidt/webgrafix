cmake_minimum_required(VERSION 3.31)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_CXX_EXTENSIONS OFF)

project(webgrafix LANGUAGES C CXX)

include(CheckIPOSupported)

check_ipo_supported(RESULT supported OUTPUT error)

if(supported AND CMAKE_BUILD_TYPE STREQUAL "Release")
    message(STATUS "IPO / LTO enabled")
    set(CMAKE_INTERPROCEDURAL_OPTIMIZATION TRUE)
else()
    message(STATUS "IPO / LTO not enabled: <${error}>")
endif()

# set(CMAKE_CXX_FLAGS_DEBUG "-Wall -Wextra")

set(DAWN_FETCH_DEPENDENCIES ON)
add_subdirectory(third_party/dawn EXCLUDE_FROM_ALL)

find_program(SLANGC_EXECUTABLE slangc REQUIRED)

file(GLOB_RECURSE SOURCES CONFIGURE_DEPENDS ${PROJECT_SOURCE_DIR}/src/*.cpp)
file(GLOB_RECURSE HEADERS CONFIGURE_DEPENDS ${PROJECT_SOURCE_DIR}/src/*.h ${PROJECT_SOURCE_DIR}/src/*.hpp)

file(GLOB_RECURSE SLANG_SOURCE_FILES CONFIGURE_DEPENDS "${PROJECT_SOURCE_DIR}/assets/shaders/*.slang")
list(FILTER SLANG_SOURCE_FILES EXCLUDE REGEX "common\\.slang$")

add_executable(${PROJECT_NAME} ${HEADERS} ${SOURCES})

target_include_directories(${PROJECT_NAME} PRIVATE ${PROJECT_SOURCE_DIR}/src)

function(add_slang_shader_target TARGET)
  cmake_parse_arguments(PARSE_ARGV 1 ARG "" "" "SOURCES")

  if(NOT ARG_SOURCES)
    return()
  endif()

  set(SHADERS_OUTPUT_DIR ${CMAKE_CURRENT_BINARY_DIR}/shaders)
  set(WGSL_OUTPUTS "")

  foreach(SOURCE_FILE ${ARG_SOURCES})
    get_filename_component(FILE_NAME ${SOURCE_FILE} NAME_WE)
    set(OUTPUT_WGSL ${SHADERS_OUTPUT_DIR}/${FILE_NAME}.wgsl)

    add_custom_command(
      OUTPUT ${OUTPUT_WGSL}
      COMMAND ${CMAKE_COMMAND} -E make_directory ${SHADERS_OUTPUT_DIR}
      COMMAND ${SLANGC_EXECUTABLE} ${SOURCE_FILE} -target wgsl -o ${OUTPUT_WGSL}
      DEPENDS ${SOURCE_FILE}
      COMMENT "Compiling Slang Shader: ${SOURCE_FILE} to WGSL"
      VERBATIM)
    list(APPEND WGSL_OUTPUTS ${OUTPUT_WGSL})
  endforeach()

  add_custom_target(${TARGET} DEPENDS ${WGSL_OUTPUTS})
endfunction()

add_slang_shader_target(${PROJECT_NAME}_slang_shader SOURCES ${SLANG_SOURCE_FILES})

if(TARGET ${PROJECT_NAME}_slang_shader)
  add_dependencies(${PROJECT_NAME} ${PROJECT_NAME}_slang_shader)
endif()

if(EMSCRIPTEN)
    # TODO: THIS NEEDS to be fixed
    set_target_properties(${PROJECT_NAME} PROPERTIES SUFFIX ".html")
    # target_link_libraries(${PROJECT_NAME} PRIVATE emdawnwebgpu_cpp webgpu_glfw)
    add_compile_options("--use-port=emdawnwebgpu")
    target_link_options(${PROJECT_NAME} PRIVATE "-sASYNCIFY=1" "-sUSE_GLFW=3" "--use-port=emdawnwebgpu")
else()
    target_link_libraries(${PROJECT_NAME} PRIVATE webgpu_cpp webgpu_dawn glfw webgpu_glfw)
endif()
