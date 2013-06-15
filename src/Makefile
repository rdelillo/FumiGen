#Makefile Fumigen

MACHINE = $(shell uname -s)

ifeq ($(MACHINE),Darwin)
	OPENGL_INC = -FOpenGL
	OPENGL_LIB = -framework OpenGL -lm
	SDL_INC = `sdl-config --cflags`
	SDL_LIB = `sdl-config --libs`
	CXX=g++
else
	OPENGL_INC = -I/usr/X11R6/include -I API/lib3ds/include
	OPENGL_LIB = -I/usr/lib64 -lGL -lGLU -lm
	LDLIBSOPTIONS=-LAPI/lib3ds/lib   API/lib3ds/lib/lib3ds.a
	SDL_INC = -I/usr/include/SDL/
	SDL_LIB = `sdl-config --libs`
	#RENDERMAN_INC =  -I$(DELIGHT)/include
	#RENDERMAN_LIB = -L/$(DELIGHT)/lib -l3delight
	CXX=g++
endif


COMPILER_FLAGS=-g -I.
COMPILER_FLAGS_WARN= -Wall -I.

EXE=TestAppli

INCLUDE= $(SDL_INC) $(OPENGL_INC) $(RENDERMAN_INC)
LIBS= $(SDL_LIB) $(OPENGL_LIB) $(RENDERMAN_LIB) $(LDLIBSOPTIONS)

OBJS = main.o Application.o Figure.o
OBJS += Boid.o Boids.o Explosion.o
OBJS += Camera.o Tools.o

all : $(EXE) 

$(EXE) : $(OBJS)
	$(CXX) $(COMPILER_FLAGS_WARN) $^ $(LIBS) -o $@

%.o: %.cpp
	$(CXX) -c $(COMPILER_FLAGS_WARN) $(INCLUDE) $<



.PHONY : clean ultraclean

clean::
	rm -f *.o *~

ultraclean : clean
	rm -f $(EXE)
