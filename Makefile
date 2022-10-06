linux:
	g++ main.cpp glad/src/glad.c -omarchingCubes -O2 -lglfw -lGL -lX11 -lpthread -ldl -lGLU -Wno-write-strings -Iglad/include