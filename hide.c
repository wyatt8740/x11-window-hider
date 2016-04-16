/* hide.c:                                                                  *
 * -------------------------------------------------------------------------*
 * hides currently focused X11 window, for later unhiding with 'unhide.c'.  *
 * Stores a list of window ID's, in order of hiding, in a 'stack' file in   *
 * the user's home directory. This is mostly a 'security through obscurity' *
 * thing - it provides no encryption, but anyone who looks in the file will *
 * only see a lot of numbers. Most people are not likely, unless they've    *
 * looked at this program (or the bash script I wrote ages ago that this    *
 * descends from), to recognize that they are in fact X11 window ID's. Even *
 * with this knowledge, linux/unix systems most often don't come with       *
 * anything like 'xdotool' that can re-map unmapped windows (So, basically, *
 * I guess you could use it as a 'boss key' with a keyboard shortcut).      *
 *                                                                          *
 * The 'hidden window' file is ~/hidden by default. You can edit that       *
 * filename in the source code below.                                       *
 *                                                                          *
 * Compiling requires libxcb and it's development headers to be installed.  *
 * on Debian/Ubuntu, this should install them:                              *
 *   sudo apt-get install libxcb1-dev                                       *
 *                                                                          *
 * To compile, I usually do:                                                *
 *   gcc -Wall hide.c -o hide -lxcb                                         *
 *                                                                          *
 * Questions? Comments? Concerns? If you can contact me, I will help you.   *
 *--------------------------------------------------------------------------*/
/*#define DEBUG*/
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <pwd.h>
#include <string.h>
#include <xcb/xcb.h>
#include <xcb/xproto.h>

/* POSIX needed for getpwuid. If somehow you don't have POSIX,
 * but do have X11, remove unistd.h and the getpwuid part below.
 * Also, if I forget to remove it, remember that sleep() below is POSIX.
 * Additionally, you'll have to do your own check for the file's existance
 * later (because access() is POSIX too, IIRC). */
#include <unistd.h>

/* set filename here. To change the path to something above your home dir,
 * you'll have to go down a ways in the source to where I look for the HOME
 * environment variable. THE FILENAME HERE MUST BEGIN WITH A SLASH OR YOU
 * COULD GET STRANGE ERRORS. */
char windowListFileName[]="/hidden";

int main(int argc, char *argv[]){
	xcb_connection_t	*connection;

	/* Open X Server connection; use the DISPLAY variable as
	 * default display name. First NULL means to use DISPLAY
	 * variable, second NULL means you don't care which screen
	 * it's on  */
	connection = xcb_connect (NULL,NULL);

	/* find input focus; this is the window to hide.
	 * Unfortunately, it doesn't return a window ID straight to me,
	 * so I have to use a struct (groan)*/
	xcb_flush(connection);

	/*try to get stuff out of that god forsaken cookie*/
	xcb_window_t windowID = xcb_get_input_focus_reply
			(
				connection,
				xcb_get_input_focus (connection),
				NULL
			)->focus; 	/* get member 'focus' from the struct.
					 * Struct is of the type xcb_window_t.*/

	const char *homedir;
        if ((homedir = getenv("HOME")) == NULL) {
                homedir = getpwuid(getuid())->pw_dir;
        }

/* This just prints the home dir and tells you how much space is allocated
 * for it; since it's null-terminated you will have to add one byte. */

#ifdef DEBUG
        printf("homedir: %s\narray size:%d\n",homedir,(int)strlen(homedir));
#endif

/* set path variable length to homedir + length of file name + null
 * termination (1). The file contains a list of hidden window ID's. */
	char path[(int)strlen(homedir) + strlen(windowListFileName) + 1];

/* now that path is set to the size needed (and not a byte more than needed
 * for null-termination Memory savings WOOOO), time to concatenate it all
 * together and put it into the space we reserved. sprintf automatically
 * null-terminates. (Can you tell I'm paranoid about that?) */
	sprintf(path,"%s%s",homedir,windowListFileName);

/* get ready for the file! */
	FILE *windowListFile;

/* if file doesn't exist */
	if( access( path, F_OK ) == -1 ) {
		/* file doesn't exist, so try to make a blank one. We don't want to
		 * erase existing files though, so only do it if it doesn't exist */
		creat(path,0666);
	}

	if( access( path, F_OK ) == -1 ) {
		/* huh? still doesn't exist? dafuq? time to bail */
		printf("Can't create window list file, check permissions.\n");
		return -1;
	}

/* if it exists, time to append data to it! re-opening time. */
	windowListFile = fopen(path,"r+"); /* doesn't create a file if none exists,
                                        * so checked first */

/* C89 (ANSI C) and K&R C require that you set your position in the file
 * after using fopen(). So I'm seeking next. */

/*          Go to end of file. I'm using this similarly to '>>' in sh or bash
 *      to concatenate to the end of a file.
 *          The original 'xdotool/bash' implementation I made of this program
 *      did use '>>', actually. */
	fseek(windowListFile, 0, 2); /* I learned fseek() syntax from an older book.
			      * 2 is equivalent to SEEK_END. (The book:
			      * Programming in C, Revised Edition, Stephen
			      * Kochan)*/

	/* Print window ID. Unnecessary when I finish coding. */
	/* This doesn't send it to the file, of course. */
	#ifdef DEBUG
	printf("%d\n",windowID);
	#endif

	/* the riskiest line of code in this entire project I think, because
	 * while it appears that all window ID's are 8 characters wide I'm
	 * not 100% sure and am assigning what I think is probably ample.
	 * if something fails, maybe check here.
	*/
	char writeline[24];
	sprintf(writeline,"%d\n",windowID);
	fputs(writeline,windowListFile);
	fclose(windowListFile);
	/* unmap window (hide it) */
	xcb_unmap_window (connection, windowID);
	/* Make sure all commands are sent to X server.
	 * Never forget to flush! */
	xcb_flush(connection);

	/* disconnect when done */
	xcb_disconnect (connection);
	/* Quit */
	return 0;
}

