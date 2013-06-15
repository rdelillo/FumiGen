#include "Mesh.hpp"
#include "Boid.hpp"

#include <algorithm>

// Builder 
// Create a Mesh from an obj file
Mesh::Mesh(const std::string fileName):
m_model(NULL),
m_refMesh(NULL),
m_nbFaces(0)
{
	m_type = "3D_MESH";
	m_model = lib3ds_file_open(fileName.c_str());
	// Verify the model
	if(!m_model)                                                                 	
	{
		std::cout << "Error : unable to load the given file" << std::endl;
		exit(2);
	}
	// File is OK, load the model and construct the mesh
	_loadDataFromFile();
	_adaptMesh();                                                     
	_generateBoidsFromMesh();
}

// Load Mesh data from file
void Mesh::_loadDataFromFile()
{
	// Loop through all the meshes
	// (better to keep 1 per file)
	//@WARNING : do not use an int for warning when build
	for(int iMesh=0; iMesh<m_model->nmeshes; ++iMesh)
	{
		// Double check file content is OK
		if(m_model->meshes[iMesh]->nfaces <= 1)                              
		{
			std::cout << "Error file content is wrong" << std::endl;
			exit(2);
		}
		m_refMesh = m_model->meshes[iMesh];
		m_nbFaces = m_refMesh->nfaces;
		// Get bouding box    
		lib3ds_mesh_bounding_box(m_refMesh, m_boundingMin, m_boundingMax);        

		// Loop through every face
		//@WARNING : do not use an int for warning on build
		for(int iFace=0; iFace<m_nbFaces; ++iFace)
		{
		    Lib3dsFace* face = &m_refMesh->faces[iFace];
		    for(unsigned int i=0; i<3; ++i)
		    {
			std::vector<float> point;
			for(unsigned int idx=0; idx<3; ++idx)
				point.push_back(m_refMesh->vertices[face->index[i]][idx]);

			// Add this point to rough data
			m_mesh.push_back(point);
		    }
		}
	} 
}

// Generate boid field	
void Mesh::_generateBoidsFromMesh()
{
	for(unsigned int i=0; i<m_mesh.size(); ++i)
	{
		//@TODO: optimize the mesh
		// Always add the first one
		Boid b(0);			
		for(unsigned int j=0; j<3; ++j)
			b.setPosition(j, m_mesh[i][j]);

 		// Add this point as a new Boid
		m_group.push_back(b);
		continue;	
	
		// Brutal compare and add to mesh
		//@WARNING need to use a regular int here
		/*for(int k=i-1; k>=0; --k)
		{
			const float x = m_group[k].position(0);
			const float y = m_group[k].position(1);
			const float z = m_group[k].position(2);
			// Point is already a boid
			if((x == m_mesh[i][0]) &&
			   (y == m_mesh[i][1]) &&
			   (z == m_mesh[i][2]))
				continue;
			Boid b(m_group.size());
			for(unsigned int j=0; j<3; ++j)
				b.setPosition(j, m_mesh[i][j]);
			// Add this point as a new Boid
			m_group.push_back(b);	
		}*/
	}
}

//Adapt mesh to be between 0 and 1
void Mesh::_adaptMesh()
{
	//Get max norm
	std::vector<float> normVector;
	for(unsigned int i=0; i<3; ++i)
		normVector.push_back(sqrt(pow(m_boundingMax[i]-m_boundingMin[i],2)));

	float norm = normVector[0];
	if(normVector[1] > std::max(normVector[0], normVector[2])) norm = normVector[1];
	if(normVector[2] > std::max(normVector[0], normVector[1])) norm = normVector[2];
  
	// Reduce mesh
	for(unsigned int i=0; i<m_mesh.size(); ++i)
	{
		for(unsigned int j=0; j<3; ++j)
			m_mesh[i][j] /= norm;
	}
}

//Move : do nothing
void Mesh::move()
{
}

//Draw function OpenGL
void Mesh::brutalDraw()
{
	std::cout << "brutal Draw - "<< m_type << "(" << \
				m_group.size() <<")"<< std::endl;
	/*
	for(unsigned int i=0; i<m_group.size(); ++i)
	{
		float x = m_group[i].position(0);
		float y = m_group[i].position(1);
		float z = m_group[i].position(2);
		
		glVertex3d(x,y,z);
	}*/
}
