#include "main.hpp"
#include "argument.hpp"
#include "cmdline.hpp"
#include "context.hpp"
#include "debug.hpp"
#include "error.hpp"

/* using namespace */
using namespace std;
using namespace cmdline;
using namespace desktop;

int main(const int argument_count, char **const argument_value)
{
    /* parse argument */
    parser argument_parser;
    argument_parser.add<string>(Argument_Fbdev, Argument_Fbdev_Short, Argument_Fbdev_Refer,
                                Argument_Fbdev_Need, Argument_Fbdev_Default); /* add fbdev */
    argument_parser.add<string>(Argument_Evdev, Argument_Evdev_Short, Argument_Evdev_Refer,
                                Argument_Evdev_Need, Argument_Evdev_Default); /* add evdev */
    argument_parser.add<string>(Argument_Pipe, Argument_Pipe_Short, Argument_Pipe_Refer,
                                Argument_Pipe_Need, Argument_Pipe_Default); /* add pipe */
    argument_parser.add<string>(Argument_Gui, Argument_Gui_Short, Argument_Gui_Refer,
                                Argument_Gui_Need, Argument_Gui_Default); /* add gui */
    argument_parser.parse_check(argument_count, argument_value);          /* execute checking */

    Debug_Log("--" Argument_Fbdev "=" << argument_parser.get<string>(Argument_Fbdev));
    Debug_Log("--" Argument_Evdev "=" << argument_parser.get<string>(Argument_Evdev));
    Debug_Log("--" Argument_Pipe "=" << argument_parser.get<string>(Argument_Pipe));
    Debug_Log("--" Argument_Gui "=" << argument_parser.get<string>(Argument_Gui));

    /* confirm that arguments are correct */
    try
    {
        string command_string("find /dev -wholename " +
                              argument_parser.get<string>(Argument_Fbdev)); /* generate command */
        int command_result = system(command_string.data());                 /* execute command */
        if (0 != command_result)
        {
            Debug_Error("failed to find fbdev");
            throw error(error::error_enum::Find);
        }
        Debug_Notice("find fbdev successfully");

        command_string.clear();
        command_string.append("find /dev -wholename " +
                              argument_parser.get<string>(Argument_Evdev)); /* generate command */
        command_result = system(command_string.data());                     /* execute command */
        if (0 != command_result)
        {
            Debug_Error("failed to find evdev");
            throw error(error::error_enum::Find);
        }
        Debug_Notice("find evdev successfully");

        context::get_unique()->load_pipe_json(
            argument_parser.get<string>(Argument_Pipe)); /* load pipe json file */
        context::get_unique()->load_gui_json(
            argument_parser.get<string>(Argument_Gui)); /* load gui json file */
    }
    catch (error &error)
    {
        exit(error.get_type()); /* exit with error code */
    }

    /* test */
    context::get_unique()->replace_pipe_value("pipe", "gogogo");
    context::get_unique()->replace_gui_value("gui", "hello");

    return 0;
}
