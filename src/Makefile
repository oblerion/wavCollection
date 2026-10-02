#! /bin/bash

CC = gcc
BIN = main

LUALINUX = ../dep/lua-5.4.2/lua_linux
LUAWIN = ../dep/lua-5.4.2/lua_win
FLAGLINUX = -g -Wall -lGL -lm -lpthread -ldl -lrt -lX11
FLAGWIN= -g -Wall -lm -lopengl32 -lgdi32 -lwinmm -mwindows -static-libgcc
# start os detect
OSFLAG :=
ifeq ($(OS),Windows_NT)
	OSFLAG = WIN
else
	UNAME_S := $(shell uname -s)
	ifeq ($(UNAME_S),Linux)
		OSFLAG = LINUX
	endif
	ifeq ($(UNAME_S),Darwin)
		OSFLAG = OSX
	endif
endif

ifeq ($(OSFLAG),WIN)
	BIN = $(BIN).exe
endif



LUA :=
ifeq ($(OSFLAG),LINUX)
	LUA = $(LUALINUX)
endif
ifeq ($(OSFLAG),WIN)
	LUA = $(LUAWIN)
endif

RAYLIBLIN = ../dep/raylib-5.5_linux_amd64_stat
RAYLIBWIN = ../dep/raylib-5.5_win32_mingw-w64

RAYLIB :=
ifeq ($(OSFLAG),LINUX)
	RAYLIB = $(RAYLIBLIN)
endif
ifeq ($(OSFLAG),WIN)
	RAYLIB = $(RAYLIBWIN)
endif

FLAG :=
ifeq ($(OSFLAG),LINUX)
	FLAG = $(FLAGLINUX)
endif
ifeq ($(OSFLAG),WIN)
	FLAG = $(FLAGWIN)
endif
INC = -I$(RAYLIB)/include -I$(LUA)/include
LIB = -L$(RAYLIB)/lib -lraylib -L$(LUA)/lib -llua
ifeq ($(LUA),1)
#	CFLAGS="${CFLAGS} -L${LUALINUX}/lib -llua"
endif

CMD_RM = rm -f
CMD_RM2 = 
ifeq ($(OSFLAG),WIN)
	CMD_RM = del
	CMD_RM2 = 2>null
endif

all: link
link: *.o
	$(CC) *.o  $(LIB) $(FLAG) -o ../$(BIN)
*.o:
	$(CC) *.c -c $(INC)

clean:
	$(CMD_RM) *.o $(CMD_RM2)
	$(CMD_RM) ../*.o $(CMD_RM2)
	$(CMD_RM) ../*.data $(CMD_RM2)
	$(CMD_RM) ../*.js $(CMD_RM2)
	$(CMD_RM) ../*.wasm $(CMD_RM2)
	$(CMD_RM) ../*.html $(CMD_RM2)


