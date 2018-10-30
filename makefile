exe: console.o gui.o Vector.o main.o Pion.o Player.o consoleBoard.o sdlBoard.o
	gcc console.o gui.o Vector.o main.o Pion.o Player.o consoleBoard.o sdlBoard.o -o exe `sdl2-config --cflags --libs` -lm -Wall

main.o: interface/console/console.c interface/sdl/gui.c main.c
	gcc -c main.c -Wall

gui.o: interface/sdl/gui.h interface/sdl/gui.c
	gcc -c interface/sdl/gui.c -Wall

console.o: interface/console/console.h interface/console/console.c interface/console/consoleBoard.h
	gcc -c interface/console/console.c -Wall

consoleBoard.o: interface/console/consoleBoard.h interface/console/consoleBoard.c define.h
	gcc -c interface/console/consoleBoard.c -Wall

sdlBoard.o: interface/sdl/sdlBoard.h interface/sdl/sdlBoard.c define.h
	gcc -c interface/sdl/sdlBoard.c -Wall

Vector.o: mods/Vector.h mods/Vector.c
	gcc -c mods/Vector.c -Wall 

Pion.o: mods/Pion.h mods/Pion.c define.h
	gcc -c mods/Pion.c -Wall

Player.o: mods/Player.h mods/Player.c
	gcc -c mods/Player.c -Wall
