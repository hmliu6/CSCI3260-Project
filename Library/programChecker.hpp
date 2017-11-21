#ifndef PROGRAMCHECKER_H
#define PROGRAMCHECKER_H

#include <string>
using std::string;

bool checkStatus(
  GLuint objectID,
	PFNGLGETSHADERIVPROC objectPropertyGetterFunc,
	PFNGLGETSHADERINFOLOGPROC getInfoLogFunc,
	GLenum statusType);

bool checkShaderStatus(GLuint shaderID);

bool checkProgramStatus(GLuint programID);

string readShaderCode(const char* fileName);

GLuint installShaders();

#endif
