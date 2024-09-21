#include "gui.hpp"
#include "debug.hpp"
#include "error.hpp"
#include "main.hpp"

#include <unistd.h>

using namespace std;
using namespace configor;

#define LV_OBJ_FLAG_ALL 0b11111111111111111111
#define LV_STATE_OPEN   LV_STATE_USER_1
#define App_Size        100
#define App_Radius      App_Size * 0.618 * 0.618

namespace desktop
{
    static void timer_callback(lv_timer_t *timer)
    {
        lv_obj_send_event((lv_obj_t *)lv_timer_get_user_data(timer), LV_EVENT_SHORT_CLICKED,
                          nullptr);
    }

    gui::app::app() {}

    void gui::app::set_object(lv_obj_t *const object) { this->object = object; }

    lv_obj_t *gui::app::get_object() { return this->object; }

    void gui::app::set_coord(const gui::coord &coord) { this->coord = coord; }

    gui::coord &gui::app::get_coord() { return this->coord; }

    void gui::app::set_icon(const std::string &icon) { this->icon = icon; }

    std::string &gui::app::get_icon() { return this->icon; }

    void gui::app::set_data(configor::json::value *const data) { this->data = data; }

    configor::json::value *gui::app::get_data() { return this->data; }

    gui::app::~app() {}

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

    void gui::animation_complete_callback(lv_anim_t *const animation)
    {
        Debug_Log("animation complete");
        class app *data = ((class app *)lv_anim_get_user_data(animation));
        context *const context = context::get_unique();
        json::value *const json_data = data->get_data();
        context->replace_pipe_value("path", (*json_data)["path"].get<string>());
        context->replace_pipe_value("argument", (*json_data)["argument"].get<string>());
        Debug_Warn("exit");
        exit(0);
    }

    void gui::short_click_event_callback(lv_event_t *const event)
    {
        lv_obj_t *const event_trigger = (lv_obj_t *)lv_event_get_target(event);
        class app *data = ((class app *)lv_event_get_user_data(event));
        lv_obj_move_foreground(event_trigger);

        if (lv_obj_has_state(event_trigger, LV_STATE_OPEN))
        {
            lv_obj_remove_state(event_trigger, LV_STATE_OPEN);

            lv_anim_t animation;
            lv_anim_init(&animation);
            lv_anim_set_var(&animation, event_trigger);
            lv_anim_set_duration(&animation, 500);

            lv_anim_set_path_cb(&animation, lv_anim_path_ease_out);
            lv_anim_set_exec_cb(&animation, (lv_anim_exec_xcb_t)lv_obj_set_x);
            lv_anim_set_values(&animation, lv_obj_get_x_aligned(event_trigger),
                               data->get_coord().x);
            lv_anim_start(&animation);

            lv_anim_set_exec_cb(&animation, (lv_anim_exec_xcb_t)lv_obj_set_y);
            lv_anim_set_values(&animation, lv_obj_get_y_aligned(event_trigger),
                               data->get_coord().y);
            lv_anim_start(&animation);

            lv_anim_set_exec_cb(&animation, (lv_anim_exec_xcb_t)lv_obj_set_width);
            lv_anim_set_values(&animation, lv_obj_get_width(event_trigger), App_Size);
            lv_anim_start(&animation);

            lv_anim_set_exec_cb(&animation, (lv_anim_exec_xcb_t)lv_obj_set_height);
            lv_anim_set_values(&animation, lv_obj_get_height(event_trigger), App_Size);
            lv_anim_start(&animation);
        }
        else
        {
            lv_obj_add_state(event_trigger, LV_STATE_OPEN);

            lv_anim_t animation;
            lv_anim_init(&animation);
            lv_anim_set_var(&animation, event_trigger);
            lv_anim_set_duration(&animation, 300);
            lv_anim_set_user_data(&animation, data);

            lv_anim_set_path_cb(&animation, lv_anim_path_ease_out);
            lv_anim_set_exec_cb(&animation, (lv_anim_exec_xcb_t)lv_obj_set_x);
            lv_anim_set_values(&animation, lv_obj_get_x_aligned(event_trigger), 0);
            lv_anim_start(&animation);

            lv_anim_set_exec_cb(&animation, (lv_anim_exec_xcb_t)lv_obj_set_y);
            lv_anim_set_values(&animation, lv_obj_get_y_aligned(event_trigger), 0);
            lv_anim_start(&animation);

            lv_anim_set_exec_cb(&animation, (lv_anim_exec_xcb_t)lv_obj_set_width);
            lv_anim_set_values(&animation, lv_obj_get_width(event_trigger),
                               lv_obj_get_width(lv_obj_get_parent(event_trigger)));
            lv_anim_start(&animation);

            lv_anim_set_exec_cb(&animation, (lv_anim_exec_xcb_t)lv_obj_set_height);
            lv_anim_set_values(&animation, lv_obj_get_height(event_trigger),
                               lv_obj_get_height(lv_obj_get_parent(event_trigger)));
            lv_anim_set_completed_cb(&animation, animation_complete_callback);
            lv_anim_start(&animation);
        }

        Debug_Log("click " << (*(data->get_data()))["name"].get<string>());
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
        this->app = new class app[list.size()];
        char identity = 0;
        for (auto &element : list)
        {
            Debug_Log(json::wrap(element));
            lv_obj_t *tmp_app = nullptr;
            this->app[identity].set_object(tmp_app = lv_button_create(this->background));
            coord temporary_coord = {0, 0};
            get_coord(temporary_coord, identity);
            this->app[identity].set_coord(temporary_coord);
            this->app[identity].set_icon(element["icon"].get<string>());
            this->app[identity].set_data(&element);

            if (0 == context::get_unique()->get_pipe_value("path").get<string>().compare(
                         element["path"].get<string>()))
            {
                lv_obj_set_size(tmp_app, this->display->hor_res, this->display->ver_res);
                lv_obj_align(tmp_app, LV_ALIGN_CENTER, 0, 0);
                lv_obj_add_state(tmp_app, LV_STATE_OPEN);

                lv_timer_t *const timer = lv_timer_create(timer_callback, 0, tmp_app);
                lv_timer_set_repeat_count(timer, 1);
                lv_timer_ready(timer);
            }
            else
            {
                lv_obj_set_size(tmp_app, App_Size, App_Size);
                lv_obj_align(tmp_app, LV_ALIGN_CENTER, temporary_coord.x, temporary_coord.y);
            }
            lv_obj_set_style_bg_color(tmp_app, lv_color_hex(0xFFFFFF),
                                      LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_bg_image_src(tmp_app, this->app[identity].get_icon().data(),
                                          LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_radius(tmp_app, App_Radius, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_pad_all(tmp_app, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_remove_flag(tmp_app, (lv_obj_flag_t)LV_OBJ_FLAG_ALL);
            lv_obj_add_flag(tmp_app,
                            (lv_obj_flag_t)(LV_OBJ_FLAG_CLICKABLE | LV_OBJ_FLAG_OVERFLOW_VISIBLE));
            lv_obj_add_event_cb(tmp_app, short_click_event_callback, LV_EVENT_SHORT_CLICKED,
                                &(this->app[identity]));
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