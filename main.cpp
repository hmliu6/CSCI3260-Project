#ifdef __APPLE__
#include <GL/glew.h>
#include <GLUT/glut.h>
#elif defined _WIN32 || defined _WIN64
#include "Library/Dependencies/glew/glew.h"
#include "Library/Dependencies/freeglut/freeglut.h"
#endif 
#include <stdlib.h>
#include <math.h>
#include "Library/Dependencies/glm/glm.hpp"
#include "Library/Dependencies/glm/gtc/matrix_transform.hpp"
#include "Library/Dependencies/glm/gtx/rotate_vector.hpp"
#include "Library/Dependencies/glm/gtx/vector_angle.hpp"

#include <iostream>
#include <vector>
#include <cmath>

#include "Library/constant.h"
#include "Library/programChecker.hpp"
#include "Library/gesture.hpp"
#include "Library/objLoader.hpp"
#include "Library/textureLoader.hpp"
#include "Library/compute.hpp"
#include "Library/cameraPosition.h"

using namespace std;


// ProgramID passed from installShaders()
GLint programID;
float specularCoefficient = 0.1f, diffuseCoefficient = 50.0f;
float earth_innRot_Degree = 0.0f;

class Object {
public:
	// Class constructor
	Object() {
		//objIndex = globalCounter++;
		glGenBuffers(1, &VAObuffer);
		glGenBuffers(1, &vertexVBO);
		glGenBuffers(1, &uvVBO);
		glGenBuffers(1, &normalVBO);
		modelScalingMatrix = glm::mat4(1.0f);
		modelTransformMatrix = glm::mat4(1.0f);
		modelRotationMatrix = glm::mat4(1.0f);
		normalMapFlag = 0;
	}

	// Pass all object data to buffer
	void loadObjToBuffer(char * objectPath) {
		bool res = loadOBJ(objectPath, vertices, uvs, normals);

		glGenVertexArrays(1, &VAObuffer);
		glBindVertexArray(VAObuffer);

		// Bind Vertices Data to Buffer
		glGenBuffers(1, &vertexVBO);
		glBindBuffer(GL_ARRAY_BUFFER, vertexVBO);
		glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(glm::vec3), &vertices[0], GL_STATIC_DRAW);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

		// Bind UV Data to Buffer
		glGenBuffers(1, &uvVBO);
		glBindBuffer(GL_ARRAY_BUFFER, uvVBO);
		glBufferData(GL_ARRAY_BUFFER, uvs.size() * sizeof(glm::vec2), &uvs[0], GL_STATIC_DRAW);
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, (void*)0);

		// Bind Normal Data to Buffer
		glGenBuffers(1, &normalVBO);
		glBindBuffer(GL_ARRAY_BUFFER, normalVBO);
		glBufferData(GL_ARRAY_BUFFER, normals.size() * sizeof(glm::vec2), &normals[0], GL_STATIC_DRAW);
		glEnableVertexAttribArray(2);
		glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

		drawSize = vertices.size();
	}

	// Pass all texture data to buffer
	void loadTextureToBuffer(char * texturePath) {
		objectTexture = loadBMP_custom(texturePath);
		TextureID = glGetUniformLocation(programID, "colorTexture");
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, objectTexture);
		glUniform1i(TextureID, 0);
	}

	void loadNormalTextureToBuffer(char * texturePath) {
		objectNormalTexture = loadBMP_custom(texturePath);
		NormalTextureID = glGetUniformLocation(programID, "normalMap");
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, objectNormalTexture);
		glUniform1i(NormalTextureID, 1);
		normalMapFlag = 1;
	}

	// Scaling
	void setScale(glm::vec3 scale) {
		modelScalingMatrix = glm::scale(glm::mat4(), scale);
	}

	// Transformation
	void setTransform(glm::vec3 transform) {
		modelTransformMatrix = glm::translate(glm::mat4(), transform);
	}

	// Rotation
	void setRotate(glm::vec3 axis, float thetaDegree) {
		modelRotationMatrix = glm::rotate(glm::mat4(), glm::radians(thetaDegree), axis);
	}

	// Rotation with self object space
	void setSelfRotate(glm::vec3 axis, float thetaDegree) {
		glm::mat4 selfRotateMatrix = glm::rotate(glm::mat4(), glm::radians(thetaDegree), axis);
		modelScalingMatrix = selfRotateMatrix * modelScalingMatrix;
	}

	// Passing all matrices to Shader
	void sendMatrix() {
		GLint ScalingMatrixID = glGetUniformLocation(programID, "ScalingMatrix");
		GLint TransformMatrixID = glGetUniformLocation(programID, "TransformMatrix");
		GLint RotateMatrixID = glGetUniformLocation(programID, "RotationMatrix");

		glUniformMatrix4fv(ScalingMatrixID, 1, GL_FALSE, &modelScalingMatrix[0][0]);
		glUniformMatrix4fv(TransformMatrixID, 1, GL_FALSE, &modelTransformMatrix[0][0]);
		glUniformMatrix4fv(RotateMatrixID, 1, GL_FALSE, &modelRotationMatrix[0][0]);

		normalMapFlagID = glGetUniformLocation(programID, "normalMapFlag");
		glUniform1i(normalMapFlagID, normalMapFlag);
		//printf("%i\n",normalMapFlag);

	}

	// Bind object buffer and draw on screen
	void renderObject() {
		glBindVertexArray(VAObuffer);
		glBindTexture(GL_TEXTURE_2D, objectTexture);
		glDrawArrays(GL_TRIANGLES, 0, drawSize);
	}

private:
	// Put all variables declaration here
	//static short globalCounter;
	//GLushort objIndex;

	GLuint VAObuffer, vertexVBO, uvVBO, normalVBO;
	GLuint objectTexture, TextureID, objectNormalTexture, NormalTextureID;
	GLuint normalMapFlagID;
	GLushort normalMapFlag;

	GLsizei drawSize;

	std::vector <glm::vec3> vertices, normals;
	std::vector <glm::vec2> uvs;

	glm::mat4 modelScalingMatrix, modelTransformMatrix, modelRotationMatrix;
};

// Initialize class object pointer globally
// C++ restricted to global class object declaration
Object *jeep = nullptr;
Object *earth = nullptr;
// MUST create new object in main and point to variables

// Camera Matrix
CameraPosition cameraPosition = {
	20.0f, 15.0f, 20.0f,
	0.0f, 0.0f, 0.0f,
};

void eyeViewMatrix() {
	glm::mat4 Projection = glm::perspective(glm::radians(45.0f), 1.0f, 2.0f, 40.0f);
	glm::mat4 View = glm::lookAt(
		glm::vec3(cameraPosition.x, cameraPosition.y, cameraPosition.z), // Camera is at (x,y,z), in World Space
		glm::vec3(0.0f, 2.0f, 0.0f), // and looks at point
		glm::vec3(0.0f, 1.0f, 0.0f)  // Head is up (set to 0,-1,0 to look upside-down)
	);
	glm::mat4 Model = glm::mat4(1.0f);
	glm::mat3 VM = glm::mat3(View * Model);
	glm::mat4 pvm = Projection * View * Model;

	GLuint MatrixID = glGetUniformLocation(programID, "PVM");
	glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &pvm[0][0]);

	GLuint ViewID = glGetUniformLocation(programID, "viewMatrix");
	GLuint ModelID = glGetUniformLocation(programID, "modelMatrix");
	GLuint VMID = glGetUniformLocation(programID, "viewmodelMatrix");

	glUniformMatrix4fv(ViewID, 1, GL_FALSE, &View[0][0]);
	glUniformMatrix4fv(ModelID, 1, GL_FALSE, &Model[0][0]);
	glUniformMatrix4fv(VMID, 1, GL_FALSE, &VM[0][0]);
}

void lightControl() {
	glm::vec3 lightPosition = glm::vec3(2.5f, 12.5f, 5.5f);

	GLuint LightID = glGetUniformLocation(programID, "lightPosition");
	glUniform3f(LightID, lightPosition.x, lightPosition.y, lightPosition.z);

	GLuint diffuseID = glGetUniformLocation(programID, "diffuseCoefficient");
	glUniform1f(diffuseID, diffuseCoefficient);

	GLuint specularID = glGetUniformLocation(programID, "specularCoefficient");
	glUniform1f(specularID, specularCoefficient);
}

// Load objects to buffer
void objDataToOpenGL() {
	// Standard steps to pass one object and its texture

	// Load jeep
	jeep->loadObjToBuffer("obj/jeep.obj");
	jeep->loadTextureToBuffer("texture/jeep_texture.bmp");
	jeep->setScale(glm::vec3(0.5f, 0.5f, 0.5f));
	jeep->setTransform(glm::vec3(2.0f, 0.3f, -1.0f));

	// Load earth
	earth->loadObjToBuffer("obj/planet.obj");
	earth->loadTextureToBuffer("texture/earth.bmp");
	earth->loadNormalTextureToBuffer("normal_map/earth_normal.bmp");
	earth->setScale(glm::vec3(2.5f, 2.5f, 2.5f));
	earth->setTransform(glm::vec3(2.0f, 5.3f, 5.0f));
	//

}

// Initialize OpenGL
void initOpenGL() {
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);
	glEnable(GL_CULL_FACE);
	glEnable(GL_POLYGON_SMOOTH);
	// Enable depth test
	glClearDepth(1.0f);
	glDepthFunc(GL_LESS);
	glEnable(GL_DEPTH_TEST);
	// glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	programID = installShaders();
	objDataToOpenGL();
}

// Keep looping to draw on screen
void drawScreen() {
	glClearColor(0.5f, 0.5f, 0.5f, 1.0f); //specify the background color
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	eyeViewMatrix();

	// Standard step to draw one object
	jeep->sendMatrix();
	jeep->renderObject();

	earth->setSelfRotate(glm::vec3(0, 1, 0), 0.01);
	earth->sendMatrix();
	earth->renderObject();
	//

	glutSwapBuffers();
	glFlush();
	glutPostRedisplay();
}

// Timer function
void timerFunction(int id)
{
	earth_innRot_Degree += 0.0003f;
	//printf("%f\n",earth_innRot_Degree);

	glutPostRedisplay();
	glutTimerFunc(700.0f / 60.0f, timerFunction, 1);
}

int main(int argc, char *argv[]) {
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGBA);
	glutInitDisplayMode(GLUT_RGB | GLUT_SINGLE | GLUT_DEPTH | GLUT_MULTISAMPLE);
	glutInitWindowSize(WINDOW_WIDTH, WINDOW_HEIGHT);
	glutCreateWindow("CSCI3260 Project");
	glewInit();

	// Create object and point to global variables
	jeep = new Object;
	earth = new Object;

	initOpenGL();
	glutDisplayFunc(drawScreen);

	// Mouse and keyboard functions are in Library/gesture.cpp
	glutMouseFunc(Mouse_Wheel_Func);
	glutKeyboardFunc(keyboardClick);
	glutSpecialFunc(arrowKey);
	glutPassiveMotionFunc(mouseCoordinate);

	glutTimerFunc(700.0f / 60.0f, timerFunction, 1);

	glutMainLoop();
	return 0;
}
