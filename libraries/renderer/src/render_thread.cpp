#include "render_thread.hpp"

#include <libyuv.h>

namespace render {

render_thread::render_thread(GLFWwindow* window, int32_t width, int32_t height)
    : m_window(window)
    , m_thread([this]() { thread_func(); })
    , m_cancellation_flag(false)
    , m_cur_width(width)
    , m_cur_height(height) {}

render_thread::~render_thread()
{
    m_cancellation_flag = true;
    m_thread.join();
}

void render_thread::surface_changed(int32_t width, int32_t height)
{
    m_cur_width = width;
    m_cur_height = height;

    if (m_renderer) {
        m_renderer->surface_changed(width, height);
    }
}

void render_thread::update_data(bnb::full_image_t image)
{
    if (m_need_swap && m_using_frame) {
        return;
    }

    m_need_swap = false;
    const auto& yuv = image.get_data<bnb::yuv_image_t>();
    m_update_buffer.y_plane = yuv.y_plane;
    m_update_buffer.uv_plane = yuv.uv_plane;
    m_need_swap = true;
}

void render_thread::draw()
{
    m_using_frame = true;

    std::swap(m_update_buffer, m_show_buffer);
    m_need_swap = false;

    m_renderer->draw(m_show_buffer.y_plane, m_show_buffer.uv_plane);
    glfwSwapBuffers(m_window);

    m_using_frame = false;
}

void render_thread::thread_func()
{
    using namespace std::chrono_literals;

    glfwMakeContextCurrent(m_window);
    glfwSwapInterval(1);

    m_renderer = std::make_unique<bnb::renderer>(m_cur_width, m_cur_height);

    while (!m_cancellation_flag) {
        if (m_need_swap) {
            draw();
        } else {
            std::this_thread::sleep_for(1us);
        }
    }

    glfwMakeContextCurrent(nullptr);
}

} //render