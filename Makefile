ENABLE_LIBNOTIFY ?= 1
DEPENDENCIES = udisks2
CFLAGS += -Wall -Wextra -Werror -Wno-unused-parameter -O3

ifeq ($(ENABLE_LIBNOTIFY), 1)
    DEPENDENCIES += libnotify
    CFLAGS += -DHAVE_LIBNOTIFY
endif

CFLAGS += $(shell pkg-config --cflags $(DEPENDENCIES))
LDLIBS += $(shell pkg-config --libs $(DEPENDENCIES))

.PHONY: all clean distclean

all: usermount

clean:

distclean: clean
	-rm usermount
