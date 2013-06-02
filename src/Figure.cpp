#include "Figure.hpp"

#include <GL/gl.h>
#include <GL/glu.h>

// Builder
Figure::Figure()
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

