# Basic Testdrive #

  * First get the latest source:

```
svn checkout http://euclid-wm.googlecode.com/svn/trunk/ euclid-wm-read-only
```

  * cd into the trunk directory

Note: you will need to have the Xlib and Xinerama headers installed prior to compiling. On Debian you can get them by installing libx11-dev and libxinerama-dev.

  * Compile:
```
make clean
make
```

  * Install:
```
su
make install
suspend

```

  * Read the man page **before** trying it out:
```
man euclid-wm
```

> Note: By default euclid-wm uses xterm and dmenu as the terminal and menu respectively; running euclid-wm without having either installed them or reconfigured euclid-wm to use alternatives gives a substandard user experience.

  * Give it a go: Log out and log in using the euclid session.

# Debugging #
Debugging a window manager poses its own set of problems, here is the best way I've found to do it:

  * Checkout as above

  * Compile with:
```
cc -O0 -g -lX11 -lXinerama -oeuclid-wm euclid-wm.c
```

  * Start a new virtual terminal (if you are doing this several times, starting the doing this from within the directory where you compiled will save you some typing when you in the next step):
```
su
xinit -- :1 vt12
```

  * You should be in a new vt with a single xterm. In the xterm, cd to the directory where you compiled. Optionally switch from root. Then
```
screen gdb euclid-wm
```

  * Start euclid, within gdm:
```
r
```

  * Then `<ctrl> + a, d` to detach screen

  * Switch back to the original vt `<ctrl> + <alt> + <F7>`

  * Attach to the screen session:
```
screen -r
```

  * Go back to vt12 and do what you want to do, `<ctrl + <alt> + <F12>`