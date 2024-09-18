#pragma once

/* fbdev */
#define Argument_Fbdev         "fbdev"
#define Argument_Fbdev_Short   'f'
#define Argument_Fbdev_Refer   "specify fbdev"
#define Argument_Fbdev_Need    false
#define Argument_Fbdev_Default "/dev/fb0"

/* evdev */
#define Argument_Evdev         "evdev"
#define Argument_Evdev_Short   'e'
#define Argument_Evdev_Refer   "specify evdev"
#define Argument_Evdev_Need    false
#define Argument_Evdev_Default "/dev/input/event0"

/* pipe */
#define Argument_Pipe          "pipe"
#define Argument_Pipe_Short    'p'
#define Argument_Pipe_Refer    "specify pipe file"
#define Argument_Pipe_Need     false
#define Argument_Pipe_Default  "pipe.json"

/* gui */
#define Argument_Gui           "gui"
#define Argument_Gui_Short     'g'
#define Argument_Gui_Refer     "specify gui configuration file"
#define Argument_Gui_Need      false
#define Argument_Gui_Default   "gui.json"
