#include "Mesh.hpp"
#include "Boid.hpp"
#include "Tools.hpp"

#include <algorithm>

// Builder 
// Create a Mesh from an obj file
Mesh::Mesh(const std::string fileName):
m_model(NULL),
m_refMesh(NULL),
m_nbFaces(0),
m_currentFrame(0)
{
	m_type = "3D_MESH";
	m_model = tool_filesystem::open3dsFile(fileName);
	// File is OK, load the model and construct the mesh
	_loadDataFromFile();
	//_adaptMesh();                                                     
	_generateBoidsFromMesh();
	// Add to the container per frame
	// There is only 1 entry since the model does not move
	m_meshes.push_back(m_mesh);
	m_roughMeshes.push_back(m_roughMesh);
}

// Construct a Mesh from a 3ds file sequence
Mesh::Mesh(const std::string filepath, const int start, const int end):
m_model(NULL),
m_refMesh(NULL),
m_nbFaces(0),
m_currentFrame(0)
{
	m_type = "3D_MESH";
	std::vector<std::string> files = tool_filesystem::brute_open3dsFiles(filepath, start, end);
	for(unsigned int i=0; i<files.size(); ++i)
	{
		m_model = tool_filesystem::open3dsFile(files[i]);
		// File is OK, load the model and construct the mesh
		_loadDataFromFile();
		//_adaptMesh();                                                     
		// Add to the container per frame
		// There is only 1 entry since the model does not move
		m_meshes.push_back(m_mesh);
		m_roughMeshes.push_back(m_roughMesh);
	}
	// Put back the reader at top 
	m_roughMesh = m_roughMeshes.at(0);
	m_mesh = m_meshes.at(0);
	_generateBoidsFromMesh();
}

// Load Mesh data from file
void Mesh::_loadDataFromFile()
{
	// Pre-clean functions
	m_roughMesh.clear();
	m_mesh.clear();
	// Loop through all the meshes
	// (better to keep 1 per file)
	//@WARNING : do use an int for warning when build
	for(int iMesh=0; iMesh<m_model->nmeshes; ++iMesh)
	{
		// Double check file content is OK
		if(m_model->meshes[iMesh]->nfaces <= 1)                              
			// There is an empty mesh in the file
			continue;

		m_refMesh = m_model->meshes[iMesh];
		m_nbFaces = m_refMesh->nfaces;
		// Get bouding box    
		lib3ds_mesh_bounding_box(m_refMesh, m_boundingMin, m_boundingMax);        

		// Loop through every face
		//@WARNING : do use an int for warning on build
		for(int iFace=0; iFace<m_nbFaces; ++iFace)
		{
		    Lib3dsFace* face = &m_refMesh->faces[iFace];
		    for(unsigned int i=0; i<3; ++i)
		    {
			std::vector<float> point;
			for(unsigned int idx=0; idx<3; ++idx)
				point.push_back(m_refMesh->vertices[face->index[i]][idx] / 100.0f);

			//@WARNING Lib3ds max introduce a switch from Y and 
			point[1] = point[2];
			point[2] = m_refMesh->vertices[face->index[i]][1]/100.0f;

			// Use a set dramatically reduced the number of points
			// Else the same point is stored for each face
			m_mesh.insert(point);
			// Add this point to rough data
			m_roughMesh.push_back(point);
		    }
		}
	} 
}

// Generate boid field	
void Mesh::_generateBoidsFromMesh()
{
	m_group.clear();
	std::set< std::vector<float> >::iterator it;
	for (it=m_mesh.begin(); it!=m_mesh.end(); ++it)
	{
		// Always add the first one
		Boid b(0);			
		for(unsigned int j=0; j<3; ++j)
			b.setPosition(j, it->at(j));

 		// Add this point as a new Boid
		m_group.push_back(b);
		continue;
	}
}

// Adapt mesh to be between 0 and 1
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
	std::set< std::vector<float> >::iterator it;
	std::set< std::vector<float> > reducedMesh;
	for (it=m_mesh.begin(); it!=m_mesh.end(); ++it)
	{
		std::vector<float> reducedPoint;
		for(unsigned int j=0; j<3; ++j)
			reducedPoint.push_back(it->at(j)/ norm);
		reducedMesh.insert(reducedPoint);
	}
	m_mesh.clear();
	m_mesh = reducedMesh;
}

// Move : do nothing
//@WARNING virtual function, needs to be overwritten
void Mesh::move()
{
	if((unsigned int)m_currentFrame < m_meshes.size())
	{
		m_roughMesh = m_roughMeshes.at(m_currentFrame);
		m_mesh = m_meshes.at(m_currentFrame);
		_generateBoidsFromMesh();
		++m_currentFrame;
	}
	else
		m_currentFrame = 0;
} 

