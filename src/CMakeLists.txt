
add_executable(${PROJECT_NAME} main.cpp)

if(EMSCRIPTEN)
    set_target_properties(${PROJECT_NAME} PROPERTIES SUFFIX ".html")
    target_link_libraries(${PROJECT_NAME} PRIVATE emdawnwebgpu_cpp webgpu_glfw)
    target_link_options(${PROJECT_NAME} PRIVATE "-sASYNCIFY=1" "-sUSE_GLFW=3")
else()
    target_link_libraries(${PROJECT_NAME} PRIVATE dawn::webgpu_dawn glfw webgpu_glfw)
endif()
