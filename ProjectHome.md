**STATUS:**
The current stable release is 0.4.2 and can be downloaded [here](https://code.google.com/p/euclid-wm/downloads/list).

If you need help with the tarball see [the instructions](http://code.google.com/p/euclid-wm/wiki/stable_install).

If you are interested in helping with development, check out [our development instructions](https://code.google.com/p/euclid-wm/wiki/dev_install)

Also please note that euclid is moving to [a new home](http://euclid-wm.sourceforge.net/index.php), please consider updating any links you have.

The following is for purely historical interest.

---


euclid-wm is a minimalist, tiling window manager for X11. It is designed to allow quick and easy management of numerous windows entirely from easy-to-learn, vim-like key-bindings.

Although there is a [plethora](http://wiki.archlinux.org/index.php/Comparison_of_Tiling_Window_Managers) of other tiling WMs, euclid-wm seeks to do two things in particular:

  * balance  the ease of use common among automatic-layout tiling window manager with the flexibility of manual layout WMs.

  * create a useful way to handle minimized applications.

Some features:
  * small,
```
$ ps -o comm,vsize,rss,trs,drs,cputime -C euclid-wm
COMMAND            VSZ   RSS  TRS   DRS     TIME
euclid-wm         3184  1096   25  3158 00:00:00

```
(as of 0.1.0).
  * completely keyboard controlled, no need to reach for the mouse . . . ever.
  * a per view/workspace stack that manages minimized/unmapped windows.
  * handles fullscreen for most apps, (at the moment flash is an exception, but there is an easy workaround)
  * simple (non-XML) configuration file
  * easy manual layout (position and resize windows with vim-style keybindings) that enforces no-gap tiling (windows are automatically resized to fill gaps or empty cells)
  * no floating layer, (this is a feature)
  * plays nice with dzen or similar external apps that set overrideredirect or write directly to root (see the config file for more)
  * on the fly reloading of settings (keeps your layout intact)

For a bit more, check out the wiki, look at [some screenshots](http://code.google.com/p/euclid-wm/wiki/scrots) or just checkout the code and see for yourself.

If you are are using euclid please consider joining [the mailing list](http://groups.google.com/group/euclid-wm) to keep up on this project (It isn't high volume, I promise.) You can also subscribe to the [feed](http://code.google.com/feeds/p/euclid-wm/updates/basic).

The development version is available via svn (see the [instructions](http://code.google.com/p/euclid-wm/wiki/dev_install)) and the latest stable is available as a tarball. For Arch users it is also available from AUR. (A big thanks to BKLive for maintaining it).


euclid-wm is **not** related to any other project named "euclid" that is not an X11 window manager.