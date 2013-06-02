#Makefile Fumigen

MACHINE = $(shell uname -s)

ifeq ($(MACHINE),Darwin)
	OPENGL_INC = -FOpenGL
	OPENGL_LIB = -framework OpenGL -lm
	SDL_INC = `sdl-config --cflags`
	SDL_LIB = `sdl-config --libs`
	CXX=g++
else
	OPENGL_INC = -I/usr/X11R6/include
	OPENGL_LIB = -I/usr/lib64 -lGL -lGLU -lm
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
LIBS= $(SDL_LIB) $(OPENGL_LIB) $(RENDERMAN_LIB)

OBJS = main.o Application.o Figure.o
OBJS += Boid.o Boids.o Explosion.o
OBJS += Camera.o Tools.o
#OBJS = Camera.o Scene.o convert.o Light.o renderman.o CameraMotion.o Rail.o Pan.o Application.o tools.o
#OBJS += main.o glew.o ShadersBases.o ObjectGL.o Light.o Spline.o  Map.o MultiSpline.o TruncateCone.o Cube.o
#OBJS += CubeLumineux.o Flocon.o Grille.o Spider.o SphereNR.o SpiderLeg.o ArbreGentil.o Abdomen.o
#OBJS += Leaf.o Champignon.o Cristal.o BlocCristal.o Feuille.o Fougere.o Rock.o Trunk.o Firebug.o Grass.o ParticleEngine.o LeafEngine.o

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
