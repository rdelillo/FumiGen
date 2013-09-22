#include "Mesh.hpp"
#include "Boid.hpp"
#include "Tools.hpp"

#include <cstdlib>
#include <algorithm>

// Builder 
// Create a Mesh from an obj file
Mesh::Mesh(const std::string fileName, const float density):
m_model(NULL),
m_refMesh(NULL),
m_nbFaces(0),
m_currentFrame(0)
{
	m_type = "3D_MESH";
	m_density = density;
	m_model = tool_filesystem::open3dsFile(fileName);
	// File is OK, load the model and construct the mesh
	_loadDataFromFile();
	// Add to the container per frame
	// There is only 1 entry since the model does not move
	m_meshes.push_back(m_mesh);
	m_roughMeshes.push_back(m_roughMesh);
	_computeDensity();
	_generateBoidsFromMesh();
}

// Construct a Mesh from a 3ds file sequence
Mesh::Mesh(const std::string filepath, const int start, const int end, const float density):
m_model(NULL),
m_refMesh(NULL),
m_nbFaces(0),
m_currentFrame(0)
{
	m_type = "3D_MESH";
	m_density = density;
	std::vector<std::string> files = tool_filesystem::brute_open3dsFiles(filepath, start, end);
	for(unsigned int i=0; i<files.size(); ++i)
	{
		m_model = tool_filesystem::open3dsFile(files[i]);
		// File is OK, load the model and construct the mesh
		_loadDataFromFile();
		// Add to the container per frame
		// There is only 1 entry since the model does not move
		m_meshes.push_back(m_mesh);
		m_roughMeshes.push_back(m_roughMesh);
	}
	// Put back the reader at top 
	m_roughMesh = m_roughMeshes.at(0);
	m_mesh = m_meshes.at(0);
	_computeDensity();
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
	}
}

// Manage the density defined for the Mesh
void Mesh::_computeDensity()
{
	// Randomly remove point to respect density
	const int nbMeshes = m_mesh.size()*(float)m_density;
	while((int)m_mesh.size() > nbMeshes)
	{
		const float random = rand()/(float) RAND_MAX; 
		const int randIdx = m_mesh.size()*random;
		std::set< std::vector<float> >::iterator it0 = m_mesh.begin();
		for(int l=0; l<randIdx; ++l)
			++it0;
		m_mesh.erase(it0);
		for(unsigned int i=1; i<m_meshes.size(); ++i)
		{
			std::set< std::vector<float> >::iterator it = m_meshes[i].begin();
			for(int l=0; l<randIdx; ++l)
				++it;
			m_meshes[i].erase(it);
		}
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
	if((unsigned int)m_currentFrame+1 < m_meshes.size())
	{
		++m_currentFrame;
		m_roughMesh = m_roughMeshes.at(m_currentFrame);
		m_mesh = m_meshes.at(m_currentFrame);
		// We need to use the same boids else
		// the intensity changes
		std::set< std::vector<float> >::iterator it;
		unsigned int i = 0;
		for (it=m_mesh.begin(); it!=m_mesh.end(); ++it)
		{
			m_group[i].setPosition(0, it->at(0));
			m_group[i].setPosition(1, it->at(1));
			m_group[i].setPosition(2, it->at(2));
			++i;
		}
	}
	else
		m_currentFrame = 0;
} 

// Render - functions RenderMan
void Mesh::render()
{
	// Convert the rough mesh for Renderman
	std::vector<float> currentMesh;
	for(unsigned int i=0; i<m_roughMesh.size() ; ++i)
	{
		for(unsigned int idx=0; idx<3; ++idx)
			currentMesh.push_back(m_roughMesh.at(i).at(idx));
	}

	// Matte Pass
	std::string figureNameMatte = m_type + "_" + m_name + "_matte";
	tool_renderman::generateRIBHeader(figureNameMatte, m_renderFrame, m_cameraMatrix);
	RiWorldBegin();
	tool_renderman::shadeMeshMattePass();
	tool_renderman::renderMesh(currentMesh);
	tool_renderman::generateRIBFileFooter();

	// Skin Pass
	std::string figureNameSkin = m_type + "_" + m_name + "_skin";
	tool_renderman::generateRIBHeader(figureNameSkin, m_renderFrame, m_cameraMatrix);
	RiWorldBegin();
	float intensity = 1.0f;
	RiLightSource("distantlight", "intensity", &intensity, RI_NULL);
	tool_renderman::shadeMeshSkinPass();
	tool_renderman::renderMesh(currentMesh);
	tool_renderman::generateRIBFileFooter();

	// Reflect Pass
	std::string figureNameReflect = m_type + "_" + m_name + "_refect";
	tool_renderman::generateRIBHeader(figureNameReflect, m_renderFrame, m_cameraMatrix);
	RiWorldBegin();
	tool_renderman::shadeMeshReflectPass();
	tool_renderman::renderMesh(currentMesh);
	tool_renderman::generateRIBFileFooter();

	// Boids pass
	std::string figureNameBoid = m_type + "_" + m_name +"_boids";
	tool_renderman::generateRIBHeader(figureNameBoid, m_renderFrame, m_cameraMatrix);
	// Render specific for figures
	RiWorldBegin();
	for(unsigned int i=0; i<m_group.size(); ++i)
		tool_renderman::renderOneBoid(m_group[i]);
	tool_renderman::generateRIBFileFooter();

	++m_renderFrame;
}

