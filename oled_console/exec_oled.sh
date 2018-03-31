#!/bin/sh
# -*- coding: utf-8 -*-
# 日本語

TTY_DEV=/dev/tty1

sudo sh -c "echo -n '$ ' > ${TTY_DEV} && echo $* > ${TTY_DEV}"
exec sudo sh -c "$* > ${TTY_DEV}"
