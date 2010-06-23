SHARE_DIR = /usr/share
EXE_DIR = /usr/bin
CC = cc
INSTALL = install
CFLAGS += -O3
CFLAGS += -oeuclid-wm
CFLAGS += -pipe
LDFLAGS += -lX11

.SILENT:
.PHONEY: install clean uninstall dist distclean

all: euclid-wm.c
	$(CC) $(LDFLAGS) $(CFLAGS) ./euclid-wm.c 
	
install: all
	
	if [ ! `which dmenu_run` ]; then echo "WARNING: dmenu_run not found, you should install dmenu before running euclid-wm"; fi
	if [ ! `which x-terminal-emulator` ]; then echo "WARNING: x-terminal-emulator not found, you should define it before running euclid-wm"; fi
	if [ ! `which x-terminal-emulator` ] && [ ! `which dmenu_run` ]; then echo "ERROR: Neither x-terminal-emulator, nor dmenu_run can be found. Without at least one of these you will not be able to run anything once you start euclid-wm.Please install one of these and try again. Aborting install."; exit 1; fi
	cp ./euclid-wm $(BIN_DIR)/euclid-wm
	cp ./start-euclid $(BIN_DIR)/start-euclid
	cp ./euclid.desktop $(SHARE_DIR)/xsessions/euclid.desktop
	cp ./euclid.1 $(SHARE_DIR)/man/man1/euclid-wm.1
	chmod 0755 $(BIN_DIR)/euclid-wm
	chmod 0644 $(SHARE_DIR)/man/man1/euclid-wm.1
	chmod 0755 $(SHARE_DIR)/xsessions/euclid.desktop
	chmod 0755 $(BIN_DIR)/start-euclid

uninstall:
	rm -f $(BIN_DIR)/euclid-wm
	rm -f $(BIN_DIR)/start-euclid
	rm -f $(SHARE_DIR)/xsessions/euclid.desktop
	rm -f $(SHARE_DIR)/man/man1/euclid-wm.1

clean: 
	rm -f euclid-wm 
