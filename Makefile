# Makefile for compiling in Linux

make: main.cpp
	g++ main.cpp -Wall -pedantic -O1 -o wdune_core_linux.exe
	cp ./wdune_core_linux.exe ../core/wdune_core_linux.exe
	