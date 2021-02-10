#include "offscreen_effect_player.hpp"
#include "offscreen_render_target.hpp"

#include "glfw_window.hpp"
#include "render_thread.hpp"

#include <bnb/spal/camera/ocv_based.hpp>

#define BNB_CLIENT_TOKEN <#Place your token here#>

int main()
{
    int32_t width = 1280;
    int32_t height = 720;

    glfwInit();

    std::optional<std::shared_ptr<bnb::interfaces::offscreen_render_target>> ort;
    ort = std::make_shared<bnb::offscreen_render_target>(width, height);

    auto oep = bnb::offscreen_effect_player::create({ BNB_RESOURCES_FOLDER }, BNB_CLIENT_TOKEN,
                                               width, height, false, ort);
    oep->load_effect("effects/Afro");

    //We make glfw_window and render_thread only for show result of OEP
    std::shared_ptr<glfw_window> window = std::make_shared<glfw_window>("");
    std::shared_ptr<render::render_thread> render_t =
            std::make_shared<render::render_thread>(window->get_window(), width, height);

    auto ef_cb = [&oep, render_t](bnb::full_image_t image) {
        auto image_ptr = std::make_shared<bnb::full_image_t>(std::move(image));

        auto get_pixel_buffer_callback = [image_ptr, render_t](std::shared_ptr<bnb::interfaces::pixel_buffer> pb) {
            auto render_callback = [render_t](bnb::full_image_t image) {
                render_t->update_data(std::move(image));
                render_t->schedule([render_t]() mutable {
                    render_t->update_context();
                });
            };
            pb->get_NV12(render_callback);
        };

        oep->process_image_async(image_ptr, get_pixel_buffer_callback);
    };

    std::shared_ptr<bnb::camera_base> m_camera_ptr = bnb::create_camera_device(ef_cb, 0);

    window->show(width, height);
    window->run_main_loop();

    return 0;
}