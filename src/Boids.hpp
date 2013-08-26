#ifndef __BOIDS_HPP__
#define __BOIDS_HPP__

#include <lib3ds.h>
#include <ctime>
#include <cstdlib>
#include <iostream>
#include <vector>
#include "math.h"

#include "Figure.hpp"
#include "Boid.hpp"

// Usufull class to test colision
// when compute boids positions
struct AABB3D
{
  float x,y,z;	// origin
  float w,h,d;	// box size
};

class Boids : public Figure
{
	private :

	// Constant for gloabl box (cube)
	std::vector<float> c_origin;			// origin
	float c_sizeBox ;				// width = height = deep

	// Constants for individual boxes
	static const float c_widthOneBoid = 1.0f;	// width	
	static const float c_heigthOneBoid = 0.2f;	// height
	static const float c_deepOneBoid = 1.0f;	// deep

	// Animation parameters
	unsigned int m_currentFrame;				// current frame (default 0)
	std::vector< std::vector<float> > m_leaderPositions; 	// position of the leader boid on time

	public :

	// Builder
	// Construct a boids system from nowhere
	Boids(const int nbUnits, const float sizeBox=5.0f);
	Boids(const int nbUnits, const std::vector<float> origin, const float sizeBox=5.0f);
	// Construct a boids system with an animated leader
	Boids(const int nbUnits, const std::string filepath, const int start, const int end, const float sizeBox=5.0f);
	// Construct a boids system from Mesh or something else
	Boids(Figure* b, const std::string filepath="", const int start=0, const int end=0);

	// Move the group (animation)
	void move_boids(float valC = 40.0, float valA = 10,
		  float valS = 0.02, float valR = 7.5);

	void move();
	
	private:
	// Init boid system
	void _init(const int nbUnits, const int sizeBox);
	// Read the position information for the leader and build animated parameters
	void _readLeaderInformation(const std::string filepath, const int start, const int end);
	// Compute the initial position for a specific boid
	void computeInitialPosition(const int idBoid);
	// Is one boid into vital space of another one
	const bool isIntoVitalSpace(const int idBoid);	

	// Move on boid
	void moveOneBoid(const int idBoid, const float UserValueC, 
		         const float UserValueV, const float UserValueS, const float UserValueR);
	// Compute cohesion (boid closed to each others)
	std::vector<float> cohesion(const int idBoid, const float UserValueC);
	// Compute separation (boid far to each others)
	std::vector<float> separation(const int idBoid, const float UserValueS);
	// Compute alignement (moderate velocity to each others)
	std::vector<float> align(const int idBoid, const float UserValueV);
	// Compute reduction and limit box for a boid
	std::vector<float> limiteBox(const int idBoid, const float UserValueR);
};

// Compute colision between 2 cubic boxes	
const bool collision(const AABB3D box1, const AABB3D box2);

#endif // __BOIDS_HPP__
