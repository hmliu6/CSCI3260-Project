#ifdef __APPLE__
	#include <GLUT/glut.h>
#elif defined _WIN32 || defined _WIN64
	#include "Dependencies/freeglut/freeglut.h"
#endif

#include <iostream>
#include <cmath>
#include "Dependencies/glm/glm.hpp"
#include "cameraPosition.hpp"
#include "constant.hpp"

using namespace std;

extern float specularCoefficient, diffuseCoefficient;
extern float cameraPosAngle, zoomConstant;
extern GLuint fogFlag;

bool passiveMouseMode = false;
extern float orbitSize, rotationSpeedConstant;

void keyboardClick(unsigned char key, int x, int y){
  if (key == ' ')
	passiveMouseMode = !passiveMouseMode;
  else if (key == 'a'){
  }
	else if (key == 'o')
		zoomConstant += 1.0f;
	else if (key == 'l')
		zoomConstant -= 1.0f;
	else if (key == '1')
		fogFlag = fogFlag ? 0 : 1;
  else if (key == 'q')
    exit(0);
}

void arrowKey(int key, int x, int y){
	if (key == GLUT_KEY_UP){
	  rotationSpeedConstant += 0.01f;
	}
	else if (key == GLUT_KEY_DOWN){
	  rotationSpeedConstant -= 0.01f;
	  rotationSpeedConstant = glm::clamp(rotationSpeedConstant, 0.0f, 1.0f);
	}
	else if (key == GLUT_KEY_LEFT){
	  // cameraPosAngle -= 0.1f;
	  orbitSize += 0.1f;
	  orbitSize = glm::clamp(orbitSize, -12.0f, -7.0f);
	  // Smaller orbit size
	}
	else if (key == GLUT_KEY_RIGHT){
	  // cameraPosAngle += 0.1f;
	  orbitSize -= 0.1f;
	  orbitSize = glm::clamp(orbitSize, -12.0f, -7.0f);
	  // Larger orbit size
	}
}

void mouseCoordinate(int x, int y){

}

void mouseWheelFunc(int button, int state, int x, int y){

}
