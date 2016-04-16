#! /bin/sh
# hide the currently focused window and write the window's ID to the
# bottom of the file ~/hidden. requires xdotool to be available.
xdotool windowunmap `xdotool getwindowfocus | tee -a $HOME/hidden`
