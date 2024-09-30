
add_subdirectory(webgrafix)

add_executable(${PROJECT_NAME}_main main.cpp)

target_link_libraries(${PROJECT_NAME}_main PRIVATE webgrafix)

