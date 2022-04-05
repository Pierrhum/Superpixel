OBJS	= Image_ppm.o Image.o main.o
SOURCE	= Image_ppm.cpp Image.cpp main.cpp
HEADER	= Image_ppm.h Image.hpp
OUT	= prog
CC	 = g++
FLAGS	 = -g -c -Wall
LFLAGS	 = 

all: $(OBJS)
	$(CC) -g $(OBJS) -o $(OUT) $(LFLAGS)

Image.o: Image.cpp
	$(CC) $(FLAGS) Image.cpp 

main.o: main.cpp
	$(CC) $(FLAGS) main.cpp 


clean:
	rm -f $(OBJS) $(OUT)