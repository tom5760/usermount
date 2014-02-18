usermount - Simple C Automounter
================================

 * Author: Tom Wambold <tom5760@gmail.com>
 * Copyright (c) 2014 Tom Wambold
 * [![Build Status](https://secure.travis-ci.org/tom5760/usermount.png?branch=master)](http://travis-ci.org/tom5760/usermount)

Purpose
-------

`usermount` is a small C program that interacts with UDisks2 via D-Bus to
automount removable devices as a normal user.

Dependencies
------------

 * udisks2 - Tested with version 2.1.2
    * http://www.freedesktop.org/wiki/Software/udisks
    * Arch Linux Package: udisks2

 * (OPTIONAL) libnotify - Tested with version 0.7.6
    * http://library.gnome.org/devel/notification-spec/
    * Arch Linux Package: libnotify

Compiling
---------

Just type `make`.

If you wish to disable notifications, edit the Makefile to comment the marked
lines.

Running
-------

Currently, `usermount` takes no arguments, so just run `usermount`.
