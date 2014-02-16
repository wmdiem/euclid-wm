#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdbool.h>
#include <string.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xatom.h>
#include <errno.h>
#include <signal.h>
#include <sys/types.h>
#include <dirent.h>
#include <sys/stat.h>
#include <poll.h>

#include <X11/extensions/Xinerama.h>

#define MAX_RESULTS 2000
#define MAX_HANDLERS 20

Display *dpy;
Window root;
unsigned long focus_pix;
unsigned long unfocus_pix;
unsigned long background_pix;
//etc. 
Window wid;
char buf[128];
short unsigned int pos = 0;
int sxo, syo, sh, sw, sn;
int fh;
GC focus_gc = NULL;
GC unfocus_gc = NULL;
XIC xic;
XFontSet xfs;
char *handler = NULL;
int nu_results;
char *results[MAX_RESULTS];
size_t result_size[MAX_RESULTS];
int results_cmdchar[MAX_RESULTS];
int h_opt = -1;
char **handler_dir;
char *handlers[MAX_HANDLERS];
char *handlers_trimmed[MAX_HANDLERS];
FILE *fin = NULL;
FILE *fout = NULL;

int xerror(Display *d, XErrorEvent *e) {
        //get and print the error description for diagnostics:
        char buff[256];
        XGetErrorText(dpy, e->error_code, buff, 256);
        fprintf(stderr,"slaunch ERROR: X error: %s\n",buff);
	//if the error is badwindow die, otherwise we keep the keyboard 
        return(0);
}

void spawn(char *cmd) { 
	if (cmd == NULL || cmd[0] == '\0') {
                return;
        };
	setsid(); //need this?
	char cmd2[512];
	strcpy (&cmd2[0],"exec ");
	strcpy (&cmd2[5],cmd);
	cmd2[strlen (cmd2)+1] = '\0';
	execl("/bin/sh","/bin/sh","-c",cmd2,NULL);
	fprintf(stderr,"error number %d  spawning %s\n",errno,cmd);
	exit(1);
         
}


void draw_win ()  {
	int x = sxo;
	int y = syo;
	int ih = fh + 5;
	int h = (((nu_results+1) * (ih)) >  (sh / 2)) ? (sh / 2) : ((nu_results+1) * (ih)) ;
	int w = sw;
	int lo = 0; //line offset
	XClearWindow(dpy,wid);

	XMoveResizeWindow(dpy,wid,x,y,w-2,h);
	XRaiseWindow(dpy,wid);
	XSync(dpy,false);//important!
	GC gc;
	gc = focus_gc;
	char *str = NULL;
	if (h_opt == -1) {
		str = buf;
	} else { 
		str = results[h_opt]+results_cmdchar[h_opt];
	}; 
	lo += fh;
	lo += 2;

	int cx = 0;
	XDrawString(dpy,wid,gc,3,lo,str, strlen(str));
	if (h_opt != -1) {//but the cursor at the end of the displayed option, leaving pos unchanged
		cx = XmbTextEscapement(xfs,str,strlen(str));

	} else { //draw it at pos	
		cx = XmbTextEscapement(xfs,str,pos);
	};
	XDrawLine(dpy,wid,focus_gc,cx+2,2,cx+2,fh+3);
		//write results here

	//we need to know how many results we can display, so we can figure out where to stawt drawing
	int items = h / ih; //ih is the height of each item
	items--; //at this point items is really "max items" we can display
	int ro = 0; //results offset
	if (items > nu_results) {
		ro = 0; //they all fiti: ro is beasicaly how far down the list of results we start displaying
	} else if (h_opt < items / 2) { //h_opt is the highlighted option/result
		ro = 0;
	} else if (nu_results - h_opt > items / 2) {
		ro = h_opt - items / 2;
	} else {
		ro = nu_results - items;
	};
		
	items = items < nu_results ? items : nu_results;
	

	int i = 0;
	//printf("%s\t%d\t%d\t%d\t%d\t%d\t%d\t%d\n",str,h_opt,nu_results,i,lo,h,ro,items);
	while (i < items )  {
		lo += fh;
		lo += 5;
		gc = h_opt != i+ro ? unfocus_gc : focus_gc;
		XDrawString(dpy,wid,gc,3,lo,results[i+ro],strlen(results[i+ro]));
		//printf("%s",results[i+ro]);
		i++;
	};
	XSync(dpy,false);

};

inline char* trim_name(char* file)  {
	char * ret[32];
	int i = 0;
	while (file[i] != '\0' && file[i] != '.' && i < 32) {

		ret[i] = file[i];
		i++;
	};
	ret[i < 32 ? i : 32] = '\0';
	// printf("%s\n",ret);
	return(ret);

};
void load_handlers() {
	//find xdg_config_home
	/*char *xdgconf = getenv("XDG_CONFIG_HOME");
	
	if (xdgconf) {
		handler_dir = malloc(strlen(xdgconf) + strlen("/euclid-menu/handlers/") + 1);
		handler_dir[0] = '\0';
		strcpy(handler_dir,xdgconf);
		strcat(handler_dir,"/euclid-menu/handlers/");
	} else {
		char *home = getenv("HOME");
		handler_dir = (char *) malloc(strlen(home) + strlen("/.config/euclid-menu/handlers/") + 1);
		handler_dir[0] = '\0';
		strcpy(handler_dir,home);
		strcat(handler_dir,"/.config/euclid-menu/handlers/");
	};
	*/
	handler_dir = (char *) malloc(strlen("/usr/share/euclid-menu/handlers/") + 1);
	handler_dir[0] = '\0';
	strcpy(handler_dir,"/usr/share/euclid-menu/handlers/");
	if (handlers[0] == NULL) {
		DIR *dir;
		struct dirent *de;
		struct stat stt;
		dir = opendir(handler_dir);
		if (!dir) {
			//printf("shit\n");
		};
		chdir(handler_dir);
		int i = 0;
		do {
			//do stuff
			de = NULL;
			de = readdir(dir);
			if (de) {
				stat(de->d_name,&stt);
				//printf("Name %s permissions %lo\n", de->d_name,(unsigned long) stt.st_mode); 
				if (stt.st_mode & S_IXUSR && de->d_name[0] != '.') {
					handlers[i]  = strdup(de->d_name);
					handlers_trimmed[i] = strdup(de->d_name);
					int j = 0;
					while (handlers_trimmed[i][j] != '\0' && handlers_trimmed[i][j] != '.') {
						handlers_trimmed[i][j] = handlers[i][j]; 
						j++;
					};
 					handlers_trimmed[i][j] = '\0';
					i++;
				};
			};
			if (i < MAX_HANDLERS) {
				handlers[i] = NULL;
				handlers_trimmed[i] = NULL;
			};
			//save name in araray
		}	while (de && i < MAX_HANDLERS) ;
	};

	//read all files
	//store the names of all that are executable in a linked list
	chdir(getenv("HOME")); //so when we run a program it isn't in a config dir. 
};

inline char* find_handler() {
	//read buf use an aray to pick the appropriate file handler and return the command
//what is here now is a bit simple, we need to also check the end of the line to see whether the returned command is prompting for another handler (e.g., if the command takes a filename or a URL as an argument
	if (buf[0] == '!') {
		if (buf[1] != '\0') {
			//find match:
			int i = 0;
			while (i < MAX_HANDLERS && handlers[i] != NULL) {
	
				if (!strcmp(&buf[1],handlers_trimmed[i])){
					return(handlers[i]);
				};
				i++;
			};
		} else {
			//we set the options to a list of handlers, and return null

			//char *ret = (char *) malloc(strlen("echo -e \"\!c\tcalculate\\n\!s\tshell command\"")+1);
			//strcpy(ret,"echo \"\!c calculate\n\!s shell command\"");
			//use realloc to make sure there is roome
			//must update the bytesize for each changed option
			//must also update nu_return
			//results, nu_results, result_size
			int i = 0;
			while (i < MAX_HANDLERS && handlers[i] != NULL) {
				//check the size of the allocated memory? 
				//printf("\t%s\n",trim_name(handlers[i]));
				if (results[i] == NULL) {
					results[i] = (char *) malloc(strlen(handlers_trimmed[i] + 5));
				};
				if (strlen(handlers_trimmed[i]) +1 <= strlen(results[i])) {
					strcpy(results[i],"!");
					strcat(results[i],handlers_trimmed[i]);
					
				} else { 
					results[i] = realloc(results[i],strlen(handlers_trimmed[i]) + 1);
					
					result_size[i] = strlen(handlers_trimmed[i]) + 1;
					strcpy(results[i],"!");
					strcat(results[i],handlers_trimmed[i]);
				};
				//printf("%s\n",results[i]);
				i++;
			};
			nu_results = i;
			//we need to close the pipes
			if (fin) {
				fclose(fin);
				fin = NULL;
			};
			if (fout) {
				fclose(fout);
				fout = NULL;
			};
		//instead of returning a string, we want to return a pair of pipes, one the stdin for the hanlder, the other it's standard out
		//is this what we want here?	
			return(NULL);
			
		};
	} else { //default handerl
		//char *ret = (char *) malloc(strlen("ls /usr/bin")+1);
		//strcpy(ret,"ls /usr/bin");
		int i = 0;
		while (i < MAX_HANDLERS && handlers[i] != NULL && strcmp(handlers_trimmed[i],"default")) {
			i++;
		};	
		if (!handlers[i]) {
			nu_results = 0;
			h_opt = -1;
		};
		return(handlers[i]); //could be null;
	};
};


void setup_pipes(char *cmd) {
	//sets up a pair of pipes for bidirectional communicaiton with handler, and forks, in the child it then uses sets its STDIN and STDOUT to the pipes and execs the handler
	static int pid;
	int pipes[2][2];
	
	if (fin) {
		fclose(fin);
		kill(pid,15);
	};
	if (fout) {
		fclose(fout);
		kill(pid,15);
	};

	if (cmd) {
	//cat the handler to handler_dir
	//we really out to do this only once and store it
	char *cmd2 = (char*) malloc(strlen(cmd) + strlen(handler_dir) + strlen(buf) + 5);
	strcpy(cmd2,handler_dir);
	strcat(cmd2,cmd);
/*	strcat(cmd2," ");
	// have to strip the handler name from the front of buf
		if (buf[0] == '!') {
			int i = 0; 
			while (buf[i] != '\0' && buf[i] != ' ') {
				i++;
			};
			strcat(cmd2,&buf[i]);
		} else {
			strcat(cmd2,buf);
		};
*/
	//printf("Getting options from %s\n",cmd2);
	
		pipe(pipes[0]);
		pipe(pipes[1]);
	
		pid = fork();
	
		if (pid==0) { //child 
			if(dpy) {
				close(ConnectionNumber(dpy));
			};
			close(pipes[0][0]);
			close(pipes[1][1]);
			dup2(pipes[0][1],STDOUT_FILENO);
			dup2(pipes[1][0],STDIN_FILENO);
			setvbuf(stdout,NULL,_IONBF,0);
	
			execl("/bin/sh","/bin/sh","-c",cmd2,NULL);

		} else { //parent

			close(pipes[0][1]);
			close(pipes[1][0]);
			fin = fdopen(pipes[0][0],"r");
			fout = fdopen(pipes[1][1],"w");
			setvbuf(fout,NULL,_IONBF,0);
			free(cmd2);
		};
	};
}


void update_options() {
//	char rbuf[256];
//	memset(rbuf,'\0',sizeof(rbuf));
//	nu_results = 0;
/*	if (cmd) {
		//cat the handler to handler_dir
		//we really out to do this only once and store it
		char *cmd2 = (char*) malloc(strlen(cmd) + strlen(handler_dir) + strlen(buf) + 5);
		strcpy(cmd2,handler_dir);
		strcat(cmd2,cmd);
		strcat(cmd2," ");
		// have to strip the handler name from the front of buf
		if (buf[0] == '!') {
			int i = 0; 
			while (buf[i] != '\0' && buf[i] != ' ') {
				i++;
			};
			strcat(cmd2,&buf[i]);
		} else {
			strcat(cmd2,buf);
		};
		//printf("Getting options from %s\n",cmd2);


		FILE *ret;
		//must add the buffer to the end of cmd
		ret = (FILE *) popen(cmd2,"r");
		if (ret) { */
			//char *l = NULL;
			//size_t n = 0;
		
		//print buf to fout
//		fflush(fin); //make sure any cruft left over from last time is out
		if (fout) {
			//printf("out is open\n");
			int i = 1;
			if (buf[0] == '!') { //trim handler name
				while (buf[i] != '\0' && buf[i-1] !=' ') {
					i++;
				};
			} else {
				i = 0;
			};
			//if (buf[i] == '\0') return;	
			fprintf(fout,"%s\n",&buf[i]);
		
			if(fin) {
				//printf("checking\n");
				i = 0;
				bool break_next = false;
				
				while (i < MAX_RESULTS  &&  getline(&results[i],&result_size[i],fin) != -1) { 
					
					//printf("-> %s",results[i]);
					//sanitize results now
					results_cmdchar[i] = 0;
					int j = 0;
					int k = 0;
					while (results[i][j] != '\0') {
						if(results[i][j] == '\t'){
							//copy a '\0' to that position, record the address of the next byte in another array so we can get to the command
							results[i][k] = '\0';
							results_cmdchar[i] = k + 1;
							k++;
						} else if (!iscntrl(results[i][j])) {
							results[i][k] = results[i][j];
							k++;
						};
						j++;
					};
					results[i][k] = '\0';
					//i++;
					if (*results[i]) { //write over empty lines and break if we get two consecutive empty lines
						i++;
						break_next = false;
					} else {
						if (break_next) break;
						break_next = true;
					};
				};
				nu_results = i; //+1;	
				//printf("%d\n",i);
			//	fclose(ret);
			//	free(cmd2);
			};
		};
//	};
}

char* loop () {
	XEvent ev;
	KeySym key;
	Status stat;
	char txt[128];
	char *t_exec = NULL;
	int len = 0;
	while (!t_exec) {
		bool draw = false;
		bool check = false;
		do {
		XNextEvent(dpy, &ev);
			//all we care about are keypresses
			if (ev.type == KeyPress) {
				key = NoSymbol;
				len = XmbLookupString(xic,&ev.xkey,txt, sizeof(txt),&key,&stat);
			//XLookupKeysym(); //big long mess
		//if the keypress is escape we leave, i.e., return NULL
		//if it is home, end, right, left we modify position (but if pos is already the end of buff, and it is right we set buff == cur_option->exec
		//if it is del of backspace, we modify buf and pos
		//if it is a printable character we insert it into buf at pos
		//if it is enter we set t_exec to the currenetly selected option, if any, else buf
		//if it is up or down we select option
		//if buffer was changed, we call a function to determine the handler, spawn handler as a pipe and update options
		//if there was any keypress we always end with a call of draw_win() unless, we already returned
		//if buf[1or buf[0]] gets touched free the handler
				if (key == XK_Escape) {
					//printf("ending\n");
					return NULL;
				} else if (key == XK_Left) {
					if (h_opt == -1 &&  pos > 0) {
						pos --;
					} else {
						h_opt = -1;
					};
					draw = true;
				} else if (key == XK_Right) { 
					if (h_opt == -1 && buf[pos] != '\0') {
						pos ++;
					} else {
						h_opt = -1;
					};
					draw = true;
				} else if (key == XK_BackSpace) { 
						if (h_opt != -1) {
							h_opt=-1;
							draw = true;
						} else if (pos > 0) {
						pos--;
						int t = pos;
						h_opt = -1;
					//buf[pos]='\0'; //too easy, have to copy whaterver is past pos, set it to \0 then cat it back	
						while (t > 0 && buf[t] != '\0') {
							buf[t] = buf[t+1];
							t++;
						};
						buf[t] = '\0';
			//			if (pos < 2 && handler) {
			//				handler = NULL;
			//			};
						draw = true;
						check = true;
					};
				} else if (key == XK_Delete && buf[pos] != '\0') {
					int t = pos;
					h_opt = -1;
					while (buf[t] != '\0') {
						buf[t] = buf[t+1] ;
						t++;
					}; 
			//		if (pos < 2 && handler) {
			//			//free(handler);
			//			handler = NULL;
			//		};
	
					draw = true;
					check = true;
				} else if (key == XK_Up) {
					if (h_opt > -1) {
						h_opt--;
					} else {
						h_opt = nu_results -1;
					};
					draw = true;
				} else if (key == XK_Down) { 
					if ( h_opt < (nu_results - 1)) {
						h_opt++;
					}else {
						h_opt = -1;
					};
					draw = true;
					
				} else if (key == XK_Home) {
					if (h_opt == -1) {
						pos = 0;
					} else { 
						h_opt = -1;
					};
					draw = true;
				} else if (key == XK_End) {
					if (h_opt == -1) {
						pos = strlen(buf) + 1;
					} else {
						h_opt = nu_results -1;
					};
					draw = true;
				} else if (key == XK_Tab) {
					if (h_opt == -1) {
						h_opt = 0;
					};
					
					strcpy(buf,results[h_opt]+results_cmdchar[h_opt]);
					pos = strlen(buf);
					h_opt = -1;
						
				//	};
					check = true;
					draw = true;
						
				} else if (key == XK_Return) {
					if (nu_results) {
						if (h_opt == -1) {
							t_exec = results[0]+results_cmdchar[0];
						} else {
							t_exec = results[h_opt]+results_cmdchar[h_opt];
						};
					} else { 
						t_exec = buf;
					};
					return(t_exec); 
						
				} else if (!iscntrl(*txt) ) {
	
				//	printf("%c\n",key);
					//if (h_opt != -1) {
					//	strcpy(buf,results[h_opt]);
					//	pos = strlen(buf);
				//	};
					int t = pos;
					h_opt = -1;
					char c = txt[0];
					char tc = buf[t];
					while (c != '\0') { 
						tc = buf[t];
						buf[t] = c;
						c = tc;
						t++;
					};
					buf[t] = '\0';
		//			if (pos < 2 ) {
						//free(handler);
		//				handler = NULL;
		//			};
	
					pos++;
					buf[t+1] = '\0';
					draw = true;
					check = true;
				};	
	
			};
		//printf("handler %s\n",handler);
			
		if (check) {
			if (handler != NULL) {
			//determine whether the handler has been changed
				if (buf[0] == '\0' && strcmp(handler,"default")) {
					//handler should be default, but isn't
					handler = NULL;
				} else if (buf[0] == '!' && buf[1] == '\0' ) {
					handler = NULL;
				} else if (buf[0] == '!') {
					int i = 0;
					while (buf[i+1] != '\0' && handler[i] != '\0') {
						if (buf[i+1] != handler[i] && buf[i+1] != ' ') {
							handler = NULL; 
							break;
						};
						i++;
					};
				};
			};

			if (!handler) {
				handler = find_handler();
				
				if (handler) {
					setup_pipes(handler);
				};
			};
			update_options();
		};
		if (draw) {
			draw_win();
		};
		} while (XPending(dpy));
	};	
	return (t_exec);
};

void clean_up() {
	XFreeFontSet(dpy,xfs);
	//ungrab keyboard,
	XUngrabKeyboard(dpy,CurrentTime);
	// Give focus to root!!!
	XSetInputFocus(dpy,root,None,CurrentTime);
	XFreeGC(dpy,focus_gc); //etc

	XDestroyIC(xic);		

	XDestroyWindow(dpy,wid);
	XCloseDisplay(dpy);
}

int main (int argc, char *argv[] ) {
	//this is to avoid leaving zombies
        //sighandler_t is a gnu extention
        signal (SIGCHLD, SIG_IGN);

        dpy = XOpenDisplay(0);
        XSetErrorHandler(xerror);
	root = DefaultRootWindow(dpy);

	XColor color;
	color.red = 100;
        color.green = 65500;
        color.blue = 100;
        color.flags = DoRed | DoGreen | DoBlue;
        XAllocColor(dpy,DefaultColormap(dpy,DefaultScreen(dpy)),&color);
        focus_pix = color.pixel;

	color.red = 50000;
        color.green = 50000;
        color.blue = 65500;
        color.flags = DoRed | DoGreen | DoBlue;
        XAllocColor(dpy,DefaultColormap(dpy,DefaultScreen(dpy)),&color);
        unfocus_pix = color.pixel;

        color.red = 100;
        color.green = 100;
        color.blue = 200;
        color.flags = DoRed | DoGreen | DoBlue;
        XAllocColor(dpy,DefaultColormap(dpy,DefaultScreen(dpy)),&color);
        background_pix = color.pixel;

	XGCValues xgcv;
	xgcv.foreground = focus_pix;
	focus_gc = XCreateGC(dpy,root,GCForeground,&xgcv);
	
	xgcv.foreground = unfocus_pix;
	unfocus_gc = XCreateGC(dpy,root,GCForeground,&xgcv);

	char **missing;
	char *def;
	int nmiss;
	xfs = XCreateFontSet(dpy,"fixed",&missing,&nmiss,&def);
	if (missing) {
		XFreeStringList(missing);
	};
	XFontStruct **xfst;
	char **names;
	XFontsOfFontSet(xfs,&xfst,&names);
	fh = xfst[0]->ascent;
	//look at load_conf and set_atoms

	sh = DisplayHeight(dpy,DefaultScreen(dpy));
	sw = DisplayWidth(dpy,DefaultScreen(dpy));
	sxo = 0;
	syo = 0;
	sn = 0;


	wid = XCreateSimpleWindow(dpy,root,sxo,syo,sw-2,20,1,unfocus_pix,background_pix);
        XSetWindowAttributes att;
        att.override_redirect = true;
        XChangeWindowAttributes(dpy,wid,CWOverrideRedirect,&att);
        XMapWindow(dpy,wid);
	XSetInputFocus(dpy,wid,None,CurrentTime);
        XSync(dpy,False);

	//grab keyboard
	int i = 0;
	while (i < 1000 && XGrabKeyboard(dpy,wid,GrabModeAsync,True,GrabModeAsync,CurrentTime) != GrabSuccess) {
		i++;
		usleep(1000);
	};
	XIM xim = XOpenIM(dpy,NULL,NULL,NULL);
        //if (!xim) die("No X input method could be opened\n");
        xic = XCreateIC(xim,XNInputStyle, XIMPreeditNothing | XIMStatusNothing,XNClientWindow, wid, XNFocusWindow, wid, NULL);

	buf[0] = '\0';	
	nu_results = 0;

	bool spawn_exec = true;
	handlers[0] = NULL;
	if (argc >= 2) {
	//printf("checking args\n");
	//printf("1%s\n2%s",argv[1],argv[2]);
		int i = 1;
		while (i < argc) {
			
			if (!strcmp(argv[i],"-l")) {
				//printf("locking specified\n");
				handlers[0] = strdup(argv[i+1]);
				handlers[1] = NULL;
				handlers_trimmed[0] = strdup("default");
				handlers_trimmed[1] = NULL;
				handler = strdup("default");
				i++;
				//printf("handler set to %s",handlers[0]);
			} else if (!strcmp(argv[i],"-r")) {
				spawn_exec=false;
			} else {

				printf("euclid-menu: an extensible interactive menu\nUSAGE:\neuclid-menu -l [handler [handler arguments]] [-r] \n\t-l <handler> lock to the specified handler. Handler must be the name of a file in in the handler folder. \n\t-r Don\'t try to execute final option, just print it.\n");
				return(0); 
			};
			i++;
		};
	};	
	load_handlers();
	handler = find_handler();
	if (handler) {
		setup_pipes(handler);
	};
	update_options();
	draw_win();
		
	
	char *exec = loop();
	
	clean_up();
	
	if (spawn_exec == true && exec) {
		//spawn(exec);	
		//make sure we trim any handler from the front if not already done
		fprintf(fout,"exec %s\n",exec);
	} else if (exec)  {
		printf("%s",exec);
	} else {
		return 0;
	};

};
