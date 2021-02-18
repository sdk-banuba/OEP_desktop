#pragma once

#include <bnb/types/full_image.hpp>

#include "program.hpp"
#include "renderer_gl_context.hpp"

namespace bnb
{
    class renderer
    {
    public:
        renderer(int width, int height);

        void surface_changed(int32_t width, int32_t height);
        void draw(color_plane y_plane, color_plane uv_plane);

    private:
        void update_camera_texture(color_plane y_plane, color_plane uv_plane);

        renderer_gl_context m_gl_context;
        program m_program;

        int m_cur_width;
        int m_cur_height;
    };
}
