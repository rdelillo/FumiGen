#include "Boids.hpp"
#include "Tools.hpp"

// Builder
// nbUnits : how many units inside the group
Boids::Boids(const int nbUnits, const float sizeBox):
m_currentFrame(0)
{
	m_type = "BOIDS_SYSTEM"; 
	// Construct a default origin to 0,0,0
	for( unsigned int i=0; i<3; ++i )
		c_origin.push_back(0.0f);

	_init(nbUnits, sizeBox);
}

Boids::Boids(const int nbUnits, const std::vector<float> origin, const float sizeBox):
m_currentFrame(0)
{
	m_type = "BOIDS_SYSTEM";
	// Set boid system origin
	for(unsigned int i=0; i<3; ++i)
		c_origin.push_back(origin[i]);

	_init(nbUnits, sizeBox);
}

// Construct a boids system with an animated leader
Boids::Boids(const int nbUnits, const std::string filepath, const int start, const int end, const float sizeBox):
m_currentFrame(0)
{
	m_type = "BOIDS_SYSTEM";
	_readLeaderInformation(filepath, start, end);
	// Init boids
	_init(nbUnits, sizeBox);
}

// Construct a boids system from Mesh or something else - with animated leader
//@WARNING: Destroy the previous figure
Boids::Boids(Figure* b, const std::string filepath, const int start, const int end)
{
	// Check if given Figure is already a Boids system
	if(b->type() == "BOIDS_SYSTEM")
		std::cout << "WARNNING given figure i already a Boids system" << std::endl;

	m_type = "BOIDS_SYSTEM_FROM_" + b->type();

	// Generate the animated parameters if providen
	if(filepath != "" && end != 0)
		_readLeaderInformation(filepath, start, end);

	//@WARNING use an int to prevent from warning
	for(int i=0; i<b->size(); ++i)
	{
		Boid newBoid(i);
		for(unsigned int idx=0; idx<3; ++idx)
			newBoid.setPosition(idx, b->getBoid(i).position(idx));
		newBoid.setIntensity(b->getBoid(i).intensity());
		m_group.push_back(newBoid);
	}
	free(b);
}

// Init boid system
void Boids::_init(const int nbUnits, const int sizeBox)
{
	m_group.clear();
	// Fill up global box constants
	c_sizeBox = sizeBox ;

	// Creation of the group
	//@WARNING use an int to avoid warnings on build
	for(int i=0; i<nbUnits ; ++i)
	{
		// Create the new boid
		const int boidId = m_group.size();
		Boid b(boidId);
		m_group.push_back(b);
		computeInitialPosition(boidId);
	}
	
	if(m_group.size() > 0)
		// Set leader 
		m_group[0].setLeaderShip(1000); 
	else
		std::cout << "Empty boids system" << std::endl;
}

// Read the position information for the leader and build animated parameters
void Boids::_readLeaderInformation(const std::string filepath, const int start, const int end)
{
	// Read providen file sequence
	std::vector<std::string> files = tool_filesystem::brute_open3dsFiles(filepath, start, end);
	for(unsigned int file_index=0; file_index<files.size(); ++file_index)
	{
		// Check current file
		// There can be more than 1 mesh in the providen 3ds file sequence
		Lib3dsFile * l_file =  tool_filesystem::open3dsFile(files[file_index]);
		
		int mesh_index = 0;
		bool mesh_found = false;
		while(mesh_index != l_file->nmeshes && !mesh_found)
		{
			Lib3dsMesh * l_mesh = l_file->meshes[mesh_index];
			// Mesh is not empty
			if(l_mesh->nfaces >= 1)                          
			{
				// Get center of mesh box 
				float x,y,z;
				x = y = z = 0.0f;
				unsigned int nbPoints = 0;
				for(unsigned int iFace=0; iFace<l_mesh->nfaces; ++iFace)
				{
				    	Lib3dsFace* face = &l_mesh->faces[iFace];
					for(unsigned int iPoint=0; iPoint<3; ++iPoint)
					{
						x+= l_mesh->vertices[face->index[iPoint]][0];
						y+= l_mesh->vertices[face->index[iPoint]][1];
						z+= l_mesh->vertices[face->index[iPoint]][2];
						++nbPoints;
					}
				}
				
				// Compute center of mesh
				std::vector<float> centerMesh;
				centerMesh.push_back((x/(float)nbPoints)/100.0f);
				centerMesh.push_back((y/(float)nbPoints)/100.0f);	
				centerMesh.push_back((z/(float)nbPoints)/100.0f);    

				// Add to animation leader position
				m_leaderPositions.push_back(centerMesh);
				mesh_found = true;
			}
			else
				++mesh_index;
		}
	}	
	// Origin is the leader position at first frame
	for(unsigned int i=0; i<3; ++i)
		c_origin.push_back(m_leaderPositions[0][i]);
}

// Abstract move function overwritten
void Boids::move()
{
	if(m_currentFrame < m_leaderPositions.size())
	{
		// Move the leader
		for(unsigned int i=0; i<3; ++i)
			m_group[0].setPosition(i, m_leaderPositions[m_currentFrame][i]);
		// Move the other boids
		move_boids();
		++m_currentFrame;
	}
	else
	{
		m_currentFrame = 0;
		// Recreate boids from nowhere
		_init(m_group.size(), c_sizeBox);
	}

}

// Boids Move
// Animate all of the boids of the system (except leader)
void Boids::move_boids(float valC, float valA,
		 float valS, float valR)
{		
	//@WARNING
	// We only update the boid which are not leader
	// so explicitely forget the first one
	for(unsigned int i=1; i<m_group.size(); ++i)
		moveOneBoid(i, valC, valA, valS, valR);
}


// Move one specific boid 
// idBoid : id of the specific boid to move
void Boids::moveOneBoid(const int idBoid, const float UserValueC, 
		 const float UserValueV, const float UserValueS, const float UserValueR)
{
	// Usual vector
	std::vector<float> v1;
	std::vector<float> v2;
	std::vector<float> v3;
	std::vector<float> v4;
	std::vector<float> newVelocity ;
	std::vector<float> newPosition ;
	std::vector<float> currentPosition;

	// Compute cohesion, alignment and separation
	v1 = cohesion(idBoid, UserValueC);
	v2 = align(idBoid, UserValueV);
	v3 = separation(idBoid, UserValueS);
	v4 = limiteBox(idBoid, UserValueR);
	currentPosition = m_group[idBoid].getPosition();
	
	for(unsigned int i=0; i<3; ++i)
		newVelocity.push_back(v1[i] + v2[i] +v3[i]);	
	for(unsigned int i=0; i<3; ++i)
		newPosition.push_back(currentPosition[i]+newVelocity[i]);

	// Update the boid values
	m_group[idBoid].move(newPosition, newVelocity);
}

// Compute the initial position for a specific boid
// idBoid : id of the specific boid to setup
// sizeBox : size of the box from origin
void Boids::computeInitialPosition(const int idBoid)
{
	// Usual position variables
	float x;
	float y;
	float z;
	// Try new random position while is not good 
	do
	{
		x = (float) rand()/RAND_MAX * c_sizeBox;
		y = (float) rand()/RAND_MAX * c_sizeBox;
		z = (float) rand()/RAND_MAX * c_sizeBox;
		m_group[idBoid].setPosition(0, c_origin[0]+x);
		m_group[idBoid].setPosition(1, c_origin[1]+y);
		m_group[idBoid].setPosition(2, c_origin[2]+z);
	}
	while( isIntoVitalSpace(idBoid) );
}

// Is one boid into vital space of another one
// idBoid : id of the specific boid to test
const bool Boids::isIntoVitalSpace(const int idBoid)
{
	// Define two cubic boxes
	AABB3D b1;
	AABB3D b2;
	// Compute and fill up box for given boid
	b1.x = m_group[idBoid].position(0);
	b1.y = m_group[idBoid].position(1);
	b1.z = m_group[idBoid].position(2);
	b1.w = c_widthOneBoid;
	b1.h = c_heigthOneBoid;
	b1.d = c_deepOneBoid;
	// For each other boids compute box
	// and test colision
	// use an int to avoid warnins on build
	for(int i = 0; i < idBoid; ++i)
	{
		if(i != idBoid)
		{
			b2.x = m_group[i].position(0);
			b2.y = m_group[i].position(1);
			b2.z = m_group[i].position(2);
			b2.w = c_widthOneBoid;
			b2.h = c_heigthOneBoid;
			b2.d = c_deepOneBoid;
			
			if( collision(b1, b2) )
				return true;
		}	
	}
	return false;	
}

// Compute cohesion (boid closed to each others)
std::vector<float> Boids::cohesion(const int idBoid, const float UserValueC)
{
	std::vector<float> center;
	for(unsigned int i=0; i<3; ++i)
		center.push_back(0.0f);
	
	float div = 0.0;	
	for(unsigned int i=0; i< m_group.size(); ++i)
	{
		if(m_group[i].idBoid() != idBoid)
		{
			center[0] += m_group[i].position(0)*m_group[i].leaderShip(); 
			center[1] += m_group[i].position(1)*m_group[i].leaderShip(); 
			center[2] += m_group[i].position(2)*m_group[i].leaderShip(); 
			div += m_group[i].leaderShip();
		}
	}
	for(unsigned int i=0; i<3; ++i)
	{
		center[i] /= div-1;
		center[i] = (center[i]-m_group[idBoid].position(i))/UserValueC;
	}
	return center;
}

// Compute separation (boid far to each others)
std::vector<float> Boids::separation(const int idBoid, const float UserValueS)
{
	std::vector<float> separation;
	for(unsigned int i=0; i<3; i++)
		separation.push_back(0.0);
		
	for(unsigned int i=0; i< m_group.size(); i++)
	{
		if(m_group[i].idBoid() != idBoid)
		{
			for(unsigned int indice = 0; indice < 3; ++indice)
			{
				if(fabs(m_group[i].position(indice) - m_group[idBoid].position(indice)) < UserValueS)
				{
					separation[indice] -= m_group[i].position(indice) - m_group[idBoid].position(indice);
				}
			}
		}
	}
	return separation;
}

// Compute alignement (moderate velocity to each others)
std::vector<float> Boids::align(const int idBoid, const float UserValueV)
{
	std::vector<float> velocity;
	for(unsigned int i=0; i<3; ++i)
		velocity.push_back(0.0);
	
	float div = 0.0;
	for(unsigned int i=0; i< m_group.size(); ++i)
	{
		if(m_group[i].idBoid() != idBoid)
		{
			velocity[0] += m_group[i].velocite(0)*m_group[i].leaderShip(); 
			velocity[1] += m_group[i].velocite(1)*m_group[i].leaderShip(); 
			velocity[2] += m_group[i].velocite(2)*m_group[i].leaderShip(); 
			div += m_group[i].leaderShip();
		}
	}
	
	for(unsigned int i=0; i<3; ++i)
	{
		velocity[i] /= div-1;
		velocity[i] = (velocity[i]-m_group[idBoid].velocite(i))/UserValueV;
	}
	return velocity;
}

// Reduce limit box for a boid
std::vector<float> Boids::limiteBox(const int idBoid, const float UserValueR)
{
	
	std::vector<float> center;
	for(unsigned int i=0; i<3; i++)
		center.push_back(0.0);
		
	std::vector<float> decalage;
	for(unsigned int i=0; i<3; i++)
		decalage.push_back(0.0);

	for(unsigned int idx=0; idx<3; ++idx)
	{
		if(m_group[idBoid].position(idx) < (center[idx] - UserValueR))
			decalage[idx] = -(m_group[idBoid].velocite(idx) * 2);
		else if(m_group[idBoid].position(idx) > (center[0] + UserValueR))
			decalage[idx] = -(m_group[idBoid].velocite(idx) * 2);
	}
	
	return decalage;
}


// Compute colision between 2 cubic boxes	
const bool collision(const AABB3D box1, const AABB3D box2)
{
   if((box2.x >= box1.x + box1.w) 	// bad on right
	|| (box2.x + box2.w <= box1.x) 	// bad on left
	|| (box2.y >= box1.y + box1.h) 	// bad on bottom
	|| (box2.y + box2.h <= box1.y)  // bad on top	
    	|| (box2.z >= box1.z + box1.d)  // bad on behind
	|| (box2.z + box2.d <= box1.z)) // bad on front
          return false; 
   else
          return true;  
}

