#include "Explosion.hpp"

#include <math.h>

Explosion::Explosion()
{
	m_type = "EXPLOSION";
}

// Make an explosion from other Figures
//@WARNING: Destroy the previous figure
Explosion::Explosion(Figure* b)
{
	m_type = "EXPLOSION_FROM_" + b->type();
	// Copy the group
	for(int i=0; i<b->size(); ++i)
	{
		Boid newBoid(i);
		for(unsigned int idx=0; idx<3; ++idx)
			newBoid.setPosition(idx, b->getBoid(i).position(idx));
		newBoid.setIntensity(b->getBoid(i).intensity());
		m_group.push_back(newBoid);
	}
	// Compute the explosion origin
	_computeCenter();
	free(b);
}

// Move the group (animation)
void Explosion::move()
{
	//@WARNING: use an int cause loop in reverse order
	for(int i=m_group.size()-1; i>=0; --i)
	{	
		if( m_group[i].intensity() > 0.0f)
			_moveOneBoid(i);
		else
			m_group.erase(m_group.begin()+i);	
	}
}

// Compute origin of explosion
void Explosion::_computeCenter()
{
	float x = 0.0f;
	float y = 0.0f;
	float z = 0.0f;

	for(unsigned int i=0; i<m_group.size(); ++i)
	{
		x += m_group[i].position(0);
		y += m_group[i].position(1);
		z += m_group[i].position(2);
	}

	m_origin.push_back((float)(x/m_group.size()));
	m_origin.push_back((float)(y/m_group.size()));
	m_origin.push_back((float)(z/m_group.size()));
}

// Move one boid
void Explosion::_moveOneBoid(const int idBoid)
{
	// Compute distance from origin
	float norm = 0.0f;
	std::vector<float> distOrigin;
	for(unsigned int i=0; i<3; ++i)
	{
		distOrigin.push_back(m_group[idBoid].position(i)-m_origin[i]);
		norm += pow(distOrigin[i],2);
	}
	norm = sqrt(norm);
	for(unsigned int i=0; i<3; ++i)
		distOrigin[i] /= norm;

	// Work on norm as percentage
	norm/= 100.0;
	float explFactor = 0.0f;
	//@TODO: Ameliorate the explosion animation
	if(norm < 0.2)			// very close from impact
		explFactor = 5.0f;
	else if (norm < 0.6)		// close from impact
		explFactor = 2.0f;
	else				// not so close from impact
 		explFactor = 0.5f;
	
	explFactor *= EXPLOSION_RANDOM;
	// Compute new position
	for(unsigned int idx=0; idx<3; ++idx)
	{
		float newValue = m_group[idBoid].position(idx)+(distOrigin[idx]*explFactor);
		m_group[idBoid].setPosition(idx,newValue);
	}
	// Update intensity
	float updateIntensity = 1.0f/explFactor * EXPLOSION_INTENSITY;
	m_group[idBoid].setIntensity(m_group[idBoid].intensity()-updateIntensity);
}
