# x11-window-hider
Easily hide and show (unmap and map) X11 windows.

I wrote the shell scripts first, using xdotool as my interface with the
X server. Later, because I was having a hard time finding C projects that
hadn't been done yet, I 'ported' the shell scripts into K&R C.

I recommend binding these programs to keyboard shortcuts, as they operate on
whichever window currently has focus.

## The Shell Scripts:
These are what you should use if you have `xdotool` and `bash` and GNU `sed`.
The hiding script doesn't require any of those but `xdotool`, but the unhiding
script requires `bash` and GNU `sed`. You can probably replace those with other
tools or options in exotic linuxes/unixes, but I was targeting my personal
Debian machine.


## C programs:
If you want something that will work on basically any POSIX OS, try the C
programs. You only need libxcb's headers and library for them to work.

(Update: 2017: for some reason, the C programs don't work on my 64 bitlaptop
with Intel graphics. I'm probably doing something wrong with the X11 protocol
or making bad assumptions. It was an early C program I wrote, just to try to
do it, so I'm not too concerned with fixing it right now.

The shell scripts still work fine, so I recommend using those. I have no
idea exactly why the C programs broke, and I don't have time to mess with them
at the moment.)

The C Programs:
I chose K&R C because I happened to have a book written in 1989 on C
handy. The book is called "Programming in C: Revised Edition" By
Stephen Kochan. It covers K&R C for most of the book, and then has
an appendix of the then-recent ANSI C standard's differences. I
believe I used only K&R C in this program, but I wrote it months ago
so I may have forgotten something. Anyway, K&R is what I learned C
in.

It should compile on basically any C compiler as long as you
have a POSIX system. By replacing the check for a file's existance
with `access()` and the code to find the path of the user's home directory
(which uses `getpwuid()`), you can probably even make this code compile in
Windows for use with XMing (a Win32 X11 server). I tried to avoid any
external libraries where possible, so it is highly portable.

To compile, run `./compile.sh` from the root of the repository.
