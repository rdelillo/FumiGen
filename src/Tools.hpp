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
	// Update the camera values according to keyboard, mouse
	void manageFps(const Application& app, Camera& camera);
}

namespace tool_debug
{
	//Print a matrix on shell
	void printMatrix(const std::vector<float> matrix);
}


//@TODO: remove undefined tools functions ?
/*
std::string * loadFile(const std::string & fileName);
void printShaderLog(GLint shaderId);
GLuint loadProgram(const std::vector<std::string> & files);
void setMatricesInShader(GLuint shaderId, GLfloat * model, GLfloat * view, GLfloat * projection);
void setLightInShader(GLuint shaderID, GLfloat * position, GLfloat power);
void setMaterialInShader(GLuint shaderID, GLfloat * ambient, GLfloat * diffuse, GLfloat * specular, GLfloat ka, GLfloat kd, GLfloat ks, GLfloat shininess);
void changeMaterialColorInShader(GLuint shaderID, GLfloat * color);
void setFilledDataInShader(GLuint shaderID, GLboolean positions, GLboolean normals, GLboolean uvs, GLboolean colors);
unsigned char * loadPPM(const std::string & filename, GLuint * width, GLuint * height);
*/

#endif //  __TOOLS_HPP__
