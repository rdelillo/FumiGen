#include "Tools.hpp"

#include "Application.hpp"
#include "Camera.hpp"

#include <iostream>
#include <string.h>
#include <fstream>
#include <sstream>
#include <math.h>

namespace tool_geometry 
{

// To normalize a vector
void normalize (std::vector<float>& v)
{
	// Compute norm of the vector
	float norm = 0.0f;
	for(unsigned int idx=0; idx<3; ++idx)
		norm += pow(v[idx],2);
	norm = sqrt(norm);
	
	// Normalize the vector
	if (norm != 0.0f)
	{
		for(unsigned int idx=0; idx<3; ++idx)
			v[idx] /= norm;
	}
}

// To get the vector product
const std::vector<float> vectorProduct (const std::vector<float>& a,
					const std::vector<float>& b)
{
	std::vector<float> result;
	result.push_back(a[1]*b[2] - a[2]*b[1]);
	result.push_back(a[2]*b[0] - a[0]*b[2]);
	result.push_back(a[0]*b[1] - a[1]*b[0]);
	return result;
}

// Does the multiplication A=A*B : all the matrices are described column-major
void multMatrixBtoMatrixA(std::vector<float>& A, const std::vector<float>& B)
{
	int i = 0; 				// row index
	int j = 0; 				// column index
	std::vector<float> temp;		// temp values
	for(unsigned int i=0; i<16; ++i)
		temp.push_back(0.0f);
    
	for(unsigned int idx=0 ; idx<16 ; ++idx)
	{
		temp[idx] = 0.0f;
		i = idx%4;
		j = idx/4;

		for(unsigned int k=0 ; k<4 ; ++k)
		{
			const int idxA = k*4 + i;
			const int idxB = j*4 + k;
			temp[idx]+=A[idxA] * B[idxB];
		}
	}
	// Recopy temp into A
	for(unsigned int i=0 ; i<16 ; ++i)
		A[i] = temp[i];
}

// Sets the provided matrix to identity
void setToIdentity(std::vector<float>& matrix)
{
	float Id[] = {	1.0, 0.0, 0.0, 0.0,
			0.0, 1.0, 0.0, 0.0, 
			0.0, 0.0, 1.0, 0.0, 
		 	0.0, 0.0, 0.0, 1.0
		     };
	
	for(unsigned int i=0 ; i<16 ; ++i)
		matrix[i] = Id[i];
}

// Sets the provided matrix to a translate matrix on vector t
void setToTranslate(std::vector<float>& matrix, const std::vector<float>& t)
{
	float T[] = {	1.0,   0.0,   0.0,   0.0,
		 	0.0,   1.0,   0.0,   0.0,
		 	0.0,   0.0,   1.0,   0.0,
		 	t[0],  t[1],  t[2],  1.0
		    };
 
    	for(unsigned int i=0 ; i<16 ; ++i)
        	matrix[i] = T[i];
}

// Sets the provided matrix to a scale matrix by coeficients in s
void setToScale(std::vector<float>& matrix, const std::vector<float>& s)
{
    	float S[] = {	s[0], 0.0,  0.0,  0.0,
                 	0.0,  s[1], 0.0,  0.0,
                 	0.0,  0.0,  s[2], 0.0,
                 	0.0,  0.0,  0.0,  1.0
		    };  
    
	for(unsigned int i=0 ; i<16 ; ++i)
	        matrix[i] = S[i];
}

// Sets the provided matrix to a rotate matrix of angle "angle", around axis "axis"
void setToRotate(std::vector<float>& matrix, const float angle,
					     const std::vector<float>& axis)
{
	const float c = cos(angle);
	const float s = sin(angle);
	const float x = axis[0]; 
	const float y = axis[1]; 
	const float z = axis[2];
	float* R;

	// Rotation on X axis
	if((x==1.0f) && (y==0.0f) && (z==0.0f))
	{
		float RX[] = {	1.0, 0.0, 0.0, 0.0, 
			     	0.0, c,   s,   0.0, 
			     	0.0, -s,  c,   0.0, 
			     	0.0, 0.0, 0.0, 1.0
			     };
		R = RX;
	}
	// Rotation on Y axis
	else if ((x==0.0f) && (y==1.0f) && (z==0.0f))
	{                    
	    	float RY[] = {	c,   0.0, -s,  0.0, 
		         	0.0, 1.0, 0.0, 0.0, 
		         	s,   0.0, c,   0.0, 
		         	0.0, 0.0, 0.0, 1.0
			     };
		R = RY;
	}
	// Rotation on Z axis
	else if ((x==0.0f) && (y==0.0f) && (z==1.0f))
	{                                          
		float RZ[] = {	c,   s,   0.0, 0.0, 
		             	-s,  c,   0.0, 0.0, 
		             	0.0, 0.0, 1.0, 0.0, 
		             	0.0, 0.0, 0.0, 1.0
			     };
		R = RZ;
	}
	// Rotation on non-standard axis
	else
	{
		float ROther[] = {	(1.0f-c)*(x*x-1.0f) + 1.0f, (1.0f-c)*x*y + (z*s),       (1.0f-c)*x*z - (y*s),       0.0f, 
			      		(1.0f-c)*x*y - (z*s),       (1.0f-c)*(y*y-1.0f) + 1.0f, (1.0f-c)*y*z + (x*s),       0.0f, 
			      		(1.0f-c)*x*z + (y*s),       (1.0f-c)*y*z - (x*s),       (1.0f-c)*(z*z-1.0f) + 1.0f, 0.0f, 
			      		0.0f,                       0.0f,                       0.0f,                       1.0f
				 };
		R = ROther;
	}
	
	for(unsigned int i=0; i<16; ++i)
		matrix[i] = R[i];
}

// Builds a perspective projection matrix and stores it in mat
// l = left,
// r = right,
// b = bottom,
// t = top,
// n = near,
// f = far in the frustum
const std::vector<float> setPerspective(const float l, const float r, const float b,
					const float t, const float n, const float f)
{
	std::vector<float> mat;
	float P[] = {	(2*n)/(r-l), 	0.0, 		0.0, 		0.0,
		 	0.0, 		(2*n)/(t-b), 	0.0, 		0.0,
		 	(r+l)/(r-l), 	(t+b)/(t-b), 	-(f+n)/(f-n), 	-1.0,
		 	0.0, 		0.0, 		0.0, 		0.0
		    };

    	for(unsigned int i=0 ; i<16 ; ++i)
        	mat.push_back(P[i]);
	return mat;
}
} //namespace


namespace tool_camera
{

	// Draw the color triangle to test display
	void drawTestSample()
	{
		glBegin(GL_TRIANGLES);
		glColor3ub(255,0,0);    glVertex3d(-0.75,-0.75, -0.3f);
		glColor3ub(0,255,0);    glVertex3d(0,0.75, -0.3f);
		glColor3ub(0,0,255);    glVertex3d(0.75,-0.75, -0.3f);
		glEnd();
	}

	// Update the camera values according to keyboard, mouse
	void manageFps(const Application& app, Camera& camera)
	{
		// Work variables
		const float SPEED_MOVE = 0.5f;
		std::vector<float> appFlags;
		std::vector<float> cameraNewPos;
		std::vector<float> inverseCameraPos;
		std::vector<float> xAxis;
		std::vector<float> yAxis;
		// Initialize work vectors
		for(unsigned int i=0; i<3; ++i)
		{
			appFlags.push_back(0.0f);
			cameraNewPos.push_back(0.0f);
			inverseCameraPos.push_back(0.0f);
			xAxis.push_back(0.0f);
			yAxis.push_back(0.0f);
		}
		// Initialize axis vectors
		xAxis[0] = 1.0f;
		yAxis[1] = 1.0f;

		// Manage the Z property
		if(app.goingBackward() == app.goingForward())
			appFlags[2] = 0;
		else if(app.goingBackward()) 
			appFlags[2] = 1;
		else 
			appFlags[2] = -1;
		
		// Manage the X property
		if(app.goingLeft() == app.goingRight())
			appFlags[0] = 0;
		else if(app.goingLeft()) 
			appFlags[0] = -1;
		else 
			appFlags[0] = 1;


		const float moveOnX = appFlags[0]*SPEED_MOVE;
		const float moveOnY = appFlags[1]*SPEED_MOVE;
		const float moveOnZ = appFlags[2]*SPEED_MOVE;

		for(unsigned int i=0; i<3; ++i)
		{
			cameraNewPos[i] = camera.position(i);
			cameraNewPos[i] += camera.xAxis(i)*moveOnX;
			cameraNewPos[i] += camera.yAxis(i)*moveOnY;
			cameraNewPos[i] += camera.zAxis(i)*moveOnZ;
		}

		// Translate by inverse of new camera position
		for(unsigned int i=0; i<3; ++i)
			inverseCameraPos[i] = -1.0f*cameraNewPos[i];
		// Get long and lat values
		const float angleLong = app.xMousePosition() + M_PI;
		const float angleLat = app.yMousePosition() + M_PI/2.0f;
		

		// Prepare matrices
		std::vector<float> rotateAroundX;
		std::vector<float> rotateAroundY;
		std::vector<float> translate;
		for(unsigned int i=0; i<16; ++i)
		{
			rotateAroundX.push_back(0.0f);
			rotateAroundY.push_back(0.0f);
			translate.push_back(0.0f);
		}
		// Compute operations matrices
		tool_geometry::setToRotate(rotateAroundX, -1.0f*angleLat, xAxis);
		tool_geometry::setToRotate(rotateAroundY, angleLong, yAxis);
		tool_geometry::setToTranslate(translate, inverseCameraPos);
		// Compute new view matrix
		tool_geometry::setToIdentity(camera.view());
		tool_geometry::multMatrixBtoMatrixA(camera.view(), rotateAroundX);
		tool_geometry::multMatrixBtoMatrixA(camera.view(), rotateAroundY);
		tool_geometry::multMatrixBtoMatrixA(camera.view(), translate);

		// Update camera values
		for (unsigned int i=0 ; i<3 ; ++i)
		{
			// Updates the axis with values in view
			camera.setXAxis(i, camera.view()[i*4+0]);
			camera.setYAxis(i, camera.view()[i*4+1]);
			camera.setZAxis(i, camera.view()[i*4+2]);
			// Updates the position of the camera c
			camera.setPosition(i, cameraNewPos[i]);
		}
	}

// namespace
}

namespace tool_debug
{
	//Print a matrix on shell
	void printMatrix(const std::vector<float> matrix)
	{
		std::cout << "" << std::endl;
		for(unsigned int i=0; i<4; ++i)
		{
			std::cout << matrix[i+0]<<" " << matrix[i+1]<<" " << matrix[i+2]<<" " << matrix[i+3]<< std::endl;
		}
		std::cout << "" << std::endl;
	}
// namespace
}
