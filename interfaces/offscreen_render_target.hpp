#pragma once

#include <bnb/types/base_types.hpp>

namespace bnb::interfaces
{
    class offscreen_render_target
    {
    public:
        virtual ~offscreen_render_target() = default;

        /**
         * Offscreen Render Target initialization. Includes initialization of gl context,
         * buffers, support objects.
         * 
         * Example init()
         */
        virtual void init() = 0;

        /**
         * Activate context for current thread
         * 
         * Example activate_context()
         */
        virtual void activate_context() = 0;

        /**
         * Preparing texture for effect_player
         * 
         * Example prepare_rendering()
         */
        virtual void prepare_rendering() = 0;


        /**
         * Orientates the image
         * 
         * @param orientation orientation
         * @param is_y_flip set flip by y
         * 
         * Example prepare_rendering()
         */
        virtual void orient_image(bnb::camera_orientation orientation, bool is_y_flip) = 0;

        /**
         * Reading current buffer of active texture
         * 
         * @return a data_t with bytes of the processed frame 
         * 
         * Example read_current_buffer()
         */
        virtual bnb::data_t read_current_buffer() = 0;
    };
} // bnb::interfaces