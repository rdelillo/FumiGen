// All the initialisation of states and events for SDL and OpenGL

#ifndef __APPLICATION_HPP__
#define __APPLICATION_HPP__

#include <vector>

#include "Variables.h"
#include "Figure.hpp"


class Application
{
	
private :
	// Timers
	SDL_TimerID _renderTimer; 				// timer for the rendering
	SDL_Surface* _drawContext;	
	
	// Windows parameters
	static const unsigned int _windowX = 1024; 		// Window origin X
	static const unsigned int _windowY = 0; 		// Window origin Y
	unsigned int _windowWidth; 				// Window width
	unsigned int _windowHeight;   				// Window height
	unsigned int _screenWidth; 				// Screen dimentions
	unsigned int _screenHeight;   				// Screen dimentions
	
	// Background video parameters
	float _backgroundColor[4]; 				// Background color
    	Uint32 _videoModeFlags;
    
	// Mouse parameters
	float _xMousePosition; 					// Mouse position - horizontal axis
	float _yMousePosition; 					// mouse position - vertical axis
	float _moveFlags[3];
	GLfloat _cameraMode;
	
	// Figure parameters
	std::vector<Figure*> m_figures;				// Contains all of the figures defined

	//Others
	GLuint _cntMove; 					// Move counter
	bool _done; 						// set to true when the window is closed or to end the application

public :

	// INIT
	Application();
	// Sets the application parameters and does all the initialisation
	void initApplication();
	// Inits SDL and OpenGL context, sets a few states
	void initDisplay(); 

	// DISPLAY
	// Turns ON/OFF wireframe mode
	void switchWireframe();
	void resize(const unsigned int w, const unsigned int h);
	// Go to fullscreen
	void switchFullScreen();

	// SDL MANAGEMENT
	// Distributes task for the "key" kind of events 
	// For example : std::cout when b key is pressed
	void handleUserEvents(SDL_Event* event);
	// Distributes task for the "user" kind of events 
	// For example : rendrFrame action occurs if a timer event is passed
	void handleKeyDownEvents(SDL_keysym* keysym);

	// LISTENER
	// Listens to events during the whole time of the application
	// and distributes corresponding tasks
	void eventLoop();
	// Render current image in OpenGL
	void renderFrame();
	// Animates an objets
	void animate();

	// DELETE
	void deleteApplication();

	// FIGURES MANAGEMENT
	// Add a new figure to the Application
	void addFigure(Figure* f);

private :
	// Draw the color triangle to test display
	void _drawTestSample();
	// Remove un-needed figures
	void _removeEmptyFigures();
};

// Create an user event for the render loop (the id is "MY_RENDER_LOOP")
// and registers it : it should now be send every "interval" set of time
Uint32 renderLoopTimer(Uint32 interval, void* param);


#endif 
