#ifndef __TOOLS_HPP__
#define __TOOLS_HPP__

#include "Application.hpp"
#include "Camera.hpp"

#include <string>
#include <vector>

namespace tool_geometry
{
	// To normalize a vector
	void normalize (std::vector<float>& v);
	// To get the vector product
	const std::vector<float> vectorProduct (const std::vector<float>& a,
						const std::vector<float>& b);
	// Does the multiplication A=A*B : all the matrices are described column-major
	void multMatrixBtoMatrixA(std::vector<float>& A, const std::vector<float>& B);
	// Sets the provided matrix to identity
	void setToIdentity(std::vector<float>& matrix);
	// Sets the provided matrix to a translate matrix on vector t
	void setToTranslate(std::vector<float>& matrix, const std::vector<float>& t);
	// Sets the provided matrix to a scale matrix by coeficients in s
	void setToScale(std::vector<float>& matrix, const std::vector<float>& s);
	// Sets the provided matrix to a rotate matrix of angle "angle", around axis "axis"
	void setToRotate(std::vector<float>& matrix, const float angle,
						     const std::vector<float>& axis);
	// Builds a perspective projection matrix and stores it in mat
	// l = left,
	// r = right,
	// b = bottom,
	// t = top,
	// n = near,
	// f = far in the frustum
	const std::vector<float> setPerspective(const float l, const float r, const float b,
						const float t, const float n, const float f);
}

namespace tool_camera
{
	// Draw the color triangle to test display
	void drawTestSample();
	// Draw a simple color scene to test display
	void drawTestScene();
	// Update the camera values according to keyboard, mouse
	void manageFps(const Application& app, Camera& camera);
}

namespace tool_filesystem
{
	// Open 3ds file sequence
	//void open3dsFiles();
	
	// Brute open 3ds file sequence since the boost
	// file system library does not appear to work
	// on my station (linking issues)
	std::vector<std::string> brute_open3dsFiles(const std::string& path, const int start_seq, const int end_seq);
}

namespace tool_debug
{
	//Print a matrix on shell
	void printMatrix(const std::vector<float> matrix);
}

#endif //  __TOOLS_HPP__
