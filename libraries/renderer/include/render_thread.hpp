#pragma once

#include <glad/glad.h>
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#include <thread>

#include "renderer.hpp"

namespace render {

struct nv12_planes
{
    bnb::color_plane y_plane;
    bnb::color_plane uv_plane;
};

class render_thread
{
public:
    render_thread(GLFWwindow* window, int32_t width, int32_t height);

    ~render_thread();

    void surface_changed(int32_t width, int32_t height);

    void update_data(bnb::full_image_t image);
    void draw();

private:
    void thread_func();

    std::unique_ptr<bnb::renderer> m_renderer { nullptr };
    GLFWwindow* m_window;
    std::thread m_thread;
    std::atomic<bool> m_cancellation_flag;

    int32_t m_cur_width;
    int32_t m_cur_height;

    nv12_planes m_update_buffer;
    nv12_planes m_show_buffer;

    std::atomic<bool> m_using_frame = false;
    std::atomic<bool> m_need_swap = false;
};

} //render
