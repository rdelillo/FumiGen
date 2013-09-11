#ifndef __FIGURE_HPP__
#define __FIGURE_HPP__

#include <ctime>
#include <cstdlib>
#include <iostream>
#include <vector>
#include <string>

#include "Boid.hpp"

// Abstract class for Figure (explosion, Boids..)
// Defines the main function to overwrite by the new Process
class Figure
{
protected :
	std::vector<Boid> m_group;			// Contains all of the boids of the group
							// To move a Process to another we copy paste the group
	std::string m_type; 				// Type of the figure 
	std::string m_name;				// Name of the figure
	//Animated parameters
	std::vector<float> m_cameraMatrix;		// Store the camera matrix for Renderman
	unsigned int m_renderFrame;			// Current render frame 

public :
	// Usual
	inline Boid getBoid(const int i) { return m_group[i]; }
	inline const int size() const { return m_group.size(); }
	inline const bool isNeeded() const { return m_group.size() > 0 ; }
	inline const std::string type() const { return m_type; }
	inline void setName(const std::string name){ m_name = name; }
	inline const std::string name() const { return m_name; }

	// Builder
	Figure();
	// Animate the Figure
	virtual void move();
	// Draw - function OpenGL
	void brutalDraw();
	// Render - functions RenderMan
	void render();
	// Render - set render camera
	void setRenderCamera(const std::vector<float>& camera);
	// Render - reset the animation parameters
	void reset();
};

#endif // __FIGURE_HPP__
