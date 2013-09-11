#include "Camera.hpp"
#include "Tools.hpp"
#include <math.h>

// Builder
Camera::Camera():
m_cameraMode("FPS"),
m_currentFrame(0)
{
	_init();
}

Camera::Camera(const std::string filepath, const int start, const int end):
m_cameraMode("FPS"),
m_currentFrame(0)
{
	_init();
	// Load 3ds file sequence
	std::vector<std::string> files = tool_filesystem::brute_open3dsFiles(filepath, start, end);
	for(unsigned int i=0; i<files.size(); ++i)
	{
		m_views.push_back(tool_camera::getModelviewFrom3dsFile(files[i]));
		m_rendermanViews.push_back(tool_camera::getRendermanTransformFrom3dsFile(files[i]));
	}
}

// Init camera with default values and first computations
void Camera::_init()
{
	// Initial position
	for(unsigned int i=0; i<3; ++i)
		m_position.push_back(0.0f);
	m_position[0] = 0.0f;
	m_position[1] = 0.70f;
	m_position[2] = 0.0f;

	// Initial parameters
	const float l = 1.0f;
	std::vector<float> c_aim;
	std::vector<float> c_up;
	for(unsigned int i=0; i<3; ++i)
	{
		c_aim.push_back(0.0f);
		c_up.push_back(0.0f);

		m_xAxis.push_back(0.0f);
		m_yAxis.push_back(0.0f);
		m_zAxis.push_back(0.0f);
	}
	c_up[1] = 1.0f;

	//Initialise the matrices
	for(unsigned int i=0; i<16; ++i)
	{
		m_view.push_back(0.0f);
		m_projection.push_back(0.0f);
	}
	
	// Initial directions
	m_left = -l;
	m_right = l;
	m_bottom = -l;
	m_top = l;
	m_near = 0.01;
	m_far = 100.0f*l;
	
	// Set default values to the matrices
	tool_geometry::setToIdentity(m_projection);
	updateProjection();
	lookAt(c_aim, c_up);
}

// Define the target from an aim vector and an up one
void Camera::lookAt(const std::vector<float>& aim, const std::vector<float>& up)
{
	for(unsigned int i=0 ; i<3; ++i)
	{
		m_yAxis[i] = up[i];	 		// y axis : up vector
		m_zAxis[i] = m_position[i] - aim[i]; 	// z axis : from aim to  position
	}

	// Compute opposite direction of axis z
	std::vector<float> minusZ;
	for(unsigned int i=0; i<3; ++i)
		minusZ.push_back(-1.0f*m_zAxis[i]);

	// new Axis x  : from axis y and axis z
	std::vector<float> newX = tool_geometry::vectorProduct(m_yAxis, m_zAxis);
	// new Axis y  : from new axis x and opposite of axis z
	std::vector<float> newY = tool_geometry::vectorProduct(newX, minusZ);
	
	// Recopy new values and normalize
	for(unsigned int i=0; i<3; ++i)
	{
		m_xAxis[i] = newX[i];
		m_yAxis[i] = newY[i];
	}
	tool_geometry::normalize(m_xAxis);
	tool_geometry::normalize(m_yAxis);
	tool_geometry::normalize(m_zAxis);
	
	// Builds the new view matrix
	updateView();
}

// Updates view matrix from the data
void Camera::updateView()
{
	std::vector<float> c_Rotate;
	std::vector<float> c_Translate;

	// Rotation to be aligned with correct  axis
	float Rotate[] = {	m_xAxis[0], m_yAxis[0], m_zAxis[0], 0.0f,
		     		m_xAxis[1], m_yAxis[1], m_zAxis[1], 0.0f,
		     		m_xAxis[2], m_yAxis[2], m_zAxis[2], 0.0f,
		     		0.0f, 	    0.0f, 	0.0f, 	    1.0f
		         };
	// Translation to be at the right distance from the scene
	float Translate[] = {	1.0f,           0.0f,           0.0f,           0.0f,
		     		0.0f,           1.0f,           0.0f,           0.0f,
		    	 	0.0f,           0.0f,           1.0f,           0.0f,
		     		-m_position[0], -m_position[1], -m_position[2], 1.0f
			    };
	// Put information into vector to be used
	for(unsigned int i=0; i<16; ++i)
	{
		c_Rotate.push_back(Rotate[i]);
		c_Translate.push_back(Translate[i]);
	}

	tool_geometry::setToIdentity(m_view);
	tool_geometry::multMatrixBtoMatrixA(m_view, c_Rotate);
	tool_geometry::multMatrixBtoMatrixA(m_view, c_Translate);
}

// Updates the projection matrix from the data
void Camera::updateProjection()
{
	const float l = m_left;
	const float r = m_right;
	const float b = m_bottom;
	const float t = m_top;
	const float n = m_near;
	const float f = m_far;

	// Perspective projection
	float P[] = {	(2.0f*n)/(r-l),  0.0f,            0.0f,               0.0f,
		  	0.0f,            (2.0f*n)/(t-b),  0.0f,               0.0f,
		 	(r+l)/(r-l),     (t+b)/(t-b),     -(f+n)/(f-n),       -1.0f,
		  	0.0f,            0.0f,            -(2.0f*f*n)/(f-n),  0.0f
		    };

	for (unsigned int i=0 ; i<16 ; ++i)
		m_projection[i] = P[i];
}

// Adjust the perspective (Fovy and image ratio)
void Camera::setPerspectiveFromAngle(const float fovy, const float aspectRatio)
{
	m_top = m_near * tan(fovy/2.0f);
	m_bottom = - m_top;
	m_left = m_bottom * aspectRatio;
	m_right = m_top * aspectRatio;

	updateProjection();
}

// Animate the Camera from 3ds file sequence
// Switch camera to PLAY mode (will automatically go back to FPS at the end)
void Camera::startPlayMode()
{
	if( m_cameraMode == "FPS")
		m_cameraMode = "PLAY";
	m_currentFrame = 0;		
}

// Move the camera based on mode
void Camera::move()
{
	if(m_currentFrame < m_views.size())
	{
		// Read the next position from the 
		// registered camera positions
		m_view = m_views[m_currentFrame];
		m_rendermanView = m_rendermanViews[m_currentFrame];
		++m_currentFrame;
	}
	else
	{
		if( m_cameraMode != "FPS")
			m_cameraMode = "FPS";	
		m_currentFrame = 0;
	}
}

// Utils
// Get camera position as float* for OpenGl
// View matrix
float* Camera::getViewf(float* modelView)
{
	for(unsigned int i=0; i<16; ++i)
		modelView[i] = m_view[i];
	return modelView;
}
// Projection matrix
float* Camera::getProjectionf(float* projection)
{
	for(unsigned int i=0; i<16; ++i)
		projection[i] = m_projection[i];
	return projection;
}	

// Get/set
// View matrix
std::vector<float>& Camera::view()
{
	return m_view;
}
// Get position of the camera
const float Camera::position(const int i) const
{
	return m_position[i];
}
// Set position of the camera
void Camera::setPosition(const int i, const float value)
{
	m_position[i] = value;
}
// X axis
// Get value for xAxis
const float Camera::xAxis(const int i) const
{
	return m_xAxis[i];
}
void Camera::setXAxis(const int i, const float value)
{
	m_xAxis[i] = value;
}
// Y axis
const float Camera::yAxis(const int i) const
{
	return m_yAxis[i];
}
void Camera::setYAxis(const int i, const float value)
{
	m_yAxis[i] = value;
}
// Z axis
const float Camera::zAxis(const int i) const
{
	return m_zAxis[i];
}
void Camera::setZAxis(const int i, const float value)
{
	m_zAxis[i] = value;
}
// Camera mode
const std::string Camera::getMode() const
{
	return m_cameraMode;
}
