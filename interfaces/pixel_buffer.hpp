#pragma once

#include <bnb/types/full_image.hpp>

using oep_image_ready_cb = std::function<void(std::optional<bnb::full_image_t> image)>;

namespace bnb::interfaces
{
    class pixel_buffer
    {
    public:
        virtual ~pixel_buffer() = default;

        /**
         * Locks the pixel_buffer. Must be called before working with textures.
         * Locking ignores the use of the process_image_async in OEP.
         * 
         * Example lock()
         */
        virtual void lock() = 0;

        /**
         * Unlocks pixel_buffer. Must be called after working with textures.
         * 
         * Example unlock()
         */
        virtual void unlock() = 0;

        /**
         * Returns the locking state of pixel_buffer.
         * 
         * @return true if pixel_buffer locked else false
         * 
         * Example is_locked()
         */
        virtual bool is_locked() = 0;

        /**
         * In thread with active texture get pixel bytes from Offscreen_render_target and
         * convert to full_image_t.
         * 
         * @param callback calling with full_image_t. full_image_t keep RGBA
         * 
         * Example process_image_async([](std::optional<full_image_t> image){})
         */
        virtual void get_rgba(oep_image_ready_cb callback) = 0;

        /**
         * In thread with active texture get pixel bytes from Offscreen_render_target and
         * convert to full_image_t.
         * 
         * @param callback calling with full_image_t. full_image_t keep NV12
         * 
         * Example process_image_async([](std::optional<full_image_t> image){})
         */
        virtual void get_nv12(oep_image_ready_cb callback) = 0;
    };
} // bnb::interfaces

