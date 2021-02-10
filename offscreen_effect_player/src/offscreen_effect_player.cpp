#include "offscreen_effect_player.hpp"
#include "offscreen_render_target.hpp"

#include <iostream>

namespace bnb
{
    std::shared_ptr<interfaces::offscreen_effect_player> offscreen_effect_player::create(
        const std::vector<std::string>& path_to_resources, const std::string& client_token,
        int32_t width, int32_t height, bool manual_audio, std::optional<ort_sptr> ort)
    {
        if (!ort.has_value()) {
            ort = std::make_shared<offscreen_render_target>(width, height);
        }

        return oep_ptr(new offscreen_effect_player(
                path_to_resources, client_token, width, height, manual_audio, *ort));
    }

    offscreen_effect_player::offscreen_effect_player(
        const std::vector<std::string>& path_to_resources, const std::string& client_token,
        int32_t width, int32_t height, bool manual_audio,
        ort_sptr offscreen_render_target)
            : m_utility(path_to_resources, client_token)
            , m_ep(bnb::interfaces::effect_player::create( {
                width, height,
                bnb::interfaces::nn_mode::automatically,
                bnb::interfaces::face_search_mode::good,
                false, manual_audio }))
            , m_ort(offscreen_render_target)
            , m_render_thread([this]() { while (!m_cancellation_flag) { m_scheduler.run_all_tasks(); } })
            , m_cancellation_flag(false)
    {
        auto task = [this, width, height]() {
            m_ort->init();
            m_ep->surface_created(width, height);
        };

        async::spawn(m_scheduler, task);
    }

    offscreen_effect_player::~offscreen_effect_player()
    {
        m_cancellation_flag = true;
    }

    void offscreen_effect_player::process_image_async(std::shared_ptr<full_image_t> image, pb_callback callback)
    {
        if (current_frame == nullptr) {
            current_frame = std::make_shared<pixel_buffer>(shared_from_this(),
                image->get_format().width, image->get_format().height);
        }

        if (current_frame.use_count() > 1) {
            std::cout << "[Warning] The interface for processing the previous frame is not destroyed" << std::endl;
            return;
        }

        auto task = [this, image, callback]() {
            m_ort->prepare_rendering();
            m_ep->push_frame(std::move(*image));
            while (m_ep->draw() < 0) {
                std::this_thread::yield();
            }
            m_ort->orient_image(camera_orientation::deg_0, true);
            callback(current_frame);
        };

        async::spawn(m_scheduler, task);
    }

    void offscreen_effect_player::load_effect(const std::string& effect_path)
    {
        auto task = [this, &effect_path]() {
            m_ep->effect_manager()->load(effect_path);
        };

        async::spawn(m_scheduler, task);
    }

    void offscreen_effect_player::unload_effect()
    {
        load_effect("");
    }

    void offscreen_effect_player::pause()
    {
        m_ep->playback_pause();
    }

    void offscreen_effect_player::resume()
    {
        m_ep->playback_play();
    }

    void offscreen_effect_player::enable_audio(bool enable)
    {
        m_ep->enable_audio(enable);
    }

    void offscreen_effect_player::call_js_method(const std::string& method, const std::string& param)
    {
        if (auto e_manager = m_ep->effect_manager()) {
            if (auto effect = e_manager->current()) {
                effect->call_js_method(method, param);
            } else {
                std::cout << "[Error] effect not loaded" << std::endl;
            }
        } else {
            std::cout << "[Error] effect manager not initialized" << std::endl;
        }
    }

    void offscreen_effect_player::read_current_buffer(std::function<void(bnb::data_t data)> callback)
    {
        std::weak_ptr<offscreen_effect_player> this_ = shared_from_this();
        auto task = [this_, callback]() {
            if (auto this_sp = this_.lock()) {
                callback(this_sp->m_ort->read_current_buffer());
            }
        };

        async::spawn(m_scheduler, task);
    }

} // bnb