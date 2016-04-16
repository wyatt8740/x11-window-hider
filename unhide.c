/* unhide.c:                                                                *
 * -------------------------------------------------------------------------*
 * Shows the last hidden X11 window (the last window ID in `~/hidden`).     *
 * Removes the last window IDi n a 'stack' file in the user's home          *
 * directory. This is mostly a 'security through obscurity' thing - it      *
 * (currently) provides no encryption, but anyone who looks in the file will*
 * will only see a lot of numbers. Most people are not likely, unless       *
 * they've looked at this program (or the bash script I wrote ages ago that *
 * this descends from), to recognize that they are in fact X11 window ID's. *
 * Even with this knowledge, linux/unix systems most often don't come with  *
 * anything like 'xdotool' that can re-map unmapped windows (So, basically, *
 * I guess you could use it as a 'boss key' with a keyboard shortcut).      *
 *                                                                          *
 * The 'hidden window' file is ~/hidden by default. You can edit that       *
 * filename in the source code below. Feel free to do a pull request to add *
 * functionality or fix bugs. The unhider is a bit more dangerous than the  *
 * hider program in that it 'dumbly' truncates the last 9 bytes off the     *
 * ~/hidden file (newline + 8 character ID). This SHOULD be safe, but I     *
 * cannot speak for all X11 implementations using 8 digit ID's. Suffice to  *
 * say, the bash version is probably safer to use, but it requires xdotool. *
 * Use this if you don't have bash or don't have xdotool.                   *
 *                                                                          *
 * Compiling requires libxcb and its development headers.                   *
 * on Debian/Ubuntu, this should install them:                              *
 *   sudo apt-get install libxcb1-dev                                       *
 *                                                                          *
 * To compile, I usually do:                                                *
 *   gcc -Wall unhide.c -o unhide -lxcb                                     *
 *                                                                          *
 * Questions? Comments? Concerns? If you contact me, I will help you.       *
 *--------------------------------------------------------------------------*/
/*#define DEBUG*/
/* remove the DEBUG definition to disable debug prints and stuff */
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <pwd.h>
#include <string.h>
#include <xcb/xcb.h>
#include <xcb/xproto.h>

#include <unistd.h> /* POSIX */

/* set filename here. To change the path to something above your home dir,
 * you'll have to go down a ways in the source to where I look for the HOME
 * environment variable. THE FILENAME HERE MUST BEGIN WITH A SLASH OR YOU
 * COULD GET STRANGE ERRORS. */
char windowListFileName[]="/hidden";
xcb_connection_t        *connection;

/* elevate_window() currently unused. Maybe later. */
/* void elevate_window(xcb_window_t window)
{
	uint32_t stack_mode[1] = { XCB_STACK_MODE_ABOVE };
	xcb_configure_window(connection, window, XCB_CONFIG_WINDOW_STACK_MODE, stack_mode);
} */


int main(int argc, char *argv[]){
	/* connect to X server */
	connection = xcb_connect (NULL,NULL);

	/* find path to file: */
	const char *homedir;
        if ((homedir = getenv("HOME")) == NULL) {
                homedir = getpwuid(getuid())->pw_dir;
        }
	#ifdef DEBUG
        printf("homedir: %s\narray size:%d\n",homedir,(int)strlen(homedir));
	#endif

	/* find total size of string*/
	char path[(int)strlen(homedir) + strlen(windowListFileName) + 1];

	/* concatenate the string in space provided above */
	sprintf(path,"%s%s",homedir,windowListFileName);

	/* if file doesn't exist */
        if( access( path, F_OK ) == -1 ) {
                /* file doesn't exist, so try to make a blank one. We don't want to
                 * erase existing files though, so only do it if it doesn't exist */
                printf("You don't have a list of hidden files. Run the hider first.\n");
		return -1;
        }
	/* basing this entirely on the assumption all window ID's are 8 characters long and every line
	 * has a newline after it. Totally safe. Yeah. Definitely. It cuts off the last 9 bytes of
	 * the file with truncate().
/* if it exists, time to append data to it! re-opening time. */
/* get ready for the file! */
	FILE *windowListFile;
 /* doesn't create a file if none exists, so checked*/
/* C89 (ANSI C) and K&R C require that you set your position in the file
 * after using fopen(). So I'm seeking to end of file */
        struct stat st;
        stat (path, &st);
        off_t FileSize = st.st_size;

	static const int max_len = 9 + 1; /* max length of line to read plus 1. */
	char buff[max_len + 1];
	xcb_window_t windowID;
if (FileSize > 9){ /* 8 byte window ID's plus a newline is 9 bytes */

	if ((windowListFile = fopen(path, "r")) != NULL)  { /* open file */
		fseek(windowListFile, -max_len, SEEK_END); /* EOF minus length needed */
		fread(buff, max_len-1, 1, windowListFile); /* read forward from close to the end */
		fclose(windowListFile); /* close file */
		buff[max_len-1] = '\0'; /* null termination is very important for a boy's health */
		char *last_newline = strrchr(buff, '\n'); /* find last newline */
		char *last_line = last_newline+1; /* go to that line */
#ifdef DEBUG
		printf("[%s]\n", last_line); /* captured */
#endif
		windowID=atoi(last_line);
		truncate(path, FileSize - 9);
	}
}
else if (FileSize > 1) { /* if file is only one line do this special case */
	windowListFile = fopen(path,"r");
	char line1[9];
	fread(line1,8,1,windowListFile);
	fclose(windowListFile);
	line1[8]='\0';
#ifdef DEBUG
	printf("only one line, contents %s\n",line1);
#endif
	windowID=atoi(line1);
	truncate(path, 0); /* wipe file clean */
}
else { /* if the file doesn't have any window ID's,wipe any straggler bytes and quit without doing anything.
	* Straggler bytes should only result from manual editing of the file, of course. But I'll admit my
	* math's been pretty shitty for the 'unhide' part of the project. */
	#ifdef DEBUG
		printf("no window ID's in the file, quitting\n");
	#endif
/*	truncate(path, 0); */ /*unnecessary because if it's less than 1 byte it must be 0 */
	return 0;
}

	/* Last window ID found, now time to unhide */
	xcb_map_window(connection, windowID);
	xcb_flush(connection); /*NEVER FORGET TO FLUSH */

	/* disconnect when done */
	xcb_disconnect (connection);
	/* Quit */
	return 0;
}
