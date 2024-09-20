#pragma once

#include "lvgl.h"
#include "main.hpp"

#include <string>

namespace desktop
{
    class gui
    {
    private:
        /**
         * @brief the coord of objest on screen.
         * @param x the x coord.
         * @param y the y coord.
         * @version 1.0
         * @date 2024/9/19
         * @author ProYRB
         */
        typedef struct coord coord;

        static gui *unique;
        lv_display_t *display = nullptr;
        lv_indev_t *indev = nullptr;
        context *context_pointer = nullptr;
        lv_obj_t *screen = nullptr;
        lv_obj_t *background = nullptr;
        lv_obj_t **app = nullptr;
        gui();
        ~gui();
        void get_coord(coord &target, const char identity);
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
