#include "gui.hpp"
#include "debug.hpp"
#include "error.hpp"
#include "main.hpp"

#include <unistd.h>

using namespace std;
using namespace configor;

#define LV_OBJ_FLAG_ALL 0b11111111111111111111
#define App_Size        100
#define App_Radius      App_Size * 0.618 * 0.618

namespace desktop
{
    /**
     * @brief the coord of object of lvgl.
     * @param x the x coord.
     * @param y the y coord.
     * @version 1.0
     * @date 2024/9/19
     * @author ProYRB
     */
    struct coord
    {
        int x;
        int y;
    };

    gui *gui::unique = nullptr;

    gui::gui() { Debug_Log("the unique of gui generation"); }

    gui::~gui() {}

    void gui::get_coord(coord &target, const char identity)
    {
        const int gui_row = this->context_pointer->get_gui_value("row").get<int>();
        const int gui_column = this->context_pointer->get_gui_value("column").get<int>();

        const int row_value = (identity / gui_column) + 1;
        const int column_value = (identity % gui_column) + 1;

        const float column_pad = (float)this->display->hor_res / (gui_column + 1);
        const float row_pad = (float)this->display->ver_res / (gui_row + 1);

        const float column_middle = (float)(gui_column + 1) / 2;
        target.x = (column_value - column_middle) * column_pad;

        const float row_middle = (float)(gui_row + 1) / 2;
        target.y = (row_value - row_middle) * row_pad;
    }

    void gui::short_click_event_callback(lv_event_t *const event)
    {
        Debug_Notice("short click event");
        lv_obj_t *const event_trigger = (lv_obj_t *)lv_event_get_target(event);
        json::value *const data = ((json::value *)lv_event_get_user_data(event));
        context *const context = context::get_unique();
        Debug_Log("data:" << (*data)["name"].get<string>());
        context->replace_pipe_value("path", (*data)["path"].get<string>());
        context->replace_pipe_value("argument", (*data)["argument"].get<string>());
        exit(0);
    }

    gui *gui::get_unique()
    {
        if (nullptr != gui::unique)
        {
            Debug_Log("the unique of gui exists one");
            return gui::unique;
        }
        return (gui::unique = new gui());
    }

    void gui::initialize(const string &fbdev, const string &evdev)
    {
        lv_init();
        Debug_Log("initialize lvgl successfully");

        try
        {
            this->display = lv_linux_fbdev_create();
            if (nullptr == display)
            {
                Debug_Error("failed to create display from fbdev");
                throw error(error::error_enum::Create);
            }
            lv_linux_fbdev_set_file(display, fbdev.data());
            lv_display_set_rotation(display, LV_DISPLAY_ROTATION_0);
            Debug_Log("create display successfully");

            this->indev = lv_evdev_create(LV_INDEV_TYPE_POINTER, evdev.data());
            if (nullptr == this->indev)
            {
                Debug_Error("failed to create indev from evdev");
                throw error(error::error_enum::Create);
            }
            Debug_Log("create indev successfully");
        }
        catch (error &error)
        {
            exit(error.get_type()); /* exit with error code */
        }

        this->context_pointer = context::get_unique();

        /* deploy background */
        this->screen = lv_screen_active();
        lv_obj_remove_flag(this->screen, (lv_obj_flag_t)LV_OBJ_FLAG_ALL);
        this->background = lv_obj_create(this->screen);
        lv_obj_remove_style_all(this->background);
        lv_obj_set_size(this->background, this->display->hor_res, this->display->ver_res);
        lv_obj_set_style_bg_image_src(this->background, "assets/background.png",
                                      LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_remove_flag(this->background, (lv_obj_flag_t)LV_OBJ_FLAG_ALL);
        lv_obj_update_layout(this->screen);
        Debug_Log("deploy background");

        /* deploy app */
        json::value &list = this->context_pointer->get_gui_value("list");
        this->app = new lv_obj_t *[list.size()];
        coord temporary_coord = {0, 0};
        char identity = 0;
        for (auto &element : list)
        {
            Debug_Log(json::wrap(element));
            this->app[identity] = lv_button_create(this->background);
            get_coord(temporary_coord, identity);
            lv_obj_set_size(this->app[identity], App_Size, App_Size);
            lv_obj_align(this->app[identity], LV_ALIGN_CENTER, temporary_coord.x,
                         temporary_coord.y);
            lv_obj_set_style_bg_color(this->app[identity], lv_color_hex(0xFFFFFF),
                                      LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_radius(this->app[identity], App_Radius,
                                    LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_pad_all(this->app[identity], 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_margin_all(this->app[identity], 30, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_remove_flag(this->app[identity], (lv_obj_flag_t)LV_OBJ_FLAG_ALL);
            lv_obj_add_flag(this->app[identity],
                            (lv_obj_flag_t)(LV_OBJ_FLAG_CLICKABLE | LV_OBJ_FLAG_OVERFLOW_VISIBLE));
            lv_obj_add_event_cb(this->app[identity], short_click_event_callback,
                                LV_EVENT_SHORT_CLICKED, &element);
            lv_obj_update_layout(this->screen);

            lv_obj_t *const label = lv_label_create(this->background);
            lv_obj_align(label, LV_ALIGN_CENTER, temporary_coord.x,
                         temporary_coord.y + (App_Size / 2) + 20);
            lv_label_set_text(label, element["name"].get<string>().data());
            lv_obj_set_style_text_color(label, lv_color_hex(0x000000),
                                        LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_text_font(label, &lv_font_custom_24, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_text_letter_space(label, 1, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_remove_flag(label, (lv_obj_flag_t)LV_OBJ_FLAG_ALL);
            lv_obj_update_layout(this->screen);

            ++identity;
            Debug_Log("deploy " << element["name"].get<string>());
        }

        Debug_Notice("initialize gui successfully");
    }

    void gui::execute()
    {
        Debug_Notice("execute gui successfully");
        while (true)
        {
            usleep(lv_timer_handler()); /* handle lvgl */
        }
    }
} // namespace desktop