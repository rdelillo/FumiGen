// All the initialisation of states and events for SDL and OpenGL

#ifndef __APPLICATION_HPP__
#define __APPLICATION_HPP__

#include <vector>
#include <string>

#include "Variables.h"
#include "Figure.hpp"
#include "Camera.hpp"

// This structure allow to store all of the scripted animation
// and make sure we are able to restart the scene from start at 
// any time.
typedef struct
{
	unsigned int indexFigure;	// Index of the figure inside the figure list
	std::string meshFilesPath;	// Path to the 3ds sequence of Mesh
	std::string boidFilesPath;	// Path to the 3ds sequence of Boid leader
	unsigned int m_startSequence;	// First frame of the sequence mesh
	unsigned int m_endSequence;	// Last frame of the sequence mesh
	unsigned int b_nbUnities;	// Nb of boids unities in the boids system
	unsigned int b_startSequence;	// First frame of the sequence boids system
	unsigned int b_endSequence;	// Last frame of the sequence boids system
	unsigned int frameBoids;	// Frame to turn into boids system
	unsigned int frameExplosion;	// Frame to explose the Figure
	float m_density;		// Density of the Figure
}
AnimatedData;

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
	
	// Keyboard parameters
	bool m_goingLeft;					// is going left
	bool m_goingRight;					// is going right
	bool m_goingForward;					// is going forward	
	bool m_goingBackward;					// is going backward

	// Figure parameters
	std::vector<Figure*> m_figures;				// Contains all of the figures defined
	std::vector<AnimatedData> m_animatedData;		// Contains all of the animated data

	//Others
	bool m_renderFlag;					// set to true when the process is rendering using Renderman
	Camera * m_camera;					// the FPS camera
	unsigned int _cntMove; 					// Move counter (total frame number)
	unsigned int _playMove;					// Play move counter (frame of the played sequence)
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
	void handleKeyUpEvents(SDL_keysym* keysym);
	void handleKeyDownEvents(SDL_keysym* keysym);

	// KEYBOARD-MOUSE MANAGEMENT
	inline bool goingLeft() const { return m_goingLeft; }
	inline bool goingRight() const { return m_goingRight; }
	inline bool goingForward() const { return m_goingForward; }
	inline bool goingBackward() const { return m_goingBackward; }
	inline float xMousePosition() const { return _xMousePosition; }
	inline float yMousePosition() const { return _yMousePosition; }

	// LISTENER
	// Listens to events during the whole time of the application
	// and distributes corresponding tasks
	void eventLoop();
	// Render current image in OpenGL
	void drawFrame();
	//@TODO: keep that ?
	// Render current frame with RenderMan
	void renderFrame();
	// Animates an objets
	void animate();

	// DELETE
	void deleteApplication();

	// FIGURES MANAGEMENT
	// Add a new figure to the Application
	void addFigure(Figure* f);
	// Return the number of Figure stored into the application
	inline unsigned int nbUnities() const { return m_figures.size(); }
	// Set the camera of the Application
	void defineCamera(Camera* camera);
	// Add an animation data for the Application
	void addAnimatedData(AnimatedData & a);

private :
	// Remove un-needed figures
	void _removeEmptyFigures();

	// Animation/Play functions
	// Reset scene : rebuild the transformed Figures
	void _reset();
	// Check transformation of the stored Figures
	void _transform();
};

// Create an user event for the render loop (the id is "MY_RENDER_LOOP")
// and registers it : it should now be send every "interval" set of time
Uint32 renderLoopTimer(Uint32 interval, void* param);
Uint32 refreshLoopTimer(Uint32 interval, void* param);

#endif 
