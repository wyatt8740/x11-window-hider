#! /bin/bash
# unhides the last hidden window ID (the bottom line in ~/hidden).
# Requires xdotool and bash to be available. Also, I think this requires
# GNU sed (for sed -i), but have not tried it on other Unixes. if it doesn't
# work, use the C program (unhide.c). The C programs will work properly with
# the ~/hidden file created by the shell scripts, and the shell scripts will
# work properly with the ~/hidden file created by the C programs.

# awk - get the last line in the file $HOME/hidden and set it to the variable
# WINDOWID.
export WINDOWID=`awk '/./{line=$0} END{print line}' $HOME/hidden`
# if the file wasn't empty, unhide (map) the window, and remove the bottom line
# from $HOME/hidden. Unset WINDOWID (probably unnecessary).
if [[ -n "$WINDOWID" ]]; then
  xdotool windowmap $WINDOWID
  sed -i '/'${WINDOWID}'/d' $HOME/hidden
  export WINDOWID=
else
  echo "No hidden (unmapped) windows found in '$HOME/hidden'."
fi
