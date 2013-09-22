#ifndef __XMLREADER_HPP__
#define __XMLREADER_HPP__

#include "Application.hpp"
#include "utils/pugixml/pugixml.hpp"

#include <ctime>
#include <cstdlib>
#include <iostream>
#include <vector>
#include <string>

//@WARNING PugiXML issue : initialize any pointer when working with the 
// library looks like breaking the XML parsing. Need to load all of 
// the information, then create the elements.
typedef struct
{
	std::string name;
	std::string filepath;
	unsigned int start;
	unsigned int end;
	float density;
} Temp_Mesh;

typedef struct 
{
	std::string name;
	std::string filepath;
	unsigned int nbUnities;
	unsigned int start;
	unsigned int end;
	unsigned int explosion;
}Temp_Boids;

// Parse the scene XML file and build the application based on it
// Uses the pugiXml library
class XmlParser
{
private :
	Application * m_application; 	// the FumiGen application
	std::string m_xmlFile;		// Xml file

public :
	// Builder
	XmlParser(const std::string file, Application * application);
private:
	// Utils
	// Read Xml file
	void _readXmlFile();
	// Parse camera information
	void _parseCamera();
	// Add Meshes
	void _addMeshes();
	// Add Boids systems
	void _addBoidsSystems();
	// Get scene node from Xml file
	pugi::xml_node _getScene();
};

#endif //__XMLREADER_HPP__
