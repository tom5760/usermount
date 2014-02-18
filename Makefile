# Comment out these two lines (and uncomment the line marked below) to disable
# notifications
PKGS=udisks2 libnotify
CPPFLAGS=-DHAVE_LIBNOTIFY

# Uncomment this line to build without notification support
#PKGS=udisks2

CFLAGS+=$(shell pkg-config --cflags $(PKGS))
LDLIBS+=$(shell pkg-config --libs $(PKGS))

CFLAGS += -Wall -Wextra -Werror -Wno-unused-parameter

#CFLAGS += -O0 -g
CFLAGS += -O3

.PHONY: all clean distclean

all: usermount

clean:

distclean: clean
	-rm usermount
