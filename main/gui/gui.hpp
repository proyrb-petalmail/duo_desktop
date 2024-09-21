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
        typedef struct coord
        {
            int x;
            int y;
        } coord;

        class app
        {
        private:
            lv_obj_t *object = nullptr;
            gui::coord coord;
            std::string icon;
            configor::json::value *data = nullptr;

        public:
            app();
            ~app();
            void set_object(lv_obj_t *const object);
            lv_obj_t *get_object();
            void set_coord(const gui::coord &coord);
            gui::coord &get_coord();
            void set_icon(const std::string &coord);
            std::string &get_icon();
            void set_data(configor::json::value *const data);
            configor::json::value *get_data();
        };

        static gui *unique;
        lv_display_t *display = nullptr;
        lv_indev_t *indev = nullptr;
        context *context_pointer = nullptr;
        lv_obj_t *screen = nullptr;
        lv_obj_t *background = nullptr;
        app *app = nullptr;
        gui();
        ~gui();
        void get_coord(coord &target, const char identity);
        static void animation_complete_callback(lv_anim_t *const animation);
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
