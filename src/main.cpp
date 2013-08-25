#include "Variables.h"
#include "Application.hpp"
#include "Figure.hpp"
#include "Mesh.hpp"
#include "Boids.hpp"
#include "Camera.hpp"


//Create an Application
Application* createApplication(Camera * camera)
{
	Application* application = new Application(); 
	application->defineCamera(camera);
	application->initApplication();
	return application;
}

// Entry point in the program
int main(int argc, char **argv)
{
	// Application creation
	Camera * camera = new Camera("/home/robin/Bureau/FumiGen/src/3ds/test_camera/camera/scene", 0, 72);
	//Camera * camera = new Camera();
	Application* application = createApplication(camera);

	// Creation of the figure to store
	//application->addFigure(new Boids(30));
	//application->addFigure(new Mesh("/home/robin/Bureau/FumiGen/src/3ds/essai.3ds"));	
	application->addFigure(new Mesh("/home/robin/Bureau/FumiGen/src/3ds/test_camera/fix_repere/scene.0000.3ds"));
	application->addFigure(new Mesh("/home/robin/Bureau/FumiGen/src/3ds/test_camera/cube_y/scene", 0, 72));
	application->addFigure(new Mesh("/home/robin/Bureau/FumiGen/src/3ds/test_camera/cube_x/scene", 0, 72));
	application->addFigure(new Mesh("/home/robin/Bureau/FumiGen/src/3ds/test_camera/cube_z/scene", 0, 72));	
	// Render Loop
	application->eventLoop();

	//Quit
	application->deleteApplication();
	free(application);
	return 0;
}
