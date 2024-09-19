#include "gui.hpp"
#include "debug.hpp"
#include "error.hpp"
#include "main.hpp"

// #include <fstream>
#include <unistd.h>

#define BACKGROUND_COLOR 0xEEEEEE
#define FOREGROUND_COLOR 0xFFFFFF

// #define CONTAINER_MARGIN       100
// #define BAR_HEIGHT             100
// #define BAR_RADIUS             BAR_HEIGHT / 3
// #define EXIT_BUTTON_SIZE       BAR_HEIGHT * 0.65
// #define EXIT_BUTTON_IMAGE_SIZE BAR_HEIGHT * 0.55
// #define BAR_PAD                (BAR_HEIGHT - EXIT_BUTTON_SIZE) / 2
// #define LINE_WIDTH             4
// #define PANEL_HEIGHT           60

using namespace std;

namespace desktop
{
    /**
     * @brief the user data in lvgl.
     * @param gui the gui pointer.
     * @version 1.0
     * @date 2024/9/19
     * @author ProYRB
     */
    typedef struct
    {
        gui *gui_pointer;
    } user_data;

    gui *gui::unique = nullptr;

    gui::gui() { Debug_Log("the unique of gui generation"); }

    gui::~gui() {}

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
            lv_display_t *const display = lv_linux_fbdev_create();
            Debug_Log("ok");
            if (nullptr == display)
            {
                Debug_Error("failed to create display from fbdev");
                throw error(error::error_enum::Create);
            }
            lv_linux_fbdev_set_file(display, fbdev.data());
            lv_display_set_rotation(display, LV_DISPLAY_ROTATION_0);
            Debug_Log("create display successfully");

            lv_indev_t *const indev_driver = lv_evdev_create(LV_INDEV_TYPE_POINTER, evdev.data());
            if (nullptr == indev_driver)
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

        /* deploy screen */
        this->screen = lv_screen_active();
        // lv_obj_remove_style_all(this->screen);
        lv_obj_set_style_bg_color(this->screen, lv_color_hex(BACKGROUND_COLOR),
                                  LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_remove_flag(this->screen, LV_OBJ_FLAG_SCROLLABLE);
        static user_data screen_event_data{this};
        lv_obj_add_event_cb(this->screen, short_click_event_callback, LV_EVENT_SHORT_CLICKED,
                            &screen_event_data);
        lv_obj_update_layout(this->screen);
        Debug_Log("deploy screen");

        this->button = lv_button_create(this->screen);
        lv_obj_set_size(this->button, 100, 100);
        lv_obj_center(this->button);
        static user_data button_event_data{this};
        lv_obj_add_event_cb(this->button, short_click_event_callback, LV_EVENT_SHORT_CLICKED,
                            &button_event_data);
        lv_obj_update_layout(this->screen);
        Debug_Log("deploy button");

        Debug_Notice("initialize gui successfully");
    }

    void gui::short_click_event_callback(lv_event_t *const event)
    {
        lv_obj_t *const event_trigger = (lv_obj_t *)lv_event_get_target(event);
        user_data *const data = ((user_data *)lv_event_get_user_data(event));
        Debug_Notice("short click event");
        if (event_trigger == data->gui_pointer->button)
        {
            // data->gui->pipe_json["app_relative_path"] = "./app";
            // LOG_MESSAGE(json::dump(data->gui->pipe_json)); /* output json content */
            // fstream file_stream("pipe.json");              /* open config json */
            // if (false == file_stream.is_open())
            // {
            //     LOG_COMMAND_RESULT(file_stream.rdstate()); /* output command result value */
            // }
            // file_stream << json::dump(data->gui->pipe_json, {json::serializer::with_indent(4, '
            // ')})
            //             << endl; /* read json file */
            // file_stream.close();
            exit(0);
        }
    }

    // void gui::timer_callback(lv_timer_t *const timer) { LOG_MESSAGE("timer timeout"); }

    void gui::execute()
    {
        while (true)
        {
            usleep(lv_timer_handler()); /* handle lvgl */
        }
    }
} // namespace desktop