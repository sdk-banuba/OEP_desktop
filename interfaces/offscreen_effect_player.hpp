#pragma once

#include <bnb/types/full_image.hpp>

#include "pixel_buffer.hpp"

using pb_sptr = std::shared_ptr<bnb::interfaces::pixel_buffer>;
using pb_wptr = std::shared_ptr<bnb::interfaces::pixel_buffer>;
namespace bnb {

    using oep_pb_ready_cb = std::function<void(std::optional<pb_wptr>)>;

namespace interfaces
{
    struct orient_format
    {
        bnb::camera_orientation orientation;
        bool is_y_flip;
    };

    class offscreen_effect_player
    {
    public:
        virtual ~offscreen_effect_player() = default;

        /**
         * An asynchronous method for passing a frame to effect player,
         * and calling callback as a frame will be processed
         * 
         * @param image full_image_t - containing a frame for processing 
         * @param callback calling when frame will be processed, containing pointer of pixel_buffer for get bytes
         * @param target_orient 
         * 
         * Example process_image_async(image_sptr, [](pb_wptr pb_sptr){})
         */
        virtual void process_image_async(std::shared_ptr<full_image_t> image, oep_pb_ready_cb callback,
                                         std::optional<orient_format> target_orient) = 0;

        /**
         * Load and activate effect async. May be called from any thread
         * 
         * @param effect_path Path to directory of effect
         * 
         * Example load_effect("effects/test_BG")
         */
        virtual void load_effect(const std::string& effect_path) = 0;

        /**
         * Unload effect from cache.
         * 
         * Example unload_effect()
         */
        virtual void unload_effect() = 0;

        /**
         * Pause in effect player
         * 
         * Example pause()
         */
        virtual void pause() = 0;

        /**
         * Resume in effect player
         * 
         * Example resume()
         */
        virtual void resume() = 0;

        /**
         * Enable or disable audio for effect
         * 
         * @param enable parameter defining whether to enable or disable audio in effect
         * 
         * Example enable_audio(false)
         */
        virtual void enable_audio(bool enable) = 0;

        /**
         * Call js method defined in config.js file of active effect
         * 
         * @param method JS function name. Member functions are not supported.
         * @param param function arguments as JSON string.
         * 
         * Example call_js_method("just_bg", "{ "recordDuration": 15, "rotation_vector": true }")
         */
        virtual void call_js_method(const std::string& method, const std::string& param) = 0;
    };
}
} // bnb::interfaces