set(include_dirs
    ${CMAKE_CURRENT_SOURCE_DIR}/include/
)

file(GLOB_RECURSE srcs
    ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp
)

add_library(show_renderer STATIC ${srcs})

target_include_directories(show_renderer PUBLIC
    ${include_dirs}
)

target_include_directories(show_renderer SYSTEM PUBLIC ${LIBYUV_INCLUDE_DIRS})

target_link_libraries(show_renderer
    Async++
    bnb_effect_player
    glad
    glfw
    yuv
)