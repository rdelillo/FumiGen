#ifndef __MESH_HPP__
#define __MESH_HPP__

#include <lib3ds.h>
#include <ctime>
#include <cstdlib>
#include <iostream>
#include <vector>
#include <set>
#include "math.h"

#include "Figure.hpp"
#include "Boid.hpp"

class Mesh : public Figure
{
private :
	// Attributes
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
	std::vector< std::vector<float> > m_roughMesh;  	// rough mesh

	// Animation attributes
	int m_currentFrame;						// current frame (default 0)	
	std::vector< std::set< std::vector<float> > > m_meshes;		// mesh per frame
	std::vector< std::vector< std::vector<float> > > m_roughMeshes; // roughMesh per frame
	
public :
	// Builder
	// Construct a Mesh from a 3ds file
	Mesh(const std::string filename);
	// Construct a Mesh from a 3ds file sequence
	Mesh(const std::string filepath, const int start, const int end);

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
