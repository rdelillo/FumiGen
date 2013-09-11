#include "Figure.hpp"
#include "Tools.hpp"

#include <ri.h>
#include <GL/gl.h>
#include <GL/glu.h>

// Builder
Figure::Figure():
m_renderFrame(0)
{
	m_type = "ABSTRACT_FIGURE"; 				
}

void Figure::move() {}

//Draw function OpenGL
void Figure::brutalDraw()
{
	std::cout << "brutal Draw - "<< m_type << "(" << \
				m_group.size() <<")"<< std::endl;
	for(unsigned int i=0; i<m_group.size(); ++i)
	{
		float x = m_group[i].position(0);
		float y = m_group[i].position(1);
		float z = m_group[i].position(2);
		
		glVertex3d(x,y,z);
	}
}

// Render - set render camera
void Figure::setRenderCamera(const std::vector<float>& camera)
{
	m_cameraMatrix.clear();
	for(unsigned int i=0; i<16; ++i)
		m_cameraMatrix.push_back(camera[i]);
}

// Render - reset the animation parameters
void Figure::reset()
{
	m_renderFrame = 0;
	m_cameraMatrix.clear();
}

// Render - functions RenderMan
void Figure::render()
{
	std::string figureName = m_type + "_" + m_name;
	tool_renderman::generateRIBHeader(figureName, m_renderFrame, m_cameraMatrix);
	// Render specific for figures
	RiWorldBegin();
	for(unsigned int i=0; i<m_group.size(); ++i)
		tool_renderman::renderOneBoid(m_group[i]);
	tool_renderman::generateRIBFileFooter();
	++m_renderFrame;
}

