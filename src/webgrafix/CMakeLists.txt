set(HEADER_FILES Webgrafix.h)
set(SOURCE_FILES Webgrafix.cpp)

find_package(spdlog CONFIG REQUIRED)

add_library(webgrafix SHARED ${HEADER_FILES} ${SOURCE_FILES})

target_link_libraries(webgrafix PRIVATE spdlog::spdlog)
target_link_libraries(webgrafix PRIVATE dawn::webgpu_dawn)
