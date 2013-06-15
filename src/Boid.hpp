#ifndef __BOID_HPP__
#define __BOID_HPP__

#include <ctime>
#include <cstdlib>
#include <iostream>
#include <vector>

#include "Boid.hpp"

class Boid
{
	private :
	int m_idBoid;				// id of the boid
	std::vector<float> m_position;		// current position of the boid
	std::vector<float> m_velocite;		// velocity of the boid
	int m_leaderShip;			// 1000 if leader, 1 else
	float m_intensity;			// 1 to 0
	
	public :
	
	// Usual Getters
	inline float const idBoid() const {return m_idBoid;};
	inline float const leaderShip() const {return m_leaderShip;};
	inline void setLeaderShip(const int value){m_leaderShip = value;};
	
	// Builder
	Boid(const int idBoid);
	
	// Move boid
	void move(const std::vector<float> newPosition, 
		  const std::vector<float > newVelocity );

	// Get boid position
	std::vector<float> getPosition();
	// Get/set boid position at
	const float position(const int i) const;
	void setPosition(const int i, const float v);
	// Get/set boid velocite at
	const float velocite(const int i) const;
	void setVelocite(const int i, const float v);
	// Get/set boid intensity
	const float intensity() const;
	void setIntensity(const float i);
};

#endif // __BOID_HPP__
