#include "Tools.hpp"

#include "Application.hpp"
#include "Camera.hpp"

#include <iostream>
#include <iomanip>
#include <string.h>
#include <fstream>
#include <sstream>
#include <math.h>
#include <set>

// Linking issues with Boost
// #include <boost/filesystem.hpp>
// #include <boost/algorithm/string/predicate.hpp>

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
			float ROther[] = {(1.0f-c)*(x*x-1.0f) + 1.0f, (1.0f-c)*x*y + (z*s),       (1.0f-c)*x*z - (y*s),       0.0f, 
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
//namespace
} 


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

	// Draw a simple color scene to test display
	void drawTestScene()
	{
		// Floor 
		glBegin(GL_QUADS);
		glColor4ub(255, 0, 0, 100); 
		glVertex3d(-10.0f, 0.0f, -10.0f);
		glVertex3d(10.0f, 0.0f, -10.0f);
		glVertex3d(10.0f, 0.0f, 10.0f);
		glVertex3d(-10.0f, 0.0f, 10.0f);
		glEnd();
		// Top
		glBegin(GL_QUADS);
		glColor4ub(0, 255, 0, 100); 
		glVertex3d(-3.0f, 0.0f, -10.0f);
		glVertex3d(-3.0f, 3.0f, -10.0f);
		glVertex3d(3.0f, 3.0f, -10.0f);
		glVertex3d(3.0f, 0.0f, -10.0f);
		glEnd();
		// Left
		glBegin(GL_QUADS);
		glColor4ub(0, 0, 255, 100); 
		glVertex3d(-10.0f, 0.0f, -3.0f);
		glVertex3d(-10.0f, 3.0f, -3.0f);
		glVertex3d(-10.0f, 3.0f, 3.0f);
		glVertex3d(-10.0f, 0.0f, 3.0f);
		glEnd();
		// Center
		glBegin(GL_QUADS);
		glColor4ub(255, 255, 255, 100); 
		glVertex3d(-0.5f, 0.5f, -0.5f);
		glVertex3d(0.5f, 0.5f, -0.5f);
		glVertex3d(0.5f, 0.5f, 0.5f);
		glVertex3d(-0.5f, 0.5f, 0.5f);
		glEnd();
	}

	// Update the camera values according to keyboard, mouse
	void manageFps(const Application& app, Camera& camera)
	{
		// Work variables
		const float SPEED_MOVE = 0.25f;
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

namespace tool_filesystem
{
	/*
	// Open 3ds file sequence
	std::vector<std::string> open3dsFiles(const std::string& path)
	{
		std::vector<std::string> match_files;
		// Check if given file is a file
		boost::filesystem::path _p(path.c_str());
		// Providen path does not exist
		if(!boost::filesystem::exists(_p))
			std::cout << "Invalid given path to construct mesh " << path << std::endl;
		// Providen path is a single file
		else if(boost::filesystem::is_regular_file(_p))
			match_files.push_back(path);
		// Providen path is a directory
		// Try to find all of the 3ds files inside it
		else if(boost::filesystem::is_directory(_p))
		{
			// Sort the files found base on name
        		std::vector<boost::filesystem::path> sorted_paths;
        		copy(boost::filesystem::directory_iterator(_p),
			     boost::filesystem::directory_iterator(),
			     std::back_inserter(sorted_paths));
        		std::sort(sorted_paths.begin(), sorted_paths.end()); 
			// From stackoverflow 
			for(unsigned int i=0; i<sorted_paths.size(); ++i)
			{
				std::string filename= sorted_paths[i].generic_string();
				// Check found file is OK finishing with the 3ds extension
				if(boost::algorithm::ends_with(filename,".3ds"))
					match_files.push_back(filename);
			}
		}
		return match_files;
	}
	*/

	// Brute open 3ds file sequence since the boost
	// file system library does not appear to work
	// on my station (linking issues)
	std::vector<std::string> brute_open3dsFiles(const std::string& path, const int start_seq, const int end_seq)
	{
		std::vector<std::string> match_files;
		for(int i=start_seq; i<=end_seq; ++i)
		{
			std::string temp_path = path;
			std::ostringstream oss;
			// Force a pad 4 to the number
			oss << std::setfill('0') << std::setw(4) << i;
			temp_path += "."+ oss.str() + ".3ds";
			match_files.push_back(temp_path);
		} 
		return match_files;
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
