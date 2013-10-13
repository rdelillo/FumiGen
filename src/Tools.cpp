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
		glVertex3d(-3.0f, 0.0f, 10.0f);
		glVertex3d(-3.0f, 3.0f, 10.0f);
		glVertex3d(3.0f, 3.0f, 10.0f);
		glVertex3d(3.0f, 0.0f, 10.0f);
		glEnd();
		// Bottom
		glBegin(GL_QUADS);
		glColor4ub(255, 255, 0, 100); 
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
		glVertex3d(-0.5f, 1.0f, -0.5f);
		glVertex3d(0.5f, 1.0f, -0.5f);
		glVertex3d(0.5f, 1.0f, 0.5f);
		glVertex3d(-0.5f, 1.0f, 0.5f);
		glEnd();
	}

	// Update the camera values according to keyboard, mouse
	void manageFps(const Application& app, Camera * camera)
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
			cameraNewPos[i] = camera->position(i);
			cameraNewPos[i] += camera->xAxis(i)*moveOnX;
			cameraNewPos[i] += camera->yAxis(i)*moveOnY;
			cameraNewPos[i] += camera->zAxis(i)*moveOnZ;
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
		tool_geometry::setToIdentity(camera->view());
		tool_geometry::multMatrixBtoMatrixA(camera->view(), rotateAroundX);
		tool_geometry::multMatrixBtoMatrixA(camera->view(), rotateAroundY);
		tool_geometry::multMatrixBtoMatrixA(camera->view(), translate);

		// Update camera values
		for (unsigned int i=0 ; i<3 ; ++i)
		{
			// Updates the axis with values in view
			camera->setXAxis(i, camera->view()[i*4+0]);
			camera->setYAxis(i, camera->view()[i*4+1]);
			camera->setZAxis(i, camera->view()[i*4+2]);
			// Updates the position of the camera c
			camera->setPosition(i, cameraNewPos[i]);
		}
	}

	// Helper : Fixed version of lib3ds_matrix_camera
	// Y and Z axis were wrong in lib3ds (confusion cause switched)
	// http://lib3ds.sourceforge.net/lib3ds-1.2.0/doc/html/matrix_8c-source.html#l00403
	// http://www.songho.ca/opengl/gl_transform.html#modelview
	std::vector<float> lib3ds_matrix_camera_fixed(float* pos, float* tgt, float roll)
	{
		// Initialize needed matrices
		std::vector<float> M;	// Target matrix
		std::vector<float> R;	// Roll matrix
		for(unsigned int i=0; i<16; ++i)
		{
			M.push_back(0.0f);
			R.push_back(0.0f);
		}

		// Initialize needed camera vectors
		std::vector<float> x;	// camera left vector
		std::vector<float> y;	// camera up
		std::vector<float> z;	// camera forward
		for(unsigned int i=0; i<3; ++i)
		{
			x.push_back(0.0f);
			y.push_back(0.0f);
			z.push_back(0.0f);
		}
		y[1] = 1.0f;	// we can assume up here to be standard
	
		// Compute camera forward 
		for(unsigned int i=0; i<3; ++i)
			z[i] = pos[i] - tgt[i];
		tool_geometry::normalize(z);
		// Compute camera up and left
		x = tool_geometry::vectorProduct(z, y);
		y = tool_geometry::vectorProduct(x, z);
		tool_geometry::normalize(x);
		tool_geometry::normalize(y);

		// Target matrix
		tool_geometry::setToIdentity(M);
		M[0] = x[0];
		M[1] = x[1];
		M[2] = x[2];
		M[4] = y[0];
		M[5] = y[1];
		M[6] = y[2];
		M[8] = z[0];
		M[9] = z[1];
		M[10] = z[2];

		// Create the Roll matrix
		tool_geometry::setToIdentity(R);
		tool_geometry::setToRotate(R, -roll, z);
		// Merge Target with Roll
		tool_geometry::multMatrixBtoMatrixA(R, M);
		// Apply the camera Translate operation
		for(unsigned int i=0; i<3; ++i)
			R[12+i] = R[i]*(-pos[0]) + R[4+i]*(-pos[1]) + R[8+i]*(-pos[2]);
		return R;
	}

	// Helper : Fixed version of lib3ds_matrix_camera for Renderman
	// As opposite to OpenGL the renderman camera transform is applied
	// on all of the object of the scene. We need to apply the opposite transformation
	std::vector<float> lib3ds_matrix_camera_renderman(float* pos, float* tgt, float roll)
	{
		// Initialize needed matrices
		std::vector<float> M;	// Target matrix
		std::vector<float> R;	// Roll matrix
		for(unsigned int i=0; i<16; ++i)
		{
			M.push_back(0.0f);
			R.push_back(0.0f);
		}

		// Initialize needed camera vectors
		std::vector<float> x;	// camera left vector
		std::vector<float> y;	// camera up
		std::vector<float> z;	// camera forward
		for(unsigned int i=0; i<3; ++i)
		{
			x.push_back(0.0f);
			y.push_back(0.0f);
			z.push_back(0.0f);
		}
		y[1] = 1.0f;	// we can assume up here to be standard
	
		// Compute camera forward 
		for(unsigned int i=0; i<3; ++i)
			z[i] = pos[i]-tgt[i];
		tool_geometry::normalize(z);
		// Compute camera up and left
		x = tool_geometry::vectorProduct(z, y);
		y = tool_geometry::vectorProduct(x, z);
		tool_geometry::normalize(x);
		tool_geometry::normalize(y);

		// Target matrix
		tool_geometry::setToIdentity(M);
		M[0] = -x[0];
		M[1] = -x[1];
		M[2] = -x[2];
		M[4] = -y[0];
		M[5] = -y[1];
		M[6] = -y[2];
		M[8] = -z[0];
		M[9] = -z[1];
		M[10] = -z[2];

		// Operate the Z rotation on revert axis
		for(unsigned int i=0; i<3; ++i)
			z[i] *= -1.0f;

		// Create the Roll matrix
		tool_geometry::setToIdentity(R);
		tool_geometry::setToRotate(R, -roll, z);
		// Merge Target with Roll
		tool_geometry::multMatrixBtoMatrixA(R, M);
		// Apply the camera Translate operation
		for(unsigned int i=0; i<3; ++i)
			R[12+i] = R[i]*(-pos[0]) + R[4+i]*(-pos[1]) + R[8+i]*(-pos[2]);
		return R;
	}

	// Import camera modelview from 3ds file (OpenGL)
	std::vector<float> getModelviewFrom3dsFile(const std::string& file)
	{
		// Load camera from 3ds file
		Lib3dsCamera * camera;
		Lib3dsFile * l_file =  tool_filesystem::open3dsFile(file);
		// Check it contains only 1 camera
		if(l_file->ncameras != 1)
		{
			std::cout << "Error : 3ds file " << l_file << 
				" contains " << l_file->ncameras << " camera(s)" << std::endl;
			exit(2);
		}
		camera = l_file->cameras[0];

		// Lib3ds invert Y and Z axis for some reasons
		// Need to fix the value providen
		float position_fixed[3];
		position_fixed[0] = camera->position[0] /100.0f;
		position_fixed[2] = camera->position[1] /100.0f;
		position_fixed[1] = camera->position[2] /100.0f;
		float target_fixed[3];
		target_fixed[0] = camera->target[0] /100.0f;
		target_fixed[2] = camera->target[1] /100.0f;
		target_fixed[1] = camera->target[2] /100.0f;

		return lib3ds_matrix_camera_fixed(position_fixed, target_fixed, camera->roll);
	}

	// Import camera tranform from 3ds file (Renderman)
	std::vector<float> getRendermanTransformFrom3dsFile(const std::string& file)
	{
		// Load camera from 3ds file
		Lib3dsCamera * camera;
		Lib3dsFile * l_file =  tool_filesystem::open3dsFile(file);
		// Check it contains only 1 camera
		if(l_file->ncameras != 1)
		{
			std::cout << "Error : 3ds file " << l_file << 
				" contains " << l_file->ncameras << " camera(s)" << std::endl;
			exit(2);
		}
		camera = l_file->cameras[0];

		// Lib3ds invert Y and Z axis for some reasons
		// Need to fix the value providen
		float position_fixed[3];
		position_fixed[0] = camera->position[0] /100.0f;
		position_fixed[2] = camera->position[1] /100.0f;
		position_fixed[1] = camera->position[2] /100.0f;
		float target_fixed[3];
		target_fixed[0] = camera->target[0] /100.0f;
		target_fixed[2] = camera->target[1] /100.0f;
		target_fixed[1] = camera->target[2] /100.0f;

		return lib3ds_matrix_camera_renderman(position_fixed, target_fixed, camera->roll);
	}
// namespace
}

namespace tool_filesystem
{
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
	
	// Import 3ds file with lib3ds and check it
	Lib3dsFile * open3dsFile(const std::string& file)
	{
		Lib3dsFile * l_file =  lib3ds_file_open(file.c_str());
		// Verify the file
		if(!l_file)                                                                 	
		{
			std::cout << "Error : unable to load the given file" 
							<< file.c_str() << std::endl;
			exit(2);
		}
		return l_file;
	}
// namespace
}

namespace tool_renderman
{
	// Construct a RIB file name 
	std::string _getRIBFile(const std::string name, const unsigned int frame, const std::string label)
	{
		std::string returnValue = "/home/robin/Bureau/Render/rib/" + name + "/";
		if(label != "")
			returnValue += label + "/";
		// Create directory if does not exist
		std::string commandDirectory = "mkdir -p " + returnValue;
		system(commandDirectory.c_str());
		// Add frame number pad 4
		std::ostringstream oss;
		oss << std::setfill('0') << std::setw(4) << frame;
		returnValue += "rib." + oss.str() + ".rib";
		return returnValue;
	}

	// Construct a TiFF file name
	std::string _getTIFFFile(const std::string name, const unsigned int frame, const std::string label)
	{
		std::string returnValue = "/home/robin/Bureau/Render/images/" + name + "/";
		if(label != "")
			returnValue += label + "/";
		// Create directory if does not exist
		std::string commandDirectory = "mkdir -p " + returnValue;
		system(commandDirectory.c_str());
		// Add frame number pad 4
		std::ostringstream oss;
		oss << std::setfill('0') << std::setw(4) << frame;
		returnValue += "img." + oss.str() + ".tiff";
		return returnValue;
	}

	// Convert an OpenGL camera to Renderman camera
	void _convertMatrixToRtMatrix(const std::vector<float>& matrix, RtMatrix converted)
	{
		for(unsigned int k=0; k<4; ++k)
		{
			for(unsigned int l=0; l<4; ++l)
				converted[k][l] = 0;
		}
		unsigned int i = 0;
		unsigned int j = 0;
		for(unsigned int indice = 0; indice<16; ++indice)
		{
			if(j == 4)
			{
				++i;
				j=0;
			}
			converted[i][j] = matrix.at(indice);
			++j;
		}
	}

	// Generate RIB file header
	void generateRIBHeader( \
		const std::string name, \
		const unsigned int frame, \
		const std::vector<float> camera, \
		const std::string label \
	)
	{
		std::string ribFile = _getRIBFile(name, frame, label);
		std::string tiffFile = _getTIFFFile(name, frame, label);
		// Create Rib file header
		RiBegin(ribFile.c_str());
		RiDisplay(tiffFile.c_str(), RI_FILE, RI_RGB, RI_NULL);
		RiFormat(RtInt(1280), RtInt(720), 1.0f);
		RtFloat fov(45.0f);
		RiProjection("perspective", RI_FOV, &fov, RI_NULL);
		// Convert Camera from OpenGL to Renderman
		RtMatrix renderCamera;
		_convertMatrixToRtMatrix(camera, renderCamera);
		RiTransform(renderCamera);
		RiPixelSamples(RtFloat(4.0f), RtFloat(4.0f));
	}

	// Generate RIB file footer
	void generateRIBFileFooter()
	{
		RiWorldEnd();
		RiEnd();
	}

	// Render one boid to renderman
	void renderOneBoid(const Boid& b)
	{
		const float x = b.position(0);
		const float y = b.position(1);
		const float z = b.position(2);
		const float size = 0.01f + (b.size() * 0.0025f);
		const float  intensity = 0.5f + b.intensity() * 0.5f;

		RiAttributeBegin();
		RiTranslate(x, y, z);
		RtFloat kr = intensity;
		RiSurface("star_core","Kd",&kr,RI_NULL);
		RiSphere(size, 0.0f, 0.5f, 360.0f, RI_NULL);
		RiAttributeEnd();
	}

	// Create the renderman attribute for a matte pass
	void shadeMeshMattePass()
	{
		RiAttributeBegin();
		RtFloat displacementbound = 3.0f;
		const char* coord = "world";
		RiAttribute("displacementbound", "sphere", &displacementbound, \
		"coordinatesystem", &(coord), RI_NULL);
		RiDisplacement("robin_mesh", RI_NULL);
		RiSurface("robin_mesh_matte", RI_NULL);
	}

	// Create the renderman attribute for a reflect pass
	void shadeMeshReflectPass()
	{
		RiAttributeBegin();
		RtFloat displacementbound = 3.0f;
		const char* coord = "world";
		RiAttribute("displacementbound", "sphere", &displacementbound, \
		"coordinatesystem", &(coord), RI_NULL);
		RiDisplacement("robin_mesh", RI_NULL);
		RiSurface("robin_mesh_color", RI_NULL);
	}

	// Create the renderman attribute for a skin pass
	void shadeMeshSkinPass()
	{
		RiAttributeBegin();
		RtFloat displacementbound = 3.0f;
		const char* coord = "world";
		RiAttribute("displacementbound", "sphere", &displacementbound, \
		"coordinatesystem", &(coord), RI_NULL);
		RiDisplacement("robin_mesh", RI_NULL);
		RiSurface("robin_mesh_skin", RI_NULL);
	}

	// Render one mesh to renderman
	void renderMesh(const std::vector<float>& mesh)
	{
		// Create a polygone from the mesh
		// Create the pointPerFace array (always  3 for each face)
		const int totalFaces = mesh.size()/9;
		RtInt pointPerFace[totalFaces];
		for(int i = 0; i<totalFaces; ++i)
			pointPerFace[i] = 3;
		// Usual parameters
		RtToken boundary[] = {"interpolateboundary"};
		RtInt nbIndiceTag[] = {0,0};
		// Indices array
		RtInt indices[mesh.size()];
		for(unsigned int i=0; i< mesh.size()/3; ++i)
			indices[i] = (RtInt) i;
		// Vertex array
		RtFloat vertices[mesh.size()];
		for(unsigned int idx = 0; idx<mesh.size(); ++idx)
			vertices[idx] = RtFloat(mesh[idx]);

		RiSubdivisionMesh( \
			"catmull-clark", \
			(RtInt)totalFaces, \
			pointPerFace, \
			indices, \
			1, \
			boundary, \
			nbIndiceTag, \
			NULL, NULL, \
			"P", vertices, \
			RI_NULL \
		);
		RiAttributeEnd();
	}
}

namespace tool_debug
{
	//Print a matrix on shell
	void printMatrix(const std::vector<float> matrix)
	{
		std::cout << "" << std::endl;
		for(unsigned int i=0; i<16; i+=4)
		{
			std::cout << matrix[i+0]<<" " << matrix[i+1]<<" " << matrix[i+2]<<" " << matrix[i+3]<< std::endl;
		}
		std::cout << "" << std::endl;
	}
// namespace
}
