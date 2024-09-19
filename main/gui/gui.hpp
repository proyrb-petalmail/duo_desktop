#pragma once

#include "lvgl.h"
#include "main.hpp"

#include <string>

namespace desktop
{
    class gui
    {
    private:
        static gui *unique;
        lv_obj_t *screen = nullptr;
        lv_obj_t *button = nullptr;
        gui();
        ~gui();
        static void short_click_event_callback(lv_event_t *const event);

    public:
        /**
         * @brief get the unique gui.
         * @return return the unique gui pointer.
         * @version 1.0
         * @date 2024/9/19
         * @author ProYRB
         */
        static gui *get_unique();
        void initialize(const std::string &fbdev, const std::string &evdev);
        void execute();
    };
} // namespace desktop
