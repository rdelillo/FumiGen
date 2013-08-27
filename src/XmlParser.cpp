#include "XmlParser.hpp"
#include "Camera.hpp"
#include "Mesh.hpp"
#include "Boids.hpp"

// Builder
XmlParser::XmlParser(const std::string file, Application * application):
m_application(application),
m_xmlFile(file)
{
	_readXmlFile();
	_parseCamera();
	_addMeshes();
	_addBoidsSystems();
}


// Utils
// Read Xml file
void XmlParser::_readXmlFile()
{
	// Open the providen Xml file 
	pugi::xml_document doc;
	pugi::xml_parse_result result = doc.load_file(m_xmlFile.c_str());
	if(!result)
	{
		std::cout << "Error: Xml parsing - " << result.description() << std::endl;
		std::cout << "Error: offset - " << result.offset  << std::endl;
		exit(2);
	}
}

// Get scene node from Xml file
pugi::xml_node XmlParser::_getScene()
{
	pugi::xml_document doc;
	doc.load_file(m_xmlFile.c_str());
	return doc.child("scene");
}

// Parse camera information
void XmlParser::_parseCamera()
{
	// Read and Add camera information
	// Always only one camera
	pugi::xml_node camera = _getScene().child("camera");
	std::string filepath = camera.attribute("filepath").value();
	int start = camera.attribute("start").as_int();
	int end = camera.attribute("end").as_int();

	// Create the application camera
	Camera * new_camera;

	// Animated camera providen
	if(end == 0)
		new_camera = new Camera();
	else
		new_camera = new Camera(filepath, start, end);
	// Add the camera into the Application
	m_application->defineCamera(new_camera);
}

// Add Meshes
void XmlParser::_addMeshes()
{
	std::vector<Temp_Mesh> meshVector;
	pugi::xml_node meshes = _getScene().child("meshes");
	// For each mesh found add it to the application
	for (pugi::xml_node_iterator it = meshes.begin(); it != meshes.end(); ++it)
	{
		Temp_Mesh meshInfo ;
		meshInfo.name = it->attribute("name").value();
		meshInfo.filepath = it->attribute("filepath").value();
		meshInfo.start = it->attribute("start").as_int();
		meshInfo.end = it->attribute("end").as_int();

		//@TODO: implements
		//int turnInto_boidsSystem = mesh.attribute("boidsSystem").as_int();
		//int turnInto_explosion = mesh.attribute("explosion").as_int();
		meshVector.push_back(meshInfo);
	}
	// For each mesh info, add it to the application
	for(unsigned int i=0; i<meshVector.size(); ++i)
	{
		Temp_Mesh meshInfo = meshVector[i];
		// Generate the mesh	
		Mesh * new_mesh ;
		// Animated mesh providen
		if(meshInfo.end == 0)
			new_mesh = new Mesh(meshInfo.filepath);
		else
			new_mesh = new Mesh(meshInfo.filepath, meshInfo.start, meshInfo.end);
		new_mesh->setName(meshInfo.name);
		// Add the new mesh into the Application
		m_application->addFigure(new_mesh);
	}
}

// Add Boids systems
void XmlParser::_addBoidsSystems()
{
	std::vector<Temp_Boids> boidsVector;
	pugi::xml_node boidsSystems = _getScene().child("boidsSystems");
	// For each boids system found store the information
	for (pugi::xml_node_iterator it = boidsSystems.begin(); it != boidsSystems.end(); ++it)
	{
		Temp_Boids boidInfo;
		boidInfo.name = it->attribute("name").value();
		boidInfo.nbUnities = it->attribute("nbUnities").as_int();
		boidInfo.filepath = it->attribute("filepath").value(); 
		boidInfo.start = it->attribute("start").as_int();	
		boidInfo.end = it->attribute("end").as_int();

		//@TODO: implements
		//int turnInto_explosion = boidsSystem.attribute("explosion").as_int();
		boidsVector.push_back(boidInfo);
	}
	// For each boids system info, add it into the application
	for(unsigned int i=0; i<boidsVector.size(); ++i)
	{
		Temp_Boids boidInfo = boidsVector[i];
		// Generate the boids system	
		Boids * new_boids ;
		// Animated leader of boids system providen
		if(boidInfo.end == 0)
			new_boids = new Boids(boidInfo.nbUnities);
		else
			new_boids = new Boids(boidInfo.nbUnities, boidInfo.filepath, boidInfo.start, boidInfo.end);
		new_boids->setName(boidInfo.name);
		// Add the new boids system into the Application
		m_application->addFigure(new_boids);
	}
}
