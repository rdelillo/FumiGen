#include "Variables.h"
#include "Application.hpp"
#include "XmlParser.hpp"


//Create an Application
Application* createApplication(const std::string xmlFile)
{
	Application* application = new Application(); 
	XmlParser(xmlFile, application);
	application->initApplication();
	return application;
}

// Entry point in the program
int main(int argc, char **argv)
{
	// Application creation
	if(argc < 2)
	{
		std::cout << "Error: no XML scene file providen" << std::endl;
		exit(2);
	}
	std::string xmlFile = argv[1];
	Application* application = createApplication(xmlFile);
		
	// Render Loop
	application->eventLoop();

	//Quit
	application->deleteApplication();
	free(application);
	return 0;
}
