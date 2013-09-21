#include "Boid.hpp"

#include <cstdlib>

// Builder
Boid::Boid(const int idBoid)
{
	m_idBoid = idBoid;
	m_intensity = 1.0f;
	m_size = rand()/(float) RAND_MAX;
	// Define basic position in origin
	for(unsigned int i=0; i<3; ++i)
	{
		m_position.push_back(0.0f);
		m_velocite.push_back(0.0f);
	}
	// Manage leaderShip
	if(m_idBoid == 0)
		m_leaderShip = 1000;
	else
		m_leaderShip = 1;
}

// Move boid
void Boid::move(const std::vector<float> newPosition, 
	  	const std::vector<float> newVelocity )
{
	for(unsigned int i=0; i<3; ++i)
	{
		m_position[i] = newPosition[i];
		m_velocite[i] = newVelocity[i];
	}
}

// Get boid position
std::vector<float> Boid::getPosition()
{
	std::vector<float> returnPosition;
	for(unsigned int i=0; i<3; ++i)
		returnPosition.push_back(m_position[i]);	
	
	return returnPosition;
}

// Get/set boid position at
const float Boid::position(const int i) const { return m_position[i]; }
void Boid::setPosition(const int i, const float v) { m_position[i] = v; }
// Get/set boid velocite at
const float Boid::velocite(const int i) const { return m_velocite[i]; }
void Boid::setVelocite(const int i, const float v) { m_velocite[i] = v; }
// Get/set boid intensity
const float Boid::intensity() const { return m_intensity; }
void Boid::setIntensity(const float i) { m_intensity = i; }
