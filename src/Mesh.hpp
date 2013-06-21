#ifndef __MESH_HPP__
#define __MESH_HPP__

#include <ctime>
#include <cstdlib>
#include <iostream>
#include <vector>
#include <set>
#include "math.h"

//#include <API/lib3ds/include/lib3ds.h>
#include <lib3ds.h>

#include "Figure.hpp"
#include "Boid.hpp"

class Mesh : public Figure
{
private :
	//Attributes
	Lib3dsFile * m_model;           		// Model (analyzed 3DS file)
	Lib3dsMesh * m_refMesh;				// Mesh reference
	int m_nbFaces;					// Number of faces of the Mesh	
	//@WARNING float* due to lib3ds
    	float m_boundingMin[3], m_boundingMax[3];     	// Bounding box
	//this info does not contain double,
	std::set< std::vector<float> > m_mesh;		// tried data
	//@WARNING to keep ?
	// It would be usefull to store them
	// for the potential renderman shaders	
	std::vector< std::vector<float> > m_roughMesh;  // rough mesh	
public :
	// Builder
	// Construct a Mesh from an obj file
	Mesh(const std::string filename);

	// Move the Mesh (animation)
	void move();
	//Draw function OpenGL
	void brutalDraw();
	
private:
	// Load Mesh data from file
	void _loadDataFromFile();
	// Generate boid field	
	void _generateBoidsFromMesh();
	// Adapt mesh to be between 0 and 1
	void _adaptMesh();                    
};

#endif // __MESH_HPP__
