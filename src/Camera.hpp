#ifndef __CAMERA_HPP__
#define __CAMERA_HPP__

#include <vector>
#include <string>

class Camera
{
private :
	// Usual
	std::vector<float> m_position; 	// Camera position
        std::vector<float> m_xAxis; 	// Camera axis x : right side
        std::vector<float> m_yAxis; 	// Camera axis y : up
        std::vector<float> m_zAxis; 	// Camera axis z : backward

	// Frustrum parameters
        float m_left; 			// x coord from center to left plane of frustum
        float m_right; 			// x coord from center to right plane of frustum
        float m_bottom; 		// y coord from center to bottom plane of frustum
        float m_top; 			// y coord from center to top plane of frustum
        float m_near;			// near (frustrum value)
        float m_far;			// far (frustrum value)	

	// Matrices
        std::vector<float> m_view;	// view matrix
        std::vector<float> m_projection;// projection matrix

	// Animation parameters
	//@TODO: temporary stored the Play/Pause flag into camera, will
	// need to be moved into Application
	std::string m_cameraMode;			// FPS or PLAY
	unsigned int m_currentFrame;				// current frame displayed on screen
	std::vector< std::vector<float> > m_views; 	// view matrixes from 3ds files sequence

public :
	// Builder
        Camera();
	Camera(const std::string filepath, const int start, const int end);

	// Update the Camera
	// Update the view matrix
        void updateView();
	// Update the projection matrix
        void updateProjection();
	// Change camera perspective
        void setPerspectiveFromAngle(const float fovy=0.75, const float aspectRatio=1.777);
	// Update the camera target
        void lookAt(const std::vector<float>& aim, const std::vector<float>& up);

	// Animate the Camera from 3ds file sequence
	// Switch camera to PLAY mode (will automatically go back to FPS at the end)
	void startPlayMode();
	// Move the camera based on mode
	void move();

	// Utils
	// Get camera matrix as float* for openGL
	float* getViewf(float* modelView);
	float* getProjectionf(float* projection);
	
	// Get/set
	// Camera mode
	const std::string getMode() const;
	// View matrix
	std::vector<float>& view();
	// Position
	const float position(const int i) const;
	void setPosition(const int i, const float value);
	// X axis
	const float xAxis(const int i) const;
	void setXAxis(const int i, const float value);
	// Y axis
	const float yAxis(const int i) const;
	void setYAxis(const int i, const float value);
	// Z axis
	const float zAxis(const int i) const;
	void setZAxis(const int i, const float value);
private:
	// Init camera with default values and first computations
	void _init();
};


#endif
