#ifdef __APPLE__
	#include <GLUT/glut.h>
#elif defined _WIN32 || defined _WIN64
	#include "Dependencies/freeglut/freeglut.h"
#endif

#include <iostream>
#include <cmath>
#include "Dependencies/glm/glm.hpp"
#include "Dependencies/glm/gtc/matrix_transform.hpp"
#include "cameraPosition.hpp"
#include "constant.hpp"

using namespace std;

extern float specularCoefficient, diffuseCoefficient;
extern float cameraPosAngle, zoomConstant;
extern int fogFlag;
extern glm::vec3 fogColor;
extern glm::vec3 cameraPosition;
extern float perspectiveAngle;
extern int viewFlag;
extern float maxSize;

bool passiveMouseMode = false;
extern float orbitSize, rotationSpeedConstant;

void keyboardClick(unsigned char key, int x, int y){
  if (key == ' ')
	passiveMouseMode = !passiveMouseMode;

	// Zoom in/out Effect
	else if (key == 'o'){
		zoomConstant += 1.0f;
		zoomConstant = glm::clamp(zoomConstant, -50.0f, 59.f);
		cameraPosition = glm::vec3(60.0f - zoomConstant, 20.0f, 60.0f - zoomConstant);
	}
	else if (key == 'l'){
		zoomConstant -= 1.0f;
		zoomConstant = glm::clamp(zoomConstant, -50.0f, 59.f);
		cameraPosition = glm::vec3(60.0f - zoomConstant, 20.0f, 60.0f - zoomConstant);
	}
	//

	else if (key == 'a'){
		// Viewpoint at +Y axis
		perspectiveAngle = 90.0f;
		viewFlag = 2;
	}
	else if (key == 's'){
		// Viewpoint at +X axis
		perspectiveAngle = 45.0f;
		viewFlag = 1;
	}
	else if (key == 'd'){
		// Viewpoint at +X axis
		perspectiveAngle = 45.0f;
		viewFlag = -1;
	}
	else if (key == 'f'){
		// Viewpoint attached to airplane
		perspectiveAngle = 75.0f;
		viewFlag = 99;
	}
	else if (key == 'g'){
		// Back to original viewpoint
		perspectiveAngle = 45.0f;
		viewFlag = 0;
	}

	// Fog Effect
	else if (key == '1')
		fogFlag = fogFlag ? 0 : 1;
	else if (key == '2')
		fogColor = fogColor == glm::vec3(0.5, 0.5, 0.5) ? glm::vec3(0.5, 0.5, 0.6) : glm::vec3(0.5, 0.5, 0.5);
	//

  else if (key == 'q')
    exit(0);
}

void arrowKey(int key, int x, int y){
	if (key == GLUT_KEY_UP){
	  rotationSpeedConstant += 1.0f;
	  rotationSpeedConstant = glm::clamp(rotationSpeedConstant, 0.0f, 100.0f);
	}
	else if (key == GLUT_KEY_DOWN){
	  rotationSpeedConstant -= 1.0f;
	  rotationSpeedConstant = glm::clamp(rotationSpeedConstant, 0.0f, 100.0f);
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
	printf("mouseWheelFunc: %d\n", state);
	// printf("mouseWheelFuncx: %d  y: %d\n", x, y);
	if (button == GLUT_RIGHT_BUTTON && state == GLUT_DOWN){
		zoomConstant -= 0.5f;
		zoomConstant = glm::clamp(zoomConstant, -50.0f, 59.f);
	}
	if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN){
		zoomConstant += 0.5f;
		zoomConstant = glm::clamp(zoomConstant, -50.0f, 59.f);
	}
	cameraPosition = glm::vec3(60.0f - zoomConstant, 20.0f, 60.0f - zoomConstant);
}
