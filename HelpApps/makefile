Target = helper
CC = gcc

SRC = ${wildcard ./*.c}
OBJ = ${patsubst %.c, ${objDir}%.o, ${SRC}}

main: FileMaker
	${CC} -o ./build/${Target} ${SRC} -lz
	./build/${Target} 

FileMaker:
	if not exist "build" mkdir build