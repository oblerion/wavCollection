#! /bin/bash

all: clean
	cd src && make
clean:
	cd src && make clean