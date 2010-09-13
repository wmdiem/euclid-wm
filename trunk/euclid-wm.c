/*

Copyright (c) 2010, William M. Diem <wmdiem at gmail dot com>
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:
* Redistributions of source code must retain the above copyright
  notice, this list of conditions and the following disclaimer.
* Redistributions in binary form must reproduce the above copyright
  notice, this list of conditions and the following disclaimer in the
  documentation and/or other materials provided with the distribution.
* The names of the contributor(s) may not be used to endorse or promote products
  derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER(S) BE LIABLE FOR ANY
DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

scrotwm < http://www.scrotwm.org/ > and dwm < http://dwm.suckless.org/ > 
were used as a model for some parts.
Thus the one or more of the following notices may apply to some sections:

* scrotwm: 
* Copyright (c) 2009 Marco Peereboom <marco@peereboom.us>
* Copyright (c) 2009 Ryan McBride <mcbride@countersiege.com>
* Copyright (c) 2009 Darrin Chandler <dwchandler@stilyagin.com>
* Copyright (c) 2009 Pierre-Yves Ritschard <pyr@spootnik.org>
*
* Permission to use, copy, modify, and distribute this software for any
* purpose with or without fee is hereby granted, provided that the above
* copyright notice and this permission notice appear in all copies.
*
* THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
* WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
* MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
* ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
* WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
* ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
* OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
*
* dwm:
* 2006-2008 Anselm R Garbe <garbeam at gmail dot com>
* 2006-2007 Sander van Dijk <a dot h dot vandijk at gmail dot com>
* 2006-2007 Jukka Salmi <jukka at salmi dot ch>
* 2007 Premysl Hruby <dfenze at gmail dot com>
* 2007 Szabolcs Nagy <nszabolcs at gmail dot com>
* 2007 Christof Musik <christof at sendfax dot de>
* 2007-2008 Enno Gottox Boland <gottox at s01 dot de>
* 2007-2008 Peter Hartlich <sgkkr at hartlich dot com>
* 2008 Martin Hurton <martin dot hurton at gmail dot com>
*
* Permission is hereby granted, free of charge, to any person obtaining a
* copy of this software and associated documentation files (the "Software"),
* to deal in the Software without restriction, including without limitation
* the rights to use, copy, modify, merge, publish, distribute, sublicense,
* and/or sell copies of the Software, and to permit persons to whom the
* Software is furnished to do so, subject to the following conditions:
*
* The above copyright notice and this permission notice shall be included in
* all copies or substantial portions of the Software.
*
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL
* THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
* LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
* FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
* DEALINGS IN THE SOFTWARE.

*/
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdbool.h>
#include <string.h>
#include <sys/time.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xatom.h>
#include <errno.h>
#include <X11/extensions/Xinerama.h>

#define BINDINGS 64 
/*BASIC VARIABLE TYPES*/

/*
 * Overall structure
 * 
 * v = view
 * 		|		\
 * t = 	tracks		stack
 * 		|
 * c = 	containers
 * 		|
 * w = win
 * 
 */


struct screen {
	struct screen *next;
	struct view *v;
	Window stackid;
	int x;
	int y;
	unsigned int w;
	unsigned int h;
};

struct view {
	struct view *next;
	struct view *prev;
	struct track *ft; //first track
	struct stack_item *stack;
	struct cont *mfocus; //focus
	struct stack_item *sfocus;//stackfocus
	int idx;
	bool showstack;
	bool orientv; //tracks run verically?
	bool fs; //fullscreen?
};

struct track {
	struct view *view;
	struct track *next;
	struct track *prev;
	struct cont *c;
	int size;
};

struct cont {
	struct track *track;
	struct cont *next;
	struct cont *prev;
	struct win *win; 
	int size; //size represents h or w depending on the orientation of the layout

};

struct  win {
	Window id; 
	struct win *next;
	bool del_win;
	bool take_focus;
	bool fullscreen;
};

struct stack_item {
	struct win *win;
	struct stack_item *next;
	struct stack_item *prev;
};

struct binding {
	unsigned int *mask;
	unsigned int keycode;
};


/*
 *GLOBAL VARIABLES
 */

struct view *fv = NULL; //first view
//struct view *cs->v = NULL; //current view
struct screen *cs = NULL;
struct screen *firstscreen = NULL;
struct win *first_win = NULL;

//unsigned int cs->w = 1026;
//unsigned int cs->h = 860; 
unsigned int mod = Mod1Mask;
unsigned int mods = Mod1Mask | ShiftMask;
bool sloppy_focus = true;
struct binding bindings[BINDINGS];
Display *dpy;
Window root;
unsigned long focus_pix; 
unsigned long unfocus_pix;
unsigned long stack_background_pix;
unsigned long stack_focus_pix;
unsigned long stack_unfocus_pix;
GC focus_gc = NULL;
GC unfocus_gc = NULL;
bool gxerror = false;
//Window cs->stackid;
Atom wm_del_win;
Atom wm_take_focus;
Atom wm_prot;
Atom wm_change_state;
Atom wm_fullscreen;
char *dcmd = NULL;
char *tcmd = NULL;
char *ccmd01 = NULL;
char *ccmd02 = NULL;
char *ccmd03 = NULL;
char *ccmd04 = NULL;
char *ccmd05 = NULL;
char *ccmd06 = NULL;
char *ccmd07 = NULL;
char *ccmd08 = NULL;
char *ccmd09 = NULL;
char *ccmd10 = NULL;
unsigned short res_top = 0;
unsigned short res_bot = 0;
unsigned short res_left = 0;
unsigned short res_right = 0;
unsigned short resize_inc = 15;
unsigned short offscreen = 0;

//records the keycode in appropriate array
void bind_key(char s[12], unsigned int *m, struct binding *b) {
	unsigned int code;
	code = XKeysymToKeycode(dpy,XStringToKeysym(s));
	b->keycode = code;
	b->mask = m;
}


//handles binding the default keys
void load_defaults() {
	//note that the array index is significant
	//it binds the binding to the relevant code
	//these would be more readable if we #defined them

	//resize up down left right		4	0-3
	bind_key("y",&mod,&bindings[0]);
	bind_key("u",&mod,&bindings[1]);
	bind_key("i",&mod,&bindings[2]);
	bind_key("o",&mod,&bindings[3]);
	
	//move win to view next prev 1-0	12	4-15
	bind_key("1",&mods,&bindings[4]);
	bind_key("2",&mods,&bindings[5]);
	bind_key("3",&mods,&bindings[6]);
	bind_key("4",&mods,&bindings[7]);
	bind_key("5",&mods,&bindings[8]);
	bind_key("6",&mods,&bindings[9]);
	bind_key("7",&mods,&bindings[10]);
	bind_key("8",&mods,&bindings[11]);
	bind_key("9",&mods,&bindings[12]);
	bind_key("0",&mods,&bindings[13]);
	bind_key("n",&mods,&bindings[14]);
	bind_key("m",&mods,&bindings[15]);

	//change view next prev 1-0		12	16-27
	bind_key("1",&mod,&bindings[16]);
	bind_key("2",&mod,&bindings[17]);
	bind_key("3",&mod,&bindings[18]);
	bind_key("4",&mod,&bindings[19]);
	bind_key("5",&mod,&bindings[20]);
	bind_key("6",&mod,&bindings[21]);
	bind_key("7",&mod,&bindings[22]);
	bind_key("8",&mod,&bindings[23]);
	bind_key("9",&mod,&bindings[24]);
	bind_key("0",&mod,&bindings[25]);
	bind_key("n",&mod,&bindings[26]);
	bind_key("m",&mod,&bindings[27]);

	//shift window u d r l  		4 	28-31
	bind_key("h",&mods,&bindings[28]);
	bind_key("j",&mods,&bindings[29]);
	bind_key("k",&mods,&bindings[30]);
	bind_key("l",&mods,&bindings[31]);

	//toggle stack				1	32
	bind_key("space",&mod,&bindings[32]);

	//move to stack				1	33
	bind_key("period",&mod,&bindings[33]);

	//move to main				1	34
	bind_key("comma",&mod,&bindings[34]);

	//swap stack and main			1	35
	bind_key("slash",&mod,&bindings[35]);

	//swap stack up down			2	36-37
	bind_key("semicolon",&mods,&bindings[36]);
	bind_key("apostrophe",&mods,&bindings[37]);

	//shift main focus up down left right	4	38-41
	bind_key("h",&mod,&bindings[38]);
	bind_key("j",&mod,&bindings[39]);
	bind_key("k",&mod,&bindings[40]);
	bind_key("l",&mod,&bindings[41]);

	//shift stack focus up down		2	42-43
	bind_key("semicolon",&mod,&bindings[42]);
	bind_key("apostrophe",&mod,&bindings[43]);

	//close win				2	44-45
	bind_key("Escape",&mod,&bindings[44]);
	bind_key("Escape",&mods,&bindings[45]);

	//run menu term				2	46-47
	bind_key("Return",&mod,&bindings[46]);
	bind_key("Return",&mods,&bindings[47]);

	//fullscreen				1	48
	bind_key("space",&mods,&bindings[48]);

	//quit					1	49
	bind_key("Delete",&mods,&bindings[49]);
	
	//toggle orientation
	bind_key("Tab",&mod,&bindings[50]);
	
	// user defined -60

	bind_key("r",&mod,&bindings[61]);

	//prev/next view
	bind_key("Prior", &mod,&bindings[62]);
	bind_key("Next", &mod,  &bindings[63]);
}

void spawn(char *cmd) {
	if (cmd == NULL || cmd[0] == '\0') {
		return;
	};
	if (fork() == 0) {
		if (dpy != NULL) {
			close(ConnectionNumber(dpy));
		};
		setsid();
		execl("/bin/sh","/bin/sh","-c",cmd,NULL);
		fprintf(stderr,"error number %d  spawning %s\n",errno,cmd);
		exit(1);
	} else {
		return;
	};
}

void split(char *in, char *out1, char *out2, char delim) {
	int i = 0;
	while (*in != '\0' && *in != delim) {
		out1[i] = *in;
		in++;
		i++;
	};
	out1[i] = '\0';
	if (*in == delim) {
		in++;
		strcpy (out2,in);
	} else {
		out2[0] = '\0';
	};
}

void load_conf() {
	FILE *conf;
	char confdir[512];
	char conffile[512];
	char rcfile[512];
	memset(confdir, '\0', sizeof(confdir));
	memset(conffile, '\0',sizeof(conffile));
	memset(rcfile, '\0',sizeof(rcfile));
	confdir[0] = '\0';
	char *xdgconf = getenv("XDG_CONFIG_HOME");
	if (xdgconf == NULL) {
		char *home = getenv("HOME");
		if (home != NULL) { 
			strcpy(confdir, home);
			strcat(confdir,"/.config");
		};
	} else { 
		strcat(confdir,xdgconf);
	};
	strcat(confdir,"/euclid-wm");
	strcpy(rcfile,confdir);
	strcat(rcfile,"/euclidrc");
	spawn(rcfile);

	strcpy(conffile,confdir);
	strcat(conffile,"/euclid-wm.conf");
        conf = fopen(conffile,"r");
	if (conf == NULL) {
		fprintf(stderr,"euclid-wm ERROR: could not open conf file, falling back to internal defaults\n");
		load_defaults();
		return;
	};
	printf("euclid-wm: conf file opened successfully: %s\n",conffile);
	char line[256];
	load_defaults();
	while (fgets(line, 256, conf) != NULL) {
		//parse line
		if (line[0] != '#' && line[0] != '\n') {
			char key[64];
			char val[256];
			char *v = NULL;
			memset(key,'\0',sizeof(key));
			memset(val,'\0',sizeof(val));
			split(line,key,val,'=');
			if (val[0] == ' ') {
				v = &val[1];
			} else {
				v = &val[0];
			};
			if (val[strlen(val)-1] == '\n') {
				val[strlen(val) - 1] = '\0';
			};
			if (key[strlen(key) - 1] == ' ') {
				key[strlen(key) - 1] = '\0';
			};
		
			if (strcmp(key,"dmenu") == 0) {
				dcmd = (char *) malloc(strlen(v) * sizeof(char));
				strcpy(dcmd,v);
			} else if (strcmp(key,"term") == 0) {
				tcmd = (char *) malloc(strlen(v) * sizeof(char));
				strcpy(tcmd,v);
			} else if (strcmp(key,"resize_increment") == 0) { 
				resize_inc = atoi(v);
			} else if (strcmp(key,"reserved_top") == 0) {
				res_top = atoi(v);
			} else if (strcmp(key,"reserved_bottom") == 0) {
				res_bot = atoi(v);
			} else if (strcmp(key,"reserved_left") == 0) {
				res_left = atoi(v);
			} else if (strcmp(key,"reserved_right") == 0) {
				res_right = atoi(v);
			} else if (strcmp(key,"modkey") == 0) {
				if (strcmp(v,"1") == 0) {
					mod = Mod1Mask;
					mods = Mod1Mask | ShiftMask;
				} else if (strcmp(v,"2") == 0) {
					mod = Mod2Mask;
					mods = Mod2Mask | ShiftMask;
				} else if (strcmp(v,"3") == 0) {
					mod = Mod3Mask;
					mods = Mod3Mask | ShiftMask;
				} else if (strcmp(v,"4") == 0) {
					mod = Mod4Mask;
					mods = Mod4Mask | ShiftMask;
				} else if (strcmp(v,"5") == 0) {
					mod = Mod5Mask;
					mods = Mod5Mask | ShiftMask;
				};

			} else if (key[0] == 'c' && key[2] == 'l' && key[4] == 'r') { 
			/* here we set colors:
			 *color_main_focus
			 *color_stack_focus
			 *color_stack_unfocus
			 *color_main_unfocus
			 *color_stack_background
			 */
				XColor color;
				if (XParseColor(dpy,DefaultColormap(dpy,0),v,&color) != 0) {
					XAllocColor(dpy,DefaultColormap(dpy,0),&color);
					//main_focus
					if (key[6] == 'm' && key[11] == 'f') {
						focus_pix = color.pixel;
					//main_unfocus
					} else if (key[6] == 'm' && key[11] == 'u') {
						unfocus_pix = color.pixel;
					//stack_focus
					} else if (key[6] == 's' && key[12] == 'f') {
						stack_focus_pix = color.pixel;
					//stack_unfocus 
					} else if (key[6] == 's' && key[12] == 'u') {
						stack_unfocus_pix = color.pixel;
					//stack_background
					} else if (key[6] == 's' && key[12] == 'b') {
						stack_background_pix = color.pixel;
					} else {
						fprintf(stderr,"euclid-wm ERROR: unknown color key in config: %s\n",key);
					};
				} else {
					fprintf(stderr,"euclid-wm ERROR: unparsable color: %s\n",v);
				};
			//custom commands
			//custom_command_01 = cmd arg1 arg2
			} else if (key[0] == 'c' && key[5] == 'm' && key[8] == 'o' && key[13] == 'd') {
					#define ALSTR(P,S)\
					P = (char *) malloc(strlen(S)  * sizeof(char));\
					strcpy(P,S);

				if (key[15] == '0' && key[16] == '1') {
					ALSTR(ccmd01,v)
 				} else if (key[15] == '0' && key[16] == '2') {
					ALSTR(ccmd02,v)
				} else if (key[15] == '0' && key[16] == '3') {
					ALSTR(ccmd03,v)
				} else if (key[15] == '0' && key[16] == '4') {
					ALSTR(ccmd04,v)
				} else if (key[15] == '0' && key[16] == '5') {
					ALSTR(ccmd05,v)
				} else if (key[15] == '0' && key[16] == '6') {
					ALSTR(ccmd06,v)
				} else if (key[15] == '0' && key[16] == '7') {
					ALSTR(ccmd07,v)
				} else if (key[15] == '0' && key[16] == '8') {
					ALSTR(ccmd08,v)
				} else if (key[15] == '0' && key[16] == '9') {
					ALSTR(ccmd09,v)
				} else if (key[15] == '1' && key[16] == '0') {
					ALSTR(ccmd10,v)
				};
				
			/*
			 *Actual bindings, format
			 *bind_$ACT = mod keyname
			 *mod can be M or MS
			 *keyname is the exact name passed to X
			 */
			} else if (key[0] == 'b' && key[1] == 'i' && key[3] == 'd') {
			bool known = true;
			//select the appropriate index:
				int bindx = 0;
				if (strcmp(key,"bind_resize_left") == 0) {
					bindx = 0;
				} else if (strcmp(key,"bind_resize_down") == 0) {
					bindx = 1;
				} else if (strcmp(key,"bind_resize_up") == 0) {
					bindx = 2;
				} else if (strcmp(key,"bind_resize_right") == 0) {
					bindx = 3;
				} else if (strcmp(key,"bind_move_to_previous_view") == 0) {
					bindx = 14;
				} else if (strcmp(key,"bind_move_to_next_view") == 0) {
					bindx = 15;
				} else if (strcmp(key,"bind_goto_previous_view") == 0) {
					bindx = 26;
				} else if (strcmp(key,"bind_goto_next_view") == 0) {
					bindx = 27;
				} else if (strcmp(key,"bind_shift_win_left") == 0) {
					bindx = 28;
				} else if (strcmp(key,"bind_shift_win_down") == 0) {
					bindx = 29;
				} else if (strcmp(key,"bind_shift_win_up") == 0) {
					bindx = 30;
				} else if (strcmp(key,"bind_shift_win_right") == 0) {
					bindx = 31;
				} else if (strcmp(key,"bind_toggle_stack") == 0) {
					bindx = 32;
				} else if (strcmp(key,"bind_move_to_stack") == 0) {
					bindx = 33;
				} else if (strcmp(key,"bind_move_to_main") == 0) {
					bindx = 34;
				} else if (strcmp(key,"bind_swap_stack_and_main") == 0) {
					bindx = 35;
				} else if (strcmp(key,"bind_swap_stack_up") == 0) {
					bindx = 36;
				} else if (strcmp(key,"bind_swap_stack_down") == 0) {
					bindx = 37;
				} else if (strcmp(key,"bind_focus_left") == 0) {
					bindx = 38;
				} else if (strcmp(key,"bind_focus_down") == 0) {
					bindx = 39;
				} else if (strcmp(key,"bind_focus_up") == 0) {
					bindx = 40;
				} else if (strcmp(key,"bind_focus_right") == 0) {
					bindx = 41;
				} else if (strcmp(key,"bind_stack_focus_up") == 0) {
					bindx = 42;
				} else if (strcmp(key,"bind_stack_focus_down") == 0) {
					bindx = 43;
				} else if (strcmp(key,"bind_close_win") == 0) {
					bindx = 44;
				} else if (strcmp(key,"bind_kill_win") == 0) {
					bindx = 45;
				} else if (strcmp(key,"bind_spawn_menu") == 0) {
					bindx = 46;
				} else if (strcmp(key,"bind_spawn_term") == 0) {
					bindx = 47;
				} else if (strcmp(key,"bind_toggle_fullscreen") == 0) {
					bindx = 48;
				} else if (strcmp(key,"bind_quit") == 0) {
					bindx = 49;
				} else if (strcmp(key,"bind_toggle_orientation") == 0) {
					bindx = 50;
				} else if (strcmp(key,"bind_custom_01") == 0) {
  					bindx = 51;
  				} else if (strcmp(key,"bind_custom_02") == 0) {
					bindx = 52;
				} else if (strcmp(key,"bind_custom_03") == 0) {
					bindx = 53;
				} else if (strcmp(key,"bind_custom_04") == 0) {
					bindx = 54;
				} else if (strcmp(key,"bind_custom_05") == 0) {
					bindx = 55;
				} else if (strcmp(key,"bind_custom_06") == 0) {
					bindx = 56;
				} else if (strcmp(key,"bind_custom_07") == 0) {
					bindx = 57;
				} else if (strcmp(key,"bind_custom_08") == 0) {
					bindx = 58;
				} else if (strcmp(key,"bind_custom_09") == 0) {
					bindx = 59;
				} else if (strcmp(key,"bind_custom_10") == 0) {
					bindx = 60;
				} else if (strcmp(key,"bind_reload_config") == 0) {
					bindx = 61;
				} else if (strcmp(key,"bind_goto_previous_screen") == 0) {
					bindx = 62;
				} else if (strcmp(key,"bind_goto_next_screen") == 0) {
					bindx = 63;
				} else {
					fprintf(stderr,"euclid-wm ERROR: uknown binding in config: \"%s\"\n",key),
					known = false;
				};

				if (known == true) {
				
					//get the name of the key, and the modifier
					char m[3];
					char xkey[24];
					split(v,m,xkey,' ');
					if (m[1] == 'S') {
						bind_key(xkey,&mods,&bindings[bindx]);
					} else {
						bind_key(xkey,&mod,&bindings[bindx]);				
					};
				};
			};	 
		};
	};
	fclose(conf);
}

void commit_bindings() {
	int i;
	for (i = 0; i < BINDINGS; i++) {
		//need to check whether the binding has been set before sending garbage to X
		if (bindings[i].keycode != 0) {
			XGrabKey(dpy,bindings[i].keycode,*(bindings[i].mask),root,True,GrabModeAsync,GrabModeAsync);
			if (gxerror == true) {
				fprintf(stderr,"euclid-wm ERROR: error grabbing key %d\n",bindings[i].keycode);
				gxerror = false;
			};
		};
	};
}

/*The wmname bit fixes bugs in dumb programs that assume a reparenting wm
 taken from scrotwm, which took it from wmname
 now it also does general atom stuff
 */
void set_atoms() {
	
	wm_del_win = XInternAtom(dpy,"WM_DELETE_WINDOW",False);
	wm_take_focus = XInternAtom(dpy,"WM_TAKE_FOCUS",False);
	wm_prot = XInternAtom(dpy, "WM_PROTOCOLS", False);
	wm_change_state = XInternAtom(dpy,"_NET_WM_STATE",False);
	wm_fullscreen = XInternAtom(dpy,"_NET_WM_STATE_FULLSCREEN",False);
	Atom wm_supported = XInternAtom(dpy,"_NET_SUPPORTED",False);
	Atom wm_check = XInternAtom(dpy,"_NET_SUPPORTING_WM_CHECK",False);
	Atom wm_name = XInternAtom(dpy,"_NET_WM_NAME",False);
	Atom utf8 = XInternAtom(dpy,"UTF8_STRING",False);
	Atom supported[] = {wm_supported, wm_name, wm_change_state, wm_fullscreen};
	XChangeProperty(dpy,root,wm_check,XA_WINDOW,32,PropModeReplace,(unsigned char *)&root,1);
	XChangeProperty(dpy,root,wm_name,utf8,8,PropModeReplace,(unsigned char *) "LG3D",strlen("LG3D"));
	XChangeProperty(dpy,root,wm_supported,XA_ATOM,32,PropModeReplace,(unsigned char *) supported,4);
	XSync(dpy,False);
}

struct view * make_view() {
	struct view *ptr = (struct view *) malloc(sizeof(struct view));
	ptr->next = NULL;
	ptr->prev = NULL;
	ptr->mfocus = NULL;
	ptr->sfocus = NULL;
	ptr->idx = 0;
	ptr->ft = (struct track *) malloc(sizeof(struct track)); 
	ptr->ft->view = ptr;
	ptr->ft->next = NULL;
	ptr->ft->prev = NULL;
	ptr->ft->c = NULL;
	ptr->ft->size = cs->w;
	ptr->orientv = true;
	ptr->stack = NULL;
	ptr->showstack = true;
	ptr->fs = false;
	return ptr;
 }

struct view * find_view(int i);

void addscreen(short h, short w, short x, short y, short n) {

	struct screen *new = (struct screen * ) malloc (sizeof(struct screen));
	//set the pointers
	
	
	printf("adding screen %d: \n\tH %d \n\tW %d \n\tX %d \n\tY %d\n",n,h,w,x,y);
	new->next = NULL;
	if (firstscreen == NULL) {
		//set it as first screen
		firstscreen = new;
		cs = new;
		new->v = make_view();
		new->v->idx = 1;
		fv = new->v;
	} else {
		struct screen *s = firstscreen;
		while (s->next != NULL) {
			s = s->next;
		};
		s->next = new;
		new->v = find_view(n + 1);
	}
	//define its geomentry
	new->h = h;
	new->w = w;
	new->x = x;
	new->y = y;
	//make it a stack window
	new->stackid = XCreateSimpleWindow(dpy,root,0,((h - 15) + y),(w + x),15,1,stack_unfocus_pix,stack_background_pix);
	XSetWindowAttributes att;
	att.override_redirect = true;
	XChangeWindowAttributes(dpy,cs->stackid,CWOverrideRedirect,&att);
	XMapWindow(dpy,new->stackid);
	XSync(dpy,False);
}

void remove_cont(struct cont *c) {
	//reset focus if necessary
	if (c->next != NULL) {
		c->track->view->mfocus = c->next;
	} else if (c->prev != NULL) {
		c->track->view->mfocus = c->prev;
	} else if (c->track->next != NULL) {
		c->track->view->mfocus = c->track->next->c;
	} else if (c->track->prev != NULL) {
		c->track->view->mfocus = c->track->prev->c;
	} else {
		c->track->view->mfocus = NULL;
	};
	//remove c and clean up
	//is c first in the track?
	if (c->track->c == c) {
		//is it only in track?
		if (c->next == NULL) {
			//is track first?
			if (c->track == c->track->view->ft) {
				//is track only?
				if (c->track->next == NULL) {
					c->track->c = NULL;
					//if we are in fs we need to show the stack now
					c->track->view->fs = false;
					c->track->view->showstack = true;
					free(c);
					//set focus to root so we still get keys!
					XSetInputFocus(dpy,root,None,CurrentTime);
				} else {
					//its the first track, but there is a next
					c->track->view->ft = c->track->next;
					c->track->next->prev = NULL;
					free (c->track);
					free (c);
				};
			} else { //not first track 
				//delete it 
				c->track->prev->next = c->track->next;
				if (c->track->next != NULL) {
					c->track->next->prev = c->track->prev;
				};
				free (c->track);
				free(c);
			}; 
		} else {//its just first
			c->track->c = c->next;
			c->next->prev = NULL;
			free(c);
		};
	} else { // is there a prev 
		c->prev->next = c->next;
		if (c->next != NULL) {
			c->next->prev = c->prev;
		};
		free(c);
	};
}


struct win * add_win(Window  id) {
	//PropertyChangeMask might be important for future features
	if (sloppy_focus == true) {
		XSelectInput(dpy,id,PointerMotionMask | PointerMotionHintMask | EnterWindowMask);
	};
	XSetWindowBorderWidth(dpy,id,1);
	struct win *p = (struct win *) malloc(sizeof(struct win));
	p->take_focus = false;
	p->del_win = false;
	Atom *prot = NULL;
	Atom *pp = NULL;
	int n, j;
	if (XGetWMProtocols(dpy, id, &prot, &n)) {
		for (j = 0, pp = prot; j < n; j++, pp++) {
			if (*pp == wm_del_win) {
				p->del_win = true;
			} else if (*pp == wm_take_focus) {
				p->take_focus = true;
			};
		};
 	};
	if (prot) {
		XFree(prot);
	};
	p->next = first_win;
	first_win = p;
	p->id = id;
	return p;
}

void forget_win (Window id) {
	//first see whether we have a record of it
	if (first_win == NULL) {
		fprintf(stderr,"euclid-wm: cannot remove window %6.0lx, internal data structure is corrpupt or there are not windows being managed (no first_win defined)\n",id);
		return;
	};
	struct win * w = first_win;
	struct win * w2 = first_win;
	if (w->id != id) {
		while (w->next != NULL && w->next->id != id) {
			w = w->next;
		};
		w2 = w; //this should be the win struct before the one we are deleting
		w = w->next;
	}; 
	
	if (w == NULL) { 
		return;
	};
	//we have the win struct stored in w;
	struct view *v = fv;
	struct track *t;
	struct cont *c;
	while (v != NULL) {
		t = v->ft;
		while (t != NULL) {
			c = t->c;
			while (c != NULL) {
				if (c->win == w) {
					//if it is the main focus on a fullscreen desktop
					//take the desktop out of fs
					c->track->view->fs = false;
					//reset focus if necessary
					if (v->mfocus == c) {
						if (c->next != NULL) {
							v->mfocus = c->next;
						} else if (c->prev != NULL) {
							v->mfocus = c->prev;
						} else if (c->track->next != NULL) {
							v->mfocus = c->track->next->c;
						} else if (c->track->prev != NULL) {
							v->mfocus = c->track->prev->c;
						} else {
							v->mfocus = NULL;
						};
					};
					//remove c and clean up
					//is c first in the track?
					if (c->track->c == c) {
						//is it only in track?
						if (c->next == NULL) {//remove track
							//is track first?
							if (c->track == c->track->view->ft) {
								//is track only?
								if (c->track->next == NULL) {
									c->track->c = NULL;
									free(c);
								} else {
									//its the first track, but there is a next
									cs->v->ft = c->track->next;
									c->track->next->prev = NULL;
									free (c->track);
									free (c);
								};
							} else { //not first track 
								//delete it 
								c->track->prev->next = c->track->next;
								if (c->track->next != NULL) {
									c->track->next->prev = c->track->prev;
								};
								free (c->track);
								free(c);
							}; 
						} else {//its just first
							c->track->c = c->next;
							c->next->prev = NULL;
							free(c);
						};
					} else { //there's a prev 
						c->prev->next = c->next;
						if (c->next != NULL) {
							c->next->prev = c->prev;
						};
						free(c);
					};
				};
				c = c->next;
			};
			t = t->next;
		};
		v = v->next;
	};
	//we also need to check the stacks:
	v = fv;
	struct stack_item *s = NULL;
	while (v != NULL) {
		s = v->stack;
		while (s != NULL) {
			if (s->win == w) {
				if (s == v->stack) {
					v->stack = s->next;
				};
				if (s == v->sfocus) {
					if (s->next != NULL) {
						v->sfocus = s->next;
					} else {
						v->sfocus = s->prev;
					};
				};
				if (s->next != NULL) {
					s->next->prev = s->prev;
				};
				if (s->prev != NULL) {
					s->prev->next = s->next;
				};
				if (s == v->stack) {
					v->stack = s->next;
				};
				free(s);
			};
			s = s->next;
		};
		v = v->next;
	};
	//remove w
	//is it first?
	if (first_win == w) {
		first_win = w->next;
	};
	w2->next = w->next;
	free(w);
}

void add_client_to_view (struct win *p, struct view *v) {
	//first make sure it is not already in the target view:
	struct track *tp = v->ft;
	struct cont *cp = NULL;
	while (tp != NULL) {
		cp = tp->c;
		while (cp != NULL) {
			if (cp->win->id == p->id) {
				return;
			};
			cp = cp->next;
		};
		tp = tp->next;
	};

	//make a cont for it
	struct cont *c = (struct cont *) malloc  (sizeof(struct cont));
	//no we need to test all these
	if (v->mfocus != NULL) {
		c->next = v->mfocus->next;
		c->prev = v->mfocus;
		if (v->mfocus->next != NULL) {
			v->mfocus->next->prev = c;
		};
		v->mfocus->next = c;
		c->track = v->mfocus->track;
		c->size = v->mfocus->size;
	} else {
		//first client in view
		c->next = NULL;
		c->prev = NULL;
		c->track = v->ft;
		v->ft->c = c;
		if (v->orientv == true) {
			c->size = cs->h;
		} else {
			c->size = cs->w;
		};
	};
	c->win = p;
	v->mfocus = c;
}

void move_to_stack(struct cont *c) {
	struct stack_item *s = (struct stack_item *) malloc (sizeof(struct stack_item));
	struct stack_item *p = cs->v->sfocus;
	s->win = c->win;
	if (p != NULL) {
		s->next = p->next;
		if (p->next != NULL) {
			p->next->prev = s;
		};
		p->next = s;
	} else {
		cs->v->stack = s;
		s->next = NULL;
	};
	s->prev = p;
	cs->v->sfocus = s;
	remove_cont(c);
}

void move_to_main() {
	//just add whatever has stack focus to the layout
	if (cs->v->sfocus == NULL) {return;};
	add_client_to_view(cs->v->sfocus->win, cs->v);
	//remove it from the stack:
	struct stack_item *p = cs->v->sfocus;
	//reset sfocus
	cs->v->sfocus = NULL;
	if (p->prev != NULL) {
		cs->v->sfocus = p->prev;
		p->prev->next = p->next;
	};
	if (p->next != NULL) {
		p->next->prev = p->prev;
		if (cs->v->sfocus == NULL) {
			cs->v->sfocus = p->next;
			cs->v->stack = p->next;
		};
	};
	if (cs->v->sfocus == NULL) {
		cs->v->stack = NULL;
	};
	free(p);
}

void shift_stack_focus (bool dir) {
	//true up, false, down
	if (cs->v->sfocus == NULL) {return;};
	if (dir && cs->v->sfocus->prev != NULL) {
		cs->v->sfocus = cs->v->sfocus->prev;
	} else if (!dir && cs->v->sfocus->next != NULL) {
		cs->v->sfocus = cs->v->sfocus->next;
	};
}

bool is_top_level(Window id) {
	Window r; //root return;
	Window p; //parent return;
	Window *c; //children;
	unsigned int nc; //number of children 
	//this is a hack, hopefully temproary: for some reason in a multiscreen setup the stack for the second screen is showing up as a normal top level window:

	struct screen *s = firstscreen;
	while (s != NULL) {
		if (s->stackid == id) {
			return false;
		};
		
		s = s->next;
	};

	gxerror = false;
	XQueryTree(dpy,root,&r,&p,&c,&nc);
	if (gxerror) {
		gxerror = false;
		return(false);
	};
	int i;
	for (i = 0; i < nc; i++) {
		if (c[i] == id) {
			XWindowAttributes wa;
			XGetWindowAttributes(dpy,id,&wa);
			if (gxerror == true) {
				gxerror = false;
				XFree(c);
				return(false);
			};
			if (wa.override_redirect == true) {
				XFree(c);
				return(false);
			} else {
				XFree(c);
				return(true);
			};
		};
	};
	XFree(c);
	return(false);
}

short int convert_to_internal_dir(short int dir) {/*four possibilities: 
	 *  1) we are moving down in the track
	 *  2) we are movign up in the track
	 *  3) we are moving down accross tracks
	 *  4) we are moving up accross tracks
	 */
	if (cs->v->orientv == true) {
		short swp[] = {0,2,3,1,4};
		dir = swp[dir];
	} else {
		short swp[] = {0,4,1,3,2};
		dir = swp[dir];
	};
	return dir;
}
void shift_window(short int dir) {
	if (cs->v->mfocus == NULL) {
		return;
	};
	dir = convert_to_internal_dir(dir);
	if (dir == 1 && cs->v->mfocus->next != NULL) { //down in the track;
		//get a direct reference to all four nodes
		//make sure that we are also updating track->c if necessary 
		struct cont *tmpa;
		struct cont *tmpb;
		struct cont *tmpc;
		struct cont *tmpd;
		tmpa = cs->v->mfocus->prev;
		tmpb = cs->v->mfocus;
		tmpc = cs->v->mfocus->next;
		if (cs->v->mfocus->next != NULL) {
			tmpd = cs->v->mfocus->next->next;
		} else {tmpd = NULL;};
		if (tmpa != NULL) {
			tmpa->next = tmpc;
		} else { //a is null, b is track->c
			cs->v->mfocus->track->c = tmpc;
		};
		if (tmpd != NULL) {
			tmpd->prev = tmpb;
		};
		if (tmpc != NULL) {
			tmpc->prev = tmpa;
			tmpc->next = tmpb;
		};
		tmpb->prev = tmpc;
		tmpb->next = tmpd;
	} else if (dir == 2 && cs->v->mfocus->prev != NULL) {
		struct cont *tmpa;
		struct cont *tmpb;
		struct cont *tmpc;
		struct cont *tmpd;
		if (cs->v->mfocus->prev != NULL) {
			tmpa = cs->v->mfocus->prev->prev;
		} else {
			tmpa = NULL;
		};
		tmpb = cs->v->mfocus->prev;
		tmpc = cs->v->mfocus;
		tmpd = cs->v->mfocus->next;
		if (tmpa != NULL) {
			tmpa->next = tmpc;
		};
		if (cs->v->mfocus->track->c == tmpb) {
			cs->v->mfocus->track->c = tmpc;
		};
		if (tmpb != NULL) {
			tmpb->prev = tmpc;
			tmpb->next = tmpd;
		};
		if (tmpd != NULL) {
			tmpd->prev = tmpb;
		};
		tmpc->prev = tmpa;
		tmpc->next = tmpb;
	} else if (dir == 3) {
		//check for track->next
		if (cs->v->mfocus->track->next != NULL) {
			//find position in it
			//find the midpoint of the current cont
			struct cont *p;
			p = cs->v->mfocus->prev;
			int s = 0;
			int t = 0;
			while (p != NULL) {
				s += p->size;
				p = p->prev;
			};
			s += (cs->v->mfocus->size / 2);
			p = cs->v->mfocus->track->next->c;
			while (p->next != NULL) {
				t += p->size;
				if (t >= s) {break;};
				p = p->next;
			};
			if (p != NULL) {//it never should
				if (cs->v->mfocus->prev != NULL) {
					cs->v->mfocus->prev->next = cs->v->mfocus->next;
				} else { //it's first 
					if (cs->v->mfocus->next != NULL) {
						cs->v->mfocus->track->c = cs->v->mfocus->next;
					} else {
						//only one in the track
						//remove the old track
						if (cs->v->ft == cs->v->mfocus->track) {
							cs->v->ft = p->track;
						} else {
							cs->v->mfocus->track->prev->next = p->track;
						}; 
						p->track->prev = cs->v->mfocus->track->prev;
						free(cs->v->mfocus->track);
					};
				};
				if (cs->v->mfocus->next != NULL) {
					cs->v->mfocus->next->prev = cs->v->mfocus->prev;
				};
				//put it behind p
				struct cont *b = p->next;
				if (b != NULL) {
					b->prev = cs->v->mfocus;
				};
				p->next = cs->v->mfocus;
				cs->v->mfocus->prev = p;
				cs->v->mfocus->next = b;
				cs->v->mfocus->track = p->track;
			};
		}else{ //make a track for it
			if (cs->v->mfocus->track->c == cs->v->mfocus) { 
				if (cs->v->mfocus->next != NULL) {
						cs->v->mfocus->track->c = cs->v->mfocus->next;
					} else if (cs->v->mfocus->prev != NULL) {
						cs->v->mfocus->track->c = cs->v->mfocus->prev;
					} else {
						return; //premature return, because movign the window that direction doesn't make any sense
					};
				};
			struct track *ptr = (struct track *) malloc (sizeof(struct track));
			cs->v->mfocus->track->next = ptr;
			//update all the other links
			ptr->view = cs->v;
			ptr->next = NULL;
			ptr->prev = cs->v->mfocus->track;
			ptr->c = cs->v->mfocus;
			ptr->size = cs->v->mfocus->track->size;
			cs->v->mfocus->track = ptr;
			//patch up the hole in mfocus->track
			if (cs->v->mfocus->prev != NULL) 
				cs->v->mfocus->prev->next = cs->v->mfocus->next;
			if (cs->v->mfocus->next != NULL)
				cs->v->mfocus->next->prev = cs->v->mfocus->prev;
			cs->v->mfocus->next = NULL;
			cs->v->mfocus->prev = NULL;
		};
	} else if (dir == 4) { //4
		if (cs->v->mfocus->track->prev != NULL) {
			//find position in it
			//find the midpoint of the current cont
			struct cont *p;
			p = cs->v->mfocus->prev;
			int s = 0;
			int t = 0;
			while (p != NULL) {
				s += p->size;
				p = p->prev;
			};
			s += (cs->v->mfocus->size / 2);
			p = cs->v->mfocus->track->prev->c;
			while (p->next != NULL) {
				t += p->size;
				if (t >= s) {break;};
				p = p->next;
			};
			if (p != NULL) {//it never should
				if (cs->v->mfocus->prev != NULL) {
					cs->v->mfocus->prev->next = cs->v->mfocus->next;
				} else { //it's first in the track
					if (cs->v->mfocus->next != NULL) {
						cs->v->mfocus->track->c = cs->v->mfocus->next;
					} else {
						//only one in the track
						//remove the old track
						if (cs->v->mfocus->track->next != NULL) {
							cs->v->mfocus->track->next->prev = p->track;
						}; 
						p->track->next = cs->v->mfocus->track->next;
						free(cs->v->mfocus->track);
					};
				};
				if (cs->v->mfocus->next != NULL) {
					cs->v->mfocus->next->prev = cs->v->mfocus->prev;
				};
				//put it behind p
				struct cont *b = p->next;
				if (b != NULL) {
					b->prev = cs->v->mfocus;
				};
				p->next = cs->v->mfocus;
				cs->v->mfocus->prev = p;
				cs->v->mfocus->next = b;
				cs->v->mfocus->track = p->track;
			};
		}else{ //make a track for it
			if (cs->v->mfocus->track->c == cs->v->mfocus) { 
				if (cs->v->mfocus->next != NULL) {
						cs->v->mfocus->track->c = cs->v->mfocus->next;
					} else if (cs->v->mfocus->prev != NULL) {
						cs->v->mfocus->track->c = cs->v->mfocus->prev;
					} else {
						return; //premature return, because movign the window that direction doesn't make any sense
					};
				};
			struct track *ptr = (struct track *) malloc (sizeof(struct track));
			cs->v->mfocus->track->prev = ptr;
			cs->v->ft = ptr;
			//update all the other links
			ptr->view = cs->v;
			ptr->next = cs->v->mfocus->track;
			ptr->prev = NULL;
			ptr->c = cs->v->mfocus;
			ptr->size = cs->v->mfocus->track->size;
			cs->v->mfocus->track = ptr;
			//patch up the hole in mfocus->track
			if (cs->v->mfocus->prev != NULL) 
				cs->v->mfocus->prev->next = cs->v->mfocus->next;
			if (cs->v->mfocus->next != NULL)
				cs->v->mfocus->next->prev = cs->v->mfocus->prev;
			cs->v->mfocus->prev = NULL;
			cs->v->mfocus->next = NULL;
		};
	};
	if (cs->v->mfocus->prev != NULL) {
		cs->v->mfocus->size = cs->v->mfocus->prev->size;
	} else if (cs->v->mfocus->next != NULL) {
		cs->v->mfocus->size = cs->v->mfocus->next->size;
	};
}

void shift_main_focus(short int dir) {
	if (cs->v->mfocus == NULL) {return;};
	dir = convert_to_internal_dir(dir);
	if (dir == 1) {
		if (cs->v->fs == false) {
			if (cs->v->mfocus->next != NULL) {
				cs->v->mfocus =  cs->v->mfocus->next;
			};
		} else {
			if (cs->v->mfocus->next != NULL) {
				cs->v->mfocus = cs->v->mfocus->next;
			} else if (cs->v->mfocus->track->next != NULL) {
				cs->v->mfocus = cs->v->mfocus->track->next->c;
			};
		};
	} else if (dir == 2) { 
		if (cs->v->fs == false) {
			if (cs->v->mfocus->prev != NULL) {
				cs->v->mfocus = cs->v->mfocus->prev;
			};
		} else {
			if (cs->v->mfocus->prev != NULL) {
				cs->v->mfocus = cs->v->mfocus->prev;
			} else if (cs->v->mfocus->track->prev != NULL ) {
				cs->v->mfocus = cs->v->mfocus->track->prev->c;
			};
		};
	} else if (dir == 3 && cs->v->mfocus->track->next != NULL) {
		if (cs->v->fs == false) {
			struct cont *p;
				p = cs->v->mfocus->prev;
				int s = 0;
				int t = 0;
				while (p != NULL) {
					s += p->size;
					p = p->prev;
				};
				s += (cs->v->mfocus->size / 2);
				p = cs->v->mfocus->track->next->c;
				while (p->next != NULL) {
					t += p->size;
					if (t >= s) {break;};
					p = p->next;
				};
				cs->v->mfocus = p;
		};
	} else if (dir == 4 && cs->v->mfocus->track->prev != NULL) {
		if (cs->v->fs == false) {
			struct cont *p;
				p = cs->v->mfocus->prev;
				int s = 0;
				int t = 0;
				while (p != NULL) {
					s += p->size;
					p = p->prev;
				};
				s += (cs->v->mfocus->size / 2);
				p = cs->v->mfocus->track->prev->c;
				while (p->next != NULL) {
					t += p->size;
					if (t >= s) {break;};
					p = p->next;
				};
				cs->v->mfocus = p;
		};
	};
}

struct view * find_view (int i) {
	//this will return a pointer to a given view
	//even if it must first create it
	//some views get a numbered index
	//i -2 means move forward -3 means backward
	//-1 means last
	struct view *v = NULL;
	if (i == -2) {
		//move forward
		if (cs->v->next != NULL) {
			return(cs->v->next);
		} else {
			//make cs->v->next
			v = make_view();
			cs->v->next = v;
			v->prev = cs->v;
			v->idx = (cs->v->idx + 1);
		};
	} else if (i == -3) {
		//move backward
		if (cs->v->prev != NULL) {
			return(cs->v->prev);
		} else {
			//make cs->v->prev
			v = make_view();
			v->next = cs->v;
			cs->v->prev = v;
			v->idx = (cs->v->idx - 1);
			fv = v;
		};
	} else if (i == -1) {
		v = cs->v;
		while (v->next != NULL) {
			v = v->next;
		};
	} else if (i <= 9 && i >= 1) {
			v = fv;
			while (v->next != NULL && v->next->idx < i) {
				v = v->next;
			};
			if (fv->idx > i) { 
				v = make_view();
				v->idx = i;
				v->next = fv;
				v->prev = NULL;
				v->next->prev = v;
				fv = v;
			} else if (v->next == NULL) {
				//we overshot, and ran out of views: make the view
				//or we are looking for fv
				if (i == v->idx) {
					return(v);
				};
				v->next = make_view();
				v->next->idx = i;
				v->next->prev = v;
				v = v->next;
			} else if (v->next->idx > i && v->idx < i) {
				//we overshot, but there are higher indexed views
				//make a view in front of v
				//store v->next
				struct view *v2 = v->next;
				v->next = make_view();
				v->next->idx = i;
				v->next->prev = v;
				v->next->next = v2;
				v2->prev = v->next;
				v = v->next;
			} else if (v->next->idx == i) {
				v = v->next;
			};
	};
	return(v);
}

void goto_view(struct view *v) {
	//this just unmaps the windows of the current view
	//sets cs->v
	//and maps the windows of the new cs->v
	//it also deletes empty views
	if (v == NULL) {return;};
	struct screen *s = firstscreen;
	while (s != NULL) {
		if (s->v == v) {return;};
		s = s->next;
	};
	struct track *t = cs->v->ft;
	struct cont *c;
	while (t != NULL) {
		c = t->c;
		while (c != NULL) {
			XUnmapWindow(dpy,c->win->id);
			c = c->next;
		};
		t = t->next;
	};
	if (cs->v->mfocus == NULL && cs->v->sfocus == NULL) {
		if (cs->v->prev != NULL) {
			cs->v->prev->next = cs->v->next;
		};
		if (cs->v->next != NULL) {
			cs->v->next->prev = cs->v->prev;
		};
		if (cs->v == fv) {
			fv = cs->v->next;
		};
		free(cs->v);
	};
	cs->v = v;
	t = v->ft;
	while (t != NULL) {
		c = t->c;
		while (c != NULL) {
			XMapWindow(dpy,c->win->id);
			c = c->next;
		};
		t = t->next;
	};
}


void move_to_view(struct view *v) {
	//move currenlty focused item
	if (v == NULL || v == cs->v || cs->v->mfocus == NULL) {return;};
	//remove it from the current view
	struct win *w = cs->v->mfocus->win;
	//check whether the view we are moving it to is displayed before unmapping
	struct screen *s = firstscreen;
	while (s != NULL && s->v != v) {
		s = s->next;
	};
	if (s == NULL) {
		XUnmapWindow(dpy,w->id);
	};
	remove_cont(cs->v->mfocus);
	//add it to the new view
	add_client_to_view(w, v);
}

struct cont * id_to_cont(Window w) {
	struct track *t;
	struct cont *c;
	struct screen *s = firstscreen;
	while (s != NULL) {
		t = s->v->ft;
		while (t != NULL) {
			c = t->c;
			while (c!= NULL) {
				if (c->win->id == w) {
					return (c);
				};
				c = c->next;
			};
			t = t->next;
		};
	s = s->next;
	};
	return (NULL);
}

void resize (int dir) {
	if (cs->v->orientv == true) {
		switch (dir) {
			case 1:
				cs->v->mfocus->size -= resize_inc;
			break; 
			case 2:
				cs->v->mfocus->track->size += resize_inc;
			break;
			case 3:
				cs->v->mfocus->size += resize_inc;
			break;
			case 4:
				cs->v->mfocus->track->size -= resize_inc;
			break;
		};
	} else {
		switch (dir) {
			case 1:
				cs->v->mfocus->track->size += resize_inc;
			break; 
			case 2:
				cs->v->mfocus->size += resize_inc;
			break;
			case 3:
				cs->v->mfocus->track->size -= resize_inc;
			break;
			case 4:
				cs->v->mfocus->size -= resize_inc;
			break;
		};
	};
}

void layout() {
	struct screen *s = firstscreen;
	while (s != NULL) {
		int xo = s->x;
		int yo = s->y;
		int stackheight;
		if (s->v->showstack == false || s->v->fs == true) {
			stackheight = 0;
		} else {
			struct stack_item *si = s->v->stack;
			int i = 0;
			while (si != NULL) {
				i++;
				si = si->next;
			};
			stackheight = (i * 20); 
			if (i == 0) {
				stackheight = 8; 
			};
		};
		//draw the stack 
		XClearWindow(dpy,s->stackid);
		if (stackheight != 0) {
			XMoveResizeWindow(dpy,s->stackid,(res_left + xo),(((s->h - res_bot) - stackheight) + yo),(s->w - (res_left + res_right + 2)),(stackheight - 2));
			XRaiseWindow(dpy,s->stackid);
			XSync(dpy,false);//important!
			struct stack_item *si = s->v->stack;
			int i = 15;
			while (si != NULL) {
				GC gc;
				if (si == s->v->sfocus) {
					gc = focus_gc;
				} else {
					gc = unfocus_gc;
				};
				XTextProperty wmname;
				XGetWMName(dpy,si->win->id,&wmname);
				XDrawString(dpy,s->stackid,gc,3,i, (char *) wmname.value,wmname.nitems);
				si = si->next;
				i += 20;
			}; 
		} else { //hide stack
			XMoveResizeWindow(dpy,s->stackid,0,offscreen,s->w,10);
		};
		XSync(dpy,false);
		if (s->v->mfocus == NULL) {
			XSetInputFocus(dpy,root,None,CurrentTime);
		};
		if (s->v->fs == true && s->v->mfocus != NULL && s->v->mfocus->win != NULL) {
			//draw mf fullscreen
			int w = s->w + 2;
			int h = s->h;
/*			if (s->v->showstack == true) {
				h += 1;
			} else {
				h += 2;
			};
These lines shouldn't be necessary AS LONG AS we are hidding the stack in fs
*/
			h -= stackheight;
			XMoveResizeWindow(dpy,s->v->mfocus->win->id,(-1),(-1),(w),(h));
			XRaiseWindow(dpy,s->v->mfocus->win->id);
			if (cs == s) {
				if (s->v->mfocus->win->take_focus == true) {
					XClientMessageEvent cm;
					memset (&cm,'\0', sizeof(cm));
					cm.type = ClientMessage;
					cm.window = s->v->mfocus->win->id;
					cm.message_type = wm_prot;
					cm.format = 32;
					cm.data.l[0] = wm_take_focus;
					cm.data.l[1] = CurrentTime;
				};
				XSetInputFocus(dpy,s->v->mfocus->win->id,None,CurrentTime);
			};
			XSync(dpy,false);
		} else {
			//first check that the tracks layout:
			struct track *curt = s->v->ft;
			struct cont *curc = NULL;
			int target;
			int tot = 0;
			if (s->v->orientv == true) {
				target = s->w - (res_left + res_right);
			} else {
				target = (s->h - (res_top + res_bot)) - stackheight;
			};
			int nooftracks = 0;
			while (curt != NULL) {
				//check if the size is negligably small, requireing a reseize
				if (curt->size <= 5) {
					curt->size += 40;
				};
				tot += curt->size;
				curt = curt->next;
				nooftracks ++;
			};
			//compare tot to target, if they match go on 
			if (tot != target) {
				signed int delta = target - tot;
				delta /= nooftracks;
				curt = s->v->ft;
				while (curt != NULL) {
					curt->size += delta;
					curt = curt->next;
				};
			};
			//else calculate the difference, and adjust all tracks
			//second check that within each track the containers fit
			curt = s->v->ft;
			if (s->v->orientv != true) {
				target = s->w - (res_left + res_right);
			} else {
				target = (s->h - (res_top + res_bot)) - stackheight;
			}; 
			while (curt != NULL) {
				curc = curt->c;
				int tot = 0;
				int noofconts = 0;
				while (curc != NULL) {
					//check for cont with negligable size and resize if necessary
					if (curc->size <= 5) {
						curc->size += 40;
					};
					noofconts ++;
					tot += curc->size;
					curc = curc->next;
				};
				//if tot == target, do nothing
				//else calculate and distribute difference
				if (tot != target) {
					signed int delta = target - tot;
				 	if (noofconts != 0) {
						delta /= noofconts;
					} else {
						delta = 0;
					};
					curc = curt->c;
					while (curc != NULL) {
						curc->size += delta;
						curc = curc->next;
					};
				};
				curt = curt->next;
			};
			//walk and draw
			curt = s->v->ft; //first track of view
			int x;
			int y;
			int h;
			int w;
			int offsett = 0;
			int offsetc = 0;
			while (curt != NULL) {
				offsetc = 0;
				curc = curt->c;
				while (curc != NULL) {
					//make sure we tell windows that think they are fs that they aren't
					if (curc->win->fullscreen == true) {
						curc->win->fullscreen = false;
						XChangeProperty(dpy,curc->win->id,wm_change_state,XA_ATOM,32,PropModeReplace,(unsigned char *)0,0);
					};
					if (s == cs && curc == s->v->mfocus) {
					//set border
						XSetWindowBorder(dpy,curc->win->id,focus_pix);
						if (s->v->mfocus->win->take_focus == true) {
							XClientMessageEvent cm;
							memset (&cm,'\0', sizeof(cm));
							cm.type = ClientMessage;
							cm.window = s->v->mfocus->win->id;
							cm.message_type = wm_prot;
							cm.format = 32;
							cm.data.l[0] = wm_take_focus;
							cm.data.l[1] = CurrentTime;
						}; 
						//we intentionally do this even if the event was sent, the
						//event alone does not suffice to get focus on the window
						XSetInputFocus(dpy,curc->win->id,None,CurrentTime);
	
	
					} else {
						XSetWindowBorder(dpy,curc->win->id,unfocus_pix);
					};
					//place window
					if (s->v->orientv == true) {
						x = offsett + res_left + xo;
						y = offsetc + res_top + yo;
						w = curt->size - 2;
						h = curc->size - 2;
					} else {
						x = offsetc + res_left + xo;
						y = offsett + res_top + yo;
						w = curc->size - 2;
						h = curt->size - 2;
					};
					XMoveResizeWindow(dpy,curc->win->id,(x),(y),(w),(h));
					offsetc += curc->size;
					curc = curc->next;
				};
				offsett += curt->size;
				curt = curt->next;
			};
		};
		s = s->next;
	};
}

int xerror(Display *d, XErrorEvent *e) {
	//get and print the error description for diagnostics:
	char buff[256];
	XGetErrorText(dpy, e->error_code, buff, 256);
	fprintf(stderr,"euclid-wm ERROR: X error: %s\n",buff);
	if (e->error_code == BadWindow) {
		forget_win((Window) e->resourceid);
	};
	gxerror = true;
	return(0);
}

int event_loop() {
	bool redraw;
	struct timeval last_redraw;
	last_redraw.tv_sec = 0;
	last_redraw.tv_usec = 0;
	layout();
	XEvent ev;
	for (;;) {
		redraw = false; //this will get set to true if something gets changed onscreen
		do {
			XNextEvent(dpy, &ev);
			
			//Debugging, print all events:
			/*char *events[] = {NULL, NULL, "KeyPress","KeyRelease","ButtonPress","ButtonRelease","MotionNotify","EnterNotify","LeaveNotify","FocusIn","FocusOut","KeymapNotify","Expose","GraphicsExpose","NoExpose","VisibilityNotify","CreateNotify","DestroyNotify","UnmapNotify","MapNotify","MapRequest","ReparentNotify","ConfigureNotify","ConfigureRequest","GravityNotify","ResizeRequest","CirculateNotify","CirculateRequest","PropertyNotify","SelectionClear","SelectionRequest","SelectionNotify","ColormapNotify","ClientMessage","MappingNotify","GenericEvent"};
			printf ("eventtype: %d %s\n",ev.type,events[ev.type]);
			*/
			if (ev.type == MotionNotify && sloppy_focus == true && cs->v->fs == false) {
				if (cs->v->mfocus != NULL && cs->v->mfocus->win->id != ev.xmotion.window) {
					struct cont *f = id_to_cont(ev.xmotion.window);
					if (f != NULL) {
						cs->v->mfocus = f;
							struct screen *s = firstscreen;
							while (s != NULL && s->v != f->track->view) {
								s = s->next;
							};
							cs = s;
							cs->v->mfocus = f;

						redraw = true;
					};
				}; 
			} else if (ev.type == EnterNotify && cs->v->fs == false && ev.xcrossing.focus == false && sloppy_focus == true) { 
				struct timeval ctime;
				gettimeofday(&ctime,0);
				signed long usec = ctime.tv_usec - last_redraw.tv_usec;
				signed long sec = ctime.tv_sec - last_redraw.tv_sec;
				if ( sec > 1 || (sec == 0 && usec >= 20000) || (sec == 1 && usec <= -20000)) { 
					if (cs->v->mfocus != NULL && cs->v->mfocus->win->id != ev.xcrossing.window) {
						struct cont *f = id_to_cont(ev.xmotion.window);
						if (f != NULL) {
							struct screen *s = firstscreen;
							while (s != NULL && s->v != f->track->view) {
								s = s->next;
							};
							cs = s;
							cs->v->mfocus = f;
							redraw = true;
						};
					};
				};
			} else if (ev.type == KeyPress) {
			//first find the keypress index from bindings[]
				int i = 0;
				while (i < BINDINGS && (bindings[i].keycode != ev.xkey.keycode || *(bindings[i].mask) != ev.xkey.state)) {
					i++;
				};
  
				switch (i) {
				//resize
					case 0:
						resize(4);
						redraw = true;
						break;
					case 1:
						resize(3);
						redraw = true;
						break;
					case 2:
						resize(1);
						redraw = true;
						break;
					case 3:
						resize(2);
						redraw = true;
						break;
					//move win to view 1-0 n m
					case 4:
						move_to_view(find_view(1));
						redraw = true;
						break;
					case 5:
						move_to_view(find_view(2));
						redraw = true;
						break;
					case 6:
						move_to_view(find_view(3));
						redraw = true;
						break;
					case 7:
						move_to_view(find_view(4));
						redraw = true;
						break;
					case 8:
						move_to_view(find_view(5));
						redraw = true;
						break;
					case 9:
						move_to_view(find_view(6));
						redraw = true;
						break;
					case 10:
						move_to_view(find_view(7));
						redraw = true;
						break;
					case 11:
						move_to_view(find_view(8));
						redraw = true;
						break;
					case 12:
						move_to_view(find_view(9));
						redraw = true;
						break;
					case 13:
						move_to_view(find_view(-1));
						redraw = true;
						break;
					case 14:
						move_to_view(find_view(-3));
						redraw = true;
						break;
					case 15:
						move_to_view(find_view(-2));
						redraw = true;
						break;
					//change view
					//internally views are 0 index
					//userside 1 indexed, w/0 pointing to the last
					case 16: //1 pressed, view 0
						goto_view(find_view(1));
						redraw = true;
						break;
					case 17:
						goto_view(find_view(2));
						redraw = true;
						break;
					case 18:
						goto_view(find_view(3));
						redraw = true;
						break;
					case 19:
						goto_view(find_view(4));
						redraw = true;
						break;
					case 20:
						goto_view(find_view(5));
						redraw = true;
						break;
					case 21:
						goto_view(find_view(6));
						redraw = true;
						break;
					case 22:
						goto_view(find_view(7));
						redraw = true;
						break;
					case 23:
						goto_view(find_view(8));
						redraw = true;
						break;
					case 24:
						goto_view(find_view(9));
						redraw = true;
						break;
					//goto last:
					case 25:
						goto_view(find_view(-1));
						redraw = true;
						break;
					//goto next/prev
					case 26:
						goto_view(find_view(-3));
						redraw = true;
						break;
					case 27:
						goto_view(find_view(-2));
						redraw = true;
						break;
					//shift window
					case 28:
						shift_window(4);
						redraw = true;
						break;
					case 29:
						shift_window(3);
						redraw = true;
						break;
					case 30:
						shift_window(1);
						redraw = true;
						break;
					case 31:
						shift_window(2);
						redraw = true;
						break;
					//toggle stack
					case 32:
						if (cs->v->fs == false) {
							if (cs->v->showstack == true) {
								cs->v->showstack = false;
							} else {
								cs->v->showstack = true;
							};
							redraw = true;
						};
						break;
					//move to stack
					case 33:
						if (cs->v->mfocus == NULL) {break;};
						if (cs->v->fs == true) {break;};
						XUnmapWindow(dpy,cs->v->mfocus->win->id);
						move_to_stack(cs->v->mfocus);
						redraw = true;
						break;
					//move to main
					case 34:
						if (cs->v->fs == true) {break;};
						move_to_main();
						if (cs->v->mfocus != NULL && cs->v->mfocus->win != NULL) {
							XMapWindow(dpy,cs->v->mfocus->win->id);
						};
						redraw = true;
						break;
					//flip the layout
					case 35:
						if (cs->v->fs == true) {break;};
						if (cs->v->sfocus != NULL && cs->v->mfocus != NULL) {
							struct win *m = cs->v->mfocus->win;
							struct win *s = cs->v->sfocus->win;
							cs->v->mfocus->win = s;
							cs->v->sfocus->win = m;
							XMapWindow(dpy,cs->v->mfocus->win->id);
							XUnmapWindow(dpy,cs->v->sfocus->win->id);
							XSync(dpy,False);
							redraw = true;
						};
						break;
					//swap stack up/down
					case 36:
						if (cs->v->sfocus != NULL && cs->v->sfocus->prev != NULL) {
							struct stack_item *tmpa, *tmpb, *tmpc, *tmpd;
							tmpa = cs->v->sfocus->prev->prev;
							tmpb = cs->v->sfocus->prev;
							tmpc = cs->v->sfocus;
							tmpd =cs->v->sfocus->next;
							if (tmpa != NULL) {
								tmpa->next = tmpc;
							} else {
								cs->v->stack = tmpc;
							};
							if (tmpd != NULL) {
								tmpd->prev = tmpb;
							};
							tmpc->prev = tmpa;
							tmpc->next = tmpb;
							tmpb->prev = tmpc;
							tmpb->next = tmpd;
						};
						redraw = true;
						break;
					case 37:
						if (cs->v->sfocus != NULL && cs->v->sfocus->next != NULL) {
							struct stack_item *tmpa, *tmpb, *tmpc, *tmpd;
							tmpa = cs->v->sfocus->prev;
							tmpb = cs->v->sfocus;
							tmpc = cs->v->sfocus->next;
							tmpd =cs->v->sfocus->next->next;
							if (tmpa != NULL) {
								tmpa->next = tmpc;
							} else {
								cs->v->stack = tmpc;
							};
							if (tmpd != NULL) {
								tmpd->prev = tmpb;
							};
							tmpc->prev = tmpa;
							tmpc->next = tmpb;
							tmpb->prev = tmpc;
							tmpb->next = tmpd;
						};
						redraw = true;
						break;
					//shift main focus udrl
					case 38:
						shift_main_focus(4);
						redraw = true;
						break;
					case 39:
						shift_main_focus(3);
						redraw = true;
						break;
					case 40:
						shift_main_focus(1);
						redraw = true;
						break;
					case 41:
						shift_main_focus(2);
						redraw = true;
						break;
					//shift stack focus up down
					case 42:
						shift_stack_focus(true);
						redraw = true;
						break;
					case 43:
						shift_stack_focus(false);
						redraw = true;
						break;
					//close win soft or hard
					case 44:
						if (cs->v->mfocus == NULL || cs->v->mfocus->win == NULL) {
							break;
						};
						if (cs->v->mfocus->win->del_win == true) {
							XClientMessageEvent	cm;
							memset(&cm,'\0', sizeof cm);
							cm.type = ClientMessage;
							cm.window = cs->v->mfocus->win->id;
							cm.message_type = wm_prot;
							cm.format = 32;
							cm.data.l[0] = wm_del_win;
							cm.data.l[1] = CurrentTime;
							XSendEvent(dpy, cs->v->mfocus->win->id, False, 0L, (XEvent *)&cm);
						} else {
							XDestroyWindow(dpy,cs->v->mfocus->win->id);
						};
						break;
					case 45:
						if (cs->v->mfocus != NULL && cs->v->mfocus->win != NULL) {
							XKillClient(dpy,cs->v->mfocus->win->id);
						};
						break;
					//run menu/xterm
					case 46:
						spawn(dcmd);
						break;
					case 47:
						//spawn xterm
						spawn(tcmd);
						break;
					//fullscreen:
					case 48:
						if (cs->v->fs == true) {
							cs->v->fs = false;
						} else { 
							cs->v->fs = true;
						};
						redraw = true;
						break;
					//quit
					case 49:
						return(0);
						break;
					case 50:
						if (cs->v->orientv == true) {
							cs->v->orientv = false;
						} else {
							cs->v->orientv = true;
						};
						redraw = true;
						break;

					case 51:
						spawn(ccmd01);
						break;
					case 52:
						spawn(ccmd02);
						break;

					case 53:
						spawn(ccmd03);
						break;
					case 54:
						spawn(ccmd04);
						break;
					case 55:
						spawn(ccmd05);
						break;
					case 56:
						spawn(ccmd06);
						break;
					case 57:
						spawn(ccmd07);
						break;
					case 58:
						spawn(ccmd08);
						break;
					case 59:
						spawn(ccmd09);
						break;
					case 60:
						spawn(ccmd10);
						break;
					case 61:
						//reload configs:
						//XFreeGC()
						//free commands
						if (true) {
							if (tcmd != NULL) {
								free(tcmd);
							};
							if (dcmd != NULL) {
								free(dcmd);
							};
							if (ccmd01 != NULL) {
								free(ccmd01);
							};
							if (ccmd02 != NULL) {
								free(ccmd02);
							};
							if (ccmd03 != NULL) {
								free(ccmd03);
							};
							if (ccmd04 != NULL) {
								free(ccmd04);
							};
							if (ccmd05 != NULL) {
								free(ccmd05);
							};
							if (ccmd06 != NULL) {
								free(ccmd06);
							};
							if (ccmd07 != NULL) {
								free(ccmd07);
							};
							if (ccmd08 != NULL) {
								free(ccmd08);
							};
							if (ccmd09 != NULL) {
								free(ccmd09);
							};
							if (ccmd10 != NULL) {
								free(ccmd10);
							};

							//Unbind keys
							int i = 0;
							while (i < BINDINGS ) {
								if (bindings[i].mask != NULL) {
									XUngrabKey(dpy,bindings[i].keycode,*(bindings[i].mask),root);
								};
								i++;
							};
							//call bind keys
							load_defaults();
							//call load_config
							load_conf();
							commit_bindings();
							XFreeGC(dpy,focus_gc);
							XFreeGC(dpy,unfocus_gc);
							XGCValues xgcv;
							xgcv.foreground = stack_focus_pix;
							focus_gc = XCreateGC(dpy,cs->stackid,GCForeground,&xgcv);
							xgcv.foreground = stack_unfocus_pix;
							unfocus_gc = XCreateGC(dpy,cs->stackid,GCForeground,&xgcv);
						};
					case 62:
						//move to previous screen
						if (cs != firstscreen) {
							struct screen *s = firstscreen;
							while (s->next != cs) {
								s = s->next;
							};
							cs = s;
							redraw = true;
						};
						break;
					case 63:
						//move to next screen
						if (cs->next != NULL) {
							cs = cs->next;
							redraw = true;
						};
						break;
				};
	
			} else if (ev.type == ReparentNotify) {
				if (ev.xreparent.parent == root) {
					if (is_top_level(ev.xreparent.window) == true) {
						struct win *t;
						t = add_win(ev.xreparent.window);
						XWindowAttributes att;
						XGetWindowAttributes (dpy,ev.xreparent.window,&att);
						if (att.map_state != IsUnmapped) {
							add_client_to_view(t,cs->v);
							redraw = true;
						};
					};
				} else {
					forget_win(ev.xreparent.window);
				};
		
			} else if (ev.type == ClientMessage) {
				if (ev.xclient.message_type == wm_change_state) {
					if (ev.xclient.data.l[1] == wm_fullscreen) {
						struct cont *wc = id_to_cont(ev.xclient.window);
						if (wc != NULL) {
							if (ev.xclient.data.l[0] == 1) { //go into full screen
								wc->track->view->fs = true;
								redraw = true;
								wc->win->fullscreen = true;
								XChangeProperty(dpy,ev.xclient.window,wm_change_state,XA_ATOM,32,PropModeReplace,(unsigned char *)&wm_fullscreen,1);
							} else { // exit fullscreen
								wc->track->view->fs = false;
								redraw = true;
								wc->win->fullscreen = false;
								XChangeProperty(dpy,ev.xclient.window,wm_change_state,XA_ATOM,32,PropModeReplace,(unsigned char *)0,0);
							};
						};
					};
				};
			} else if (ev.type == DestroyNotify ) {
				forget_win(ev.xdestroywindow.window);
			} else if (ev.type == MapNotify && is_top_level(ev.xmap.window) == true) {
				//check whether it's in the layout, if not add it
				if (id_to_cont(ev.xmap.window) == NULL) {
					//see whether we know about the window
					if (is_top_level(ev.xmap.window)) {
						struct win *w;
						w = first_win;
						while (w != NULL && w->id != ev.xmap.window) {
							w = w->next;
						};
						if (w == NULL) { //we don't have a record of it
							w = add_win(ev.xmap.window);
						};
						//finally add to layout
						add_client_to_view(w,cs->v);
						redraw = true;
					};
				};
			} else if (ev.type == UnmapNotify ) {
				struct cont *s;
				s = id_to_cont(ev.xunmap.window);
				if (s != NULL ) {
					
					if (s->track->view == cs->v) {
						cs->v->fs = false;
					};
					remove_cont(s);
					//unless we caused this, we should check the window's original state 
					//before setting this
					redraw = true;
				};
			} else if (ev.type == CreateNotify && is_top_level(ev.xcreatewindow.window) ==true) {
				add_win(ev.xcreatewindow.window);
			} else if (ev.type == ConfigureNotify) {
				//if a window tries to manage itself we are going to play rough, unless it is putting itself in or out of fullscreen
				struct cont *wc = id_to_cont(ev.xconfigure.window);
				if (wc != NULL) {
					if (wc->track->view == cs->v) {
						if (ev.xconfigure.width >= cs->w  && ev.xconfigure.height >= cs->h) {
							if (cs->v->fs == false) { //if we get this request when we are already in fullscreen just ignore it, do NOT fall through to one of the last two elses
								cs->v->fs = true;
								cs->v->mfocus = wc;
								redraw = true;
							};
						} else if (cs->v->fs == true && wc == cs->v->mfocus && (ev.xconfigure.height < cs->h || ev.xconfigure.width < cs->w)) {
							cs->v->fs = false;
							redraw = true; 
						
						//when we configure a window we set its w and height to -2 the w and height of the cont on screen to allow for a border, shouldn't we be checking that here?
						} else if (wc->track->view->orientv == true) {
							//we are going to be niave and just check the w and h
							//w =track size
							if (wc->track->size - 2 != ev.xconfigure.width || wc->size - 2 != ev.xconfigure.height) {
								redraw = true;
							};
						} else {
							if (wc->track->size - 2 != ev.xconfigure.height || wc->size - 2 != ev.xconfigure.width) {
								redraw = true;
							};
						};
					};
				};
			};
	
		} while (XPending(dpy)); 
	
		if (redraw == true) {
			layout();
			gettimeofday(&last_redraw,0);
		};
	}; //end infinite loop
}

int main() {
	printf("\neuclid-wm: running\n");
	
	dpy = XOpenDisplay(0);
	XSetErrorHandler(xerror);
	//	cs->h = DisplayHeight(dpy,DefaultScreen(dpy));
//	cs->w = DisplayWidth(dpy,DefaultScreen(dpy));
//	printf("euclid-wm: sreen dimensions: %d %d\n",cs->h, cs->w);
	//set some stuff
	if ((root = DefaultRootWindow(dpy))) {
		printf("euclid-wm: root is %6.0lx\n",root);
	} else {
		fprintf(stderr,"euclid-wm ERROR: faild to find root window\n");
		return(0);
	};
	//set colors, these get overridden by the config file
	XColor color;
	//these values are shorts, 0 - 65535
	//focus color:
	color.red = 0;
	color.green = 0;
	color.blue = 65500;
	color.flags = DoRed | DoGreen | DoBlue;
	XAllocColor(dpy,DefaultColormap(dpy,DefaultScreen(dpy)),&color);
	focus_pix = color.pixel;
	//unfocus color
	color.red = 5000;
	color.green = 5000;
	color.blue = 5000;
	color.flags = DoRed | DoGreen | DoBlue;
	XAllocColor(dpy,DefaultColormap(dpy,DefaultScreen(dpy)),&color);
	unfocus_pix = color.pixel;
	//stack background:
	color.red = 100;
	color.green = 100;
	color.blue = 200;
	color.flags = DoRed | DoGreen | DoBlue;
	XAllocColor(dpy,DefaultColormap(dpy,DefaultScreen(dpy)),&color);
	stack_background_pix = color.pixel;
	//stack unfocus text:
	color.red = 60000;
	color.green = 60000;
	color.blue = 60000;
	color.flags = DoRed | DoGreen | DoBlue;
	XAllocColor(dpy,DefaultColormap(dpy,DefaultScreen(dpy)),&color);
	stack_unfocus_pix = color.pixel;
	//stack focus text:
	color.red = 0;
	color.green = 0;
	color.blue = 65500;
	color.flags = DoRed | DoGreen | DoBlue;
	XAllocColor(dpy,DefaultColormap(dpy,DefaultScreen(dpy)),&color);
	stack_focus_pix = color.pixel;
	
	//we have to do this after we get root
	memset(bindings, '\0', sizeof(bindings));
	load_conf();
	commit_bindings();

	
	set_atoms();
	int screens;
	
//DEBUGGING MULTISCREEN (on a single head setup)
	//comment out the next two lines, 
	//and the XFree(scrn_info) that follows screen setup
	//uncomment the following block

	XineramaScreenInfo *scrn_info = NULL; 
	scrn_info = XineramaQueryScreens(dpy,&screens);
	
/*	XineramaScreenInfo scrn_info[3];
	scrn_info[0].height = 400;
	scrn_info[0].width = 450;
	scrn_info[0].x_org = 5;
	scrn_info[0].y_org = 5;
	scrn_info[1].height = 300;
	scrn_info[1].width = 250;
	scrn_info[1].x_org = 200;
	scrn_info[1].y_org = 410;
	scrn_info[2].height = 500;
	scrn_info[2].width = 400;
	scrn_info[2].x_org = 650;
	scrn_info[2].y_org = 200;

	screens = 3;
*/
	printf("screens %d\n",screens);
	unsigned short sn = 0;
	if (screens == 0) {
		printf("Xinerama diabled\n");
		addscreen( DisplayHeight(dpy,DefaultScreen(dpy)),DisplayWidth(dpy,DefaultScreen(dpy)),0,0,0);

	} else {
		printf("Xinerama enabled: %d screens\n",screens);
		while (sn < screens) {
			addscreen(scrn_info[sn].height,scrn_info[sn].width,scrn_info[sn].x_org,scrn_info[sn].y_org,sn);
			sn ++;
		};
	};
//Comment this out too when debugging multiscreen 
	XFree(scrn_info);

	offscreen = DisplayHeight(dpy,DefaultScreen(dpy));
//	cs->v = make_view();
//	fv = cs->v;
//	cs->v->idx = 1;

	//now we also need to get all already exisiting windows
	Window d1, d2, *wins = NULL;
	unsigned int no;
	int i;
	XQueryTree(dpy,root,&d1,&d2,&wins,&no);
	struct win *t;
	printf("euclid-wm: %d windows\n",no);
	for (i = 0 ; i<no; i++) {
		if (is_top_level(wins[i])) {
			t = add_win(wins[i]);
			XWindowAttributes att;
			XGetWindowAttributes(dpy,wins[i],&att);
			if (att.map_state != IsUnmapped) {
				add_client_to_view(t,cs->v);
			};
			//to see how to check for iconified windows, look at scrotwm getstate(
		};
	};
	
	//and set an event on root to get new ones
	if  (XSelectInput(dpy,root,SubstructureNotifyMask )) {
		printf("euclid-wm: now controlling root\n");
	} else {
		fprintf(stderr,"euclid-wm ERROR: fail to control root, is there already a WM?");
		return(1);
	};
	XSync(dpy,False);
	
	
	XGCValues xgcv;
	xgcv.foreground = stack_focus_pix;
	focus_gc = XCreateGC(dpy,cs->stackid,GCForeground,&xgcv);
	xgcv.foreground = stack_unfocus_pix;
	unfocus_gc = XCreateGC(dpy,cs->stackid,GCForeground,&xgcv);

	layout();
	
	return (event_loop());
}
