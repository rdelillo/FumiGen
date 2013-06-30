#include "Variables.h"
#include "Application.hpp"
#include "Figure.hpp"
#include "Mesh.hpp"
#include "Boids.hpp"
//#include "Scene.hpp"


//Create an Application
Application* createApplication()
{
	Application* application = new Application(); 
	application->initApplication();
	return application;
}

// Entry point in the program
int main(int argc, char **argv)
{
	// Application creation
	Application* application = createApplication();

	// Creation of the figure to store
	application->addFigure(new Boids(30));
	application->addFigure(new Mesh("/home/robin/Bureau/FumiGen/src/3ds/essai.3ds"));	

	// Render Loop
	application->eventLoop();

	//Quit
	application->deleteApplication();
	free(application);
	return 0;
}
