#####################################################################
#
#  Makefile of dota.
#
# Author:   xiaomaoshi
# Date:     2015/5/30 create file.
# Describe:
# History
#####################################################################

CC=gcc
INCLUDE=-Iinclude
LIB=
CFLAG=-g -Wall -O2
LIBSO:=

SRC_TARGET = fight
SRC_SOURCES = $(wildcard bsc/*.c) $(wildcard cor/*.c)

$(SRC_TARGET): $(SRC_SOURCES)
	$(CC) -o $@ $^ $(INCLUDE) $(CFLAG) $(LIB) $(LIBSO)

.PHONY:clean
clean:
	rm -fr *.o $(SRC_TARGET) log/*
