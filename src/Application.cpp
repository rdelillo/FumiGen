#include "Application.hpp"
#include "Tools.hpp"

//@TODO: Ameliorate the scripting
#include "Explosion.hpp"
#include "Boids.hpp"

Application::Application():
_windowWidth(800),	
_windowHeight(450),	
_xMousePosition(0.0),
_yMousePosition(0.0),
m_goingLeft(false),
m_goingRight(false),
m_goingForward(false),
m_goingBackward(false),
m_camera(NULL)
{
	//Fill up move values
	_moveFlags[0]=0.0;
	_moveFlags[1]=0.0;
	_moveFlags[2]=0.0;

	//Default colors for background
	_backgroundColor[0]=0.0;
	_backgroundColor[1]=0.0;    
	_backgroundColor[2]=0.0;
	_backgroundColor[3]=1.0;

	//Resizable screen
	_videoModeFlags = SDL_OPENGL | SDL_RESIZABLE; 
	_done = false;
}

// Sets the application parameters and does all the initialisation
void Application::initApplication()
{
	//Display parameters init
	initDisplay();

	//Timer init (24 fps)
	_renderTimer = SDL_AddTimer(41, renderLoopTimer, this);
	_renderTimer = SDL_AddTimer(80, refreshLoopTimer, this);

	//Move counter
	_cntMove=0;
	// Initialize the projection matrix
	m_camera->setPerspectiveFromAngle();

	//Mouse is not going to leave the window
	//Mouse won't be seeable
	SDL_WM_GrabInput(SDL_GRAB_ON);
	SDL_ShowCursor(SDL_DISABLE);
}


// Inits SDL and OpenGL context, sets a few states
void Application::initDisplay()
{
	// Initialize timer, audio, video CD_ROM, and joystick.
	int sdlError=SDL_Init(SDL_INIT_EVERYTHING);
	if (sdlError<0) 
		std::cout << "Unable to init SDL : " << SDL_GetError() << std::endl ;

	// Sets openGL parameters before opening the draw context
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);    // Double buffering
	SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 16);	// Depth buffer size of 16-bit
	SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 8);    	// Color components size of 8-bit each
	SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE, 8);

	// Creation of the openGL draw context
	const SDL_VideoInfo* videoInfo=SDL_GetVideoInfo();
	_screenWidth = videoInfo->current_w;
	_screenHeight = videoInfo->current_h;

	// Specifies the size and other options about the OpenGL window
	_drawContext = SDL_SetVideoMode(_windowWidth, _windowHeight, 0, _videoModeFlags); 

	// Depth and Alpha test
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_BLEND); 
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	// Decides the background color used after this call
	glClearColor(_backgroundColor[0], _backgroundColor[1], _backgroundColor[2], _backgroundColor[3]);
	// Sets the with of the lines
	glLineWidth(2);
}

// Turns ON/OFF wireframe mode
void Application::switchWireframe()
{
	GLint wireframe[2];
	glGetIntegerv(GL_POLYGON_MODE, wireframe);
	if (wireframe[0] == GL_FILL)
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	else
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
}


// Adapts the drawing to the new size of the window
void Application::resize(const unsigned int w, const unsigned int h)
{
	_windowWidth = w;
	_windowHeight = h;

	//SDL_VideoMode update (may be unnecessary, and not woking proberly on somme operating systems)
	_drawContext = SDL_SetVideoMode(_windowWidth, _windowHeight, 0, _videoModeFlags);

	// Viewport transformation update to fit initial window size
	glViewport(0, 0, _windowWidth, _windowHeight);
}

// Go to fullscreen
void Application::switchFullScreen()
{
    if (_videoModeFlags == (SDL_OPENGL | SDL_RESIZABLE))
    {
        _videoModeFlags = SDL_OPENGL | SDL_FULLSCREEN;
        resize(_screenWidth, _screenHeight);
    }
    else
    {
        _videoModeFlags = SDL_OPENGL | SDL_RESIZABLE;
        resize(_windowWidth, _windowHeight);
    }
}

// Distributes task for the "user" kind of events 
// For example : rendrFrame action occurs if a timer event is passed
void Application::handleUserEvents(SDL_Event* event)
{
	switch (event->user.code) 
	{
		// Frames update (24fps)
		case MY_RENDER_LOOP:
			drawFrame();
			animate();
			break;
		
		// Refresh update (1sec)
		case FREE_REFRESH_LOOP:
			if(m_camera->getMode() == "FPS")
				tool_camera::manageFps(*(this), m_camera);
			break;

		default:
			break;
	}
}


// Distributes task for the "key" kind of events 
// For example : std::cout when b key is pressed
void Application::handleKeyDownEvents(SDL_keysym* keysym)
{
	switch(keysym->sym)
    	{
    		case SDLK_ESCAPE:
      			_done=true;
      			break;
      	
		// W : wireframe
    		case SDLK_w :
      			switchWireframe();
      			break;
      	
		// F : fullscreen
     		case SDLK_f :
      			switchFullScreen();
      			break;     	
	
		//@TO_REMOVE
		// E : explose everything
		case SDLK_e :
			for(unsigned int i=0; i<m_figures.size(); ++i)
				m_figures[i] = new Explosion(m_figures[i]);
			break;

		//@TO_REMOVE
		// B : make everything as Boids
		case SDLK_b :
			for(unsigned int i=0; i<m_figures.size(); ++i)
				m_figures[i] = new Boids(m_figures[i]);
			break;

		// Space : launch play mode
		//@TODO: update with the flag into application
		case SDLK_SPACE :
			m_camera->startPlayMode();
			break;

		// Camera info
		case SDLK_c :
			std::cout << "position : " << m_camera->position(0) << "," << m_camera->position(1) << "," <<  m_camera->position(2) << std::endl;
			break;

		// FPS management
		// classic : Z,Q,S,D
		case SDLK_z :	
			m_goingForward = true;
		      	break;
		case SDLK_s :
			m_goingBackward = true;
			break;
		case SDLK_q :
			m_goingLeft = true;
			break;
		case SDLK_d :
			m_goingRight = true;			
			break;

    		default:
      			break;
  	}
}

// Distributes task for the "key" kind of events 
// For example : std::cout when b key is pressed
void Application::handleKeyUpEvents(SDL_keysym* keysym)
{
	switch(keysym->sym)
    	{
		// FPS management
		// classic : Z,Q,S,D
		case SDLK_z :	
			m_goingForward = false;
		      	break;
		case SDLK_s :
			m_goingBackward = false;
			break;
		case SDLK_q :
			m_goingLeft = false;
			break;
		case SDLK_d :
			m_goingRight = false;			
			break;

    		default:
      			break;
  	}
}

// Listens to events during the whole time of the application
// and distributes corresponding tasks
void Application::eventLoop()
{
	SDL_Event event;
    	float test = 0.0f;

	while( (!_done) && (SDL_WaitEvent( &event )) ) 
	{
		switch(event.type) 
		{
			case SDL_USEREVENT:
				handleUserEvents( &event );
				break;
			case SDL_KEYUP:
				handleKeyUpEvents( &event.key.keysym );
				break; 
			case SDL_KEYDOWN:
				handleKeyDownEvents( &event.key.keysym );
				break;                                   
			case  SDL_VIDEORESIZE: 
				resize( event.resize.w, event.resize.h );
				break;                      
			case SDL_QUIT:
				_done=true;
				break;

	   		// Manage the mouse motion
			case SDL_MOUSEMOTION:
				_xMousePosition += (float)event.motion.xrel/(float)_windowWidth * 1.2f;
				test = _yMousePosition - (float)event.motion.yrel/(float)_windowHeight * 1.2f;
				if((test>-M_PI) && (test<0.0f))
					_yMousePosition -= (float)event.motion.yrel/(float)_windowHeight * 1.2f;

				break;

			default:
				break;
		} 
	}
}

// Render current image in OpenGL
void Application::drawFrame()
{
	// Clears the window with current clearing color, clears also the depth buffer
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	//<DRAW HERE>
	std::cout << "\nDRAW - FRAME : " << _cntMove << std::endl;

	// Sets the matrix mode
	//@WARNING : Need a float* for modelview and projection in OpenGL
	glMatrixMode (GL_PROJECTION);
	float projection[16];
	glLoadMatrixf(m_camera->getProjectionf(projection));
	glMatrixMode(GL_MODELVIEW);
	float modelView[16];
	glLoadMatrixf(m_camera->getViewf(modelView));

	// Usuals draw settings
	glPointSize(1.5f);
	glColor3ub(255,0,0);
	
	glBegin(GL_POINTS);
	glVertex3f(0.0f, 0.0f, -1.0f);
	glColor3ub(255,255,255);
	// Draw all of the figures
	for( unsigned int i=0; i<m_figures.size(); ++i)
		m_figures[i]->brutalDraw();
	glEnd();
	
	tool_camera::drawTestScene();
	//</DRAW HERE>
  
	// Performs the buffer swap between the current shown buffer
	SDL_GL_SwapBuffers();
}

// Animates an objets
void Application::animate()
{
	++_cntMove;
	// Remove un-needed figures 
	if(_cntMove%FREE_REFRESH_LOOP == 0)
		_removeEmptyFigures();
	// If in PLAY mode (no FPS)
	if(m_camera->getMode() == "PLAY")
	{
		// Animate the camera if needed
		m_camera->move();
		// Animate the figures
		for(unsigned int i=0; i<m_figures.size(); ++i)
			m_figures[i]->move();
	}
}

// Cleans before the application can be closed
void Application::deleteApplication()
{
	// Free all of the figures
	for(unsigned int i=0; i<m_figures.size(); ++i)
		free(m_figures[i]);
	// Free the camera
	free(m_camera);
	// Clean SDL quit
	SDL_RemoveTimer(_renderTimer);
	SDL_Quit();
}

// Add a new figure to the Application
void Application::addFigure(Figure* f)
{
	m_figures.push_back(f);
}

// Set the camera of the Application
void Application::defineCamera(Camera* camera)
{
	m_camera = camera;
}

// Remove un-needed figures (empty ones)
void Application::_removeEmptyFigures()
{	
	//@WARNING: have to use an int here
	// as loop on reverse order
	for(int i=m_figures.size()-1; i>=0; i-=1)
	{
		if( !m_figures[i]->isNeeded())
		{
			free(m_figures[i]);
			m_figures.erase(m_figures.begin()+i);
		}
	}
}

// Utils

// Create an user event for the render loop (the id is "MY_RENDER_LOOP")
// and registers it : it should now be send every "interval" set of time
Uint32 renderLoopTimer(Uint32 interval, void* param)
{
    SDL_Event event;
    event.type = SDL_USEREVENT;
    event.user.code = MY_RENDER_LOOP;
    event.user.data1 = 0;
    event.user.data2 = 0;
    SDL_PushEvent(&event);
    return interval;
}

// Create an user event for the render loop (the id is "MY_RENDER_LOOP")
// and registers it : it should now be send every "interval" set of time
Uint32 refreshLoopTimer(Uint32 interval, void* param)
{
    SDL_Event event;
    event.type = SDL_USEREVENT;
    event.user.code = FREE_REFRESH_LOOP;
    event.user.data1 = 0;
    event.user.data2 = 0;
    SDL_PushEvent(&event);
    return interval;
}

