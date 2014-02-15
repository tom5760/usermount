PKGS=udisks2 libnotify
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
