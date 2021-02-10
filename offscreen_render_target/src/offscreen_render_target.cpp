#include "offscreen_render_target.hpp"

#include "opengl.hpp"

#include <bnb/effect_player/utility.hpp>
#include <bnb/postprocess/interfaces/postprocess_helper.hpp>

namespace
{
    const char* vs_default_base =
            " precision highp float; \n "
            " layout (location = 0) in vec3 aPos; \n"
            " layout (location = 1) in vec2 aTexCoord; \n"
            "out vec2 vTexCoord;\n"
            "void main()\n"
            "{\n"
                " gl_Position = vec4(aPos, 1.0); \n"
                " vTexCoord = aTexCoord; \n"
            "}\n";

    const char* ps_default_base =
            "precision mediump float;\n"
            "in vec2 vTexCoord;\n"
            "out vec4 FragColor;\n"
            "uniform sampler2D uTexture;\n"
            "void main()\n"
            "{\n"
                "FragColor = texture(uTexture, vTexCoord);\n"
            "}\n";
}

namespace bnb
{
    offscreen_render_target::offscreen_render_target(uint32_t width, uint32_t height)
        : m_width(width)
        , m_height(height) {}

    offscreen_render_target::~offscreen_render_target()
    {
        if (m_framebuffer != 0) {
            GL_CALL(glDeleteFramebuffers(1, &m_framebuffer));
        }
        if (m_post_processing_framebuffer != 0) {
            GL_CALL(glDeleteFramebuffers(1, &m_post_processing_framebuffer));
        }
    }

    void offscreen_render_target::init()
    {
        create_context();
        activate_context();

        GL_CALL(glGenFramebuffers(1, &m_framebuffer));
        GL_CALL(glGenFramebuffers(1, &m_post_processing_framebuffer));
        GL_CALL(glBindFramebuffer(GL_FRAMEBUFFER, m_framebuffer));

        m_program = std::make_unique<program>("OrientationChange", vs_default_base, ps_default_base);
        m_frame_surface_handler = std::make_unique<frame_surface_handler>(bnb::camera_orientation::deg_0, false);
    }

    void offscreen_render_target::create_context()
    {
        glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE);

        renderer_context.reset();
        renderer_context = std::shared_ptr<GLFWwindow>(glfwCreateWindow(m_width, m_height, "", nullptr, nullptr));

        glfwMakeContextCurrent(renderer_context.get());
        load_glad_functions();
        GL_CALL(glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS));
        glfwMakeContextCurrent(nullptr);
    }

    void offscreen_render_target::activate_context()
    {
        if (renderer_context) {
            glfwMakeContextCurrent(renderer_context.get());
        }
    }

    void offscreen_render_target::load_glad_functions()
    {
    #if BNB_OS_WINDOWS || BNB_OS_MACOS
        // it's only need for use while working with dynamic libs
        utility::load_glad_functions((GLADloadproc) glfwGetProcAddress);
        bnb::interfaces::postprocess_helper::load_glad_functions(reinterpret_cast<int64_t>(glfwGetProcAddress));
    #endif

        if (0 == gladLoadGLLoader((GLADloadproc) glfwGetProcAddress)) {
            throw std::runtime_error("gladLoadGLLoader error");
        }
    }

    void offscreen_render_target::generate_texture(GLuint& texture)
    {
        GL_CALL(glGenTextures(1, &texture));
        GL_CALL(glBindTexture(GL_TEXTURE_2D, texture));
        GL_CALL(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA,  m_width, m_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL));

        GL_CALL(glTexParameteri(GLenum(GL_TEXTURE_2D), GLenum(GL_TEXTURE_MIN_FILTER), GL_NEAREST));
        GL_CALL(glTexParameteri(GLenum(GL_TEXTURE_2D), GLenum(GL_TEXTURE_MAG_FILTER), GL_NEAREST));
        GL_CALL(glTexParameterf(GLenum(GL_TEXTURE_2D), GLenum(GL_TEXTURE_WRAP_S), GLfloat(GL_CLAMP_TO_EDGE)));
        GL_CALL(glTexParameterf(GLenum(GL_TEXTURE_2D), GLenum(GL_TEXTURE_WRAP_T), GLfloat(GL_CLAMP_TO_EDGE)));
    }

    void offscreen_render_target::prepare_rendering()
    {
        if (m_offscreen_render_texture == 0) {
            generate_texture(m_offscreen_render_texture);
        }

        GL_CALL(glBindFramebuffer(GL_FRAMEBUFFER, m_framebuffer));
        GL_CALL(glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_offscreen_render_texture, 0));

        if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
            GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
            std::cout << "[ERROR] Failed to make complete framebuffer object " << status << std::endl;
            return;
        }
    }

    void offscreen_render_target::prepare_post_processing_rendering()
    {
        if (m_offscreen_post_processuing_render_texture == 0) {
            generate_texture(m_offscreen_post_processuing_render_texture);
        }
        GL_CALL(glBindFramebuffer(GL_FRAMEBUFFER, m_post_processing_framebuffer));
        GL_CALL(glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_offscreen_post_processuing_render_texture, 0));

        if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
            GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
            std::cout << "[ERROR] Failed to make complete post processing framebuffer object " << status << std::endl;
            return;
        }

        GL_CALL(glViewport(0, 0, GLsizei(m_width), GLsizei(m_height)));

        GL_CALL(glActiveTexture(GLenum(GL_TEXTURE0)));
        GL_CALL(glBindTexture(GL_TEXTURE_2D, m_offscreen_render_texture));
    }

    void offscreen_render_target::orient_image(camera_orientation orientation, bool is_y_flip)
    {
        GL_CALL(glFlush());

        if (orientation == camera_orientation::deg_0 && !is_y_flip) {
            return;
        }

        if (m_program == nullptr) {
            std::cout << "[ERROR] Not initialization m_program" << std::endl;
            return;
        }
        if (m_frame_surface_handler == nullptr) {
            std::cout << "[ERROR] Not initialization m_frame_surface_handler" << std::endl;
            return;
        }

        prepare_post_processing_rendering();
        m_program->use();
        m_frame_surface_handler->set_orientation(orientation);
        m_frame_surface_handler->set_y_flip(is_y_flip);
        // Call once for perf
        m_frame_surface_handler->update_vertices_buffer();
        m_frame_surface_handler->draw();
        m_program->unuse();
        glFlush();
    }

    data_t offscreen_render_target::read_current_buffer()
    {
        size_t size = m_width * m_height * 4;
        data_t data = data_t{ std::make_unique<uint8_t[]>(size), size };

        GL_CALL(glReadPixels(0, 0, m_width, m_height, GL_RGBA, GL_UNSIGNED_BYTE, data.data.get()));
        GL_CALL(glBindFramebuffer(GL_FRAMEBUFFER, 0));

        return data;
    }
} // bnb