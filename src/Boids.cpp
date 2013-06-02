#include "Boids.hpp"

// Builder
// nbUnits : how many units inside the group
Boids::Boids(const int nbUnits, const float sizeBox)
{
	m_type = "BOIDS_SYSTEM"; 
	// Construct a default origin to 0,0,0
	std::vector<float> origin ;
	for( unsigned int i=0; i<3; ++i )
		origin.push_back(0.0f);

	// Same as standard builder
	// Fill up global box constants
	c_sizeBox = sizeBox ;
	for( unsigned int i=0; i<3; ++i )
		c_origin.push_back(origin[i]);

	// Creation of the group
	// use an int to avoid warnings on build
	for(int i=0; i<nbUnits ; ++i)
	{
		// Create the new boid
		const int boidId = m_group.size();
		Boid b(boidId);
		m_group.push_back(b);
		computeInitialPosition(boidId);
	}
}

Boids::Boids(const int nbUnits, const float sizeBox, const std::vector<float> origin)
{
	// Fill up global box constants
	c_sizeBox = sizeBox ;
	for( unsigned int i=0; i<3; ++i )
		c_origin.push_back(origin[i]);

	// Creation of the group
	// use an int to avoid warnings on build
	for(int i=0; i<nbUnits ; ++i)
	{
		// Create the new boid
		const int boidId = m_group.size();
		Boid b(boidId);
		m_group.push_back(b);
		computeInitialPosition(boidId);
	}
}

// Abstract move function overwritten
void Boids::move()
{
	move_boids();
}

// Animate all of the boids of the system
void Boids::move_boids(float valC, float valA,
		 float valS, float valR)
{
	std::cout << "move Boids system" << std::endl;
	//@TODO: Animate leader
	m_group[0].setPosition(0,0.0f);
	m_group[0].setPosition(1,0.0f);
	m_group[0].setPosition(2,0.0f);

	
	//@WARNING
	// We only update the boid which are not leader
	// so explicitely forget the first one
	for(unsigned int i=1; i<m_group.size(); ++i)
		moveOneBoid(i, valC, valA, valS);
}


// Move one specific boid 
// idBoid : id of the specific boid to move
void Boids::moveOneBoid(const int idBoid, const float UserValueC, 
		 const float UserValueV, const float UserValueS)
{
	// Usual vector
	std::vector<float> v1;
	std::vector<float> v2;
	std::vector<float> v3;
	std::vector<float> newVelocity ;
	std::vector<float> newPosition ;
	std::vector<float> currentPosition;

	// Compute cohesion, alignment and separation
	v1 = cohesion(idBoid, UserValueC);
	v2 = align(idBoid, UserValueV);
	v3 = separation(idBoid, UserValueS);
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

