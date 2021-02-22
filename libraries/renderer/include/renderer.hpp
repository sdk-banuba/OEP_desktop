#pragma once

#include <bnb/types/full_image.hpp>

#include "program.hpp"
#include "renderer_gl_context.hpp"

namespace render
{
    struct nv12_planes
    {
        bnb::color_plane y_plane;
        bnb::color_plane uv_plane;
    };

    class renderer
    {
    public:
        renderer(int width, int height);

        void surface_changed(int32_t width, int32_t height);

        void update_data(bnb::full_image_t image);
        bool draw();

    private:
        void update_camera_texture();

        renderer_gl_context m_gl_context;
        bnb::program m_program;

        int m_cur_width;
        int m_cur_height;

        nv12_planes m_update_buffer;
        nv12_planes m_show_buffer;

        std::atomic<bool> m_rendering = false;
        std::atomic<bool> m_texture_updated = false;
    };
}
