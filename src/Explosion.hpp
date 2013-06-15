#ifndef __EXPLOSION_HPP__
#define __EXPLOSION_HPP__

#include <ctime>
#include <cstdlib>
#include <iostream>
#include <vector>

#include "Figure.hpp"
#include "Boid.hpp"

class Explosion : public Figure
{
private :
	std::vector<float> m_origin;			// origin of the explosion
	const static float EXPLOSION_RANDOM = 0.007f;	// affects position
	const static float EXPLOSION_INTENSITY = 0.001f;// affects intensity change

public :
	// Builder
	// Construct a boids system from nowhere
	Explosion();

	// Make an explosion from other Figures
	//@WARNING: Destroy the previous figure
	Explosion(Figure* b);

	// Move the group (animation)
	void move();
	
private:
	// Compute origin of explosion
	void _computeCenter();
	// Move one boid
	void _moveOneBoid(const int idBoid);
};

#endif // __EXPLOSION_HPP__
