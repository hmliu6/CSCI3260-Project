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

#include "Library/programChecker.hpp"
#include "Library/gesture.hpp"
#include "Library/objLoader.hpp"
#include "Library/textureLoader.hpp"
#include "Library/compute.hpp"
#include "Library/cameraPosition.hpp"
#include "Library/constant.hpp"

using namespace std;

#define PI 3.14159265

// ProgramID passed from installShaders()
GLint programID, skyboxProgramID, lightSourceProgramID;
float specularCoefficient = 0.5f, diffuseCoefficient = 185.0f;
float cameraPosAngle = 71.0f;
float orbitalTheta = 0.0f, saturnAlpha = 0.0f, moonTheta = 0.0f, airplaneTheta = 0.0f;
// Parameter for choosing Shader part
glm::mat4 Projection, View;
CameraPosition cameraPosition = {
	60.0f, 20.0f, 60.0f,
  0.0f, 0.0f, 0.0f };

glm::vec3 lightPosition = glm::vec3(0.0f, 0.0f, 0.0f);

float earth_innRot_Degree = 0.0f;
float cameraRotation = 0.0f, verticalRotation = 0.0f;

class Object {
public:
	// Class constructor
	Object() {
		glGenBuffers(1, &VAObuffer);
		glGenBuffers(1, &vertexVBO);
		glGenBuffers(1, &uvVBO);
		glGenBuffers(1, &normalVBO);
		modelMatrix = glm::mat4(1.0f);
		modelScalingMatrix = glm::mat4(1.0f);
		modelTransformMatrix = glm::mat4(1.0f);
		modelRotationMatrix = glm::mat4(1.0f);
		tempScalingMatrix = glm::mat4(1.0f);
		tempTransformMatrix = glm::mat4(1.0f);
		normalMapFlag = 0;
		secondTextureFlag = 0;
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
	void loadTextureToBuffer(char * texturePath, GLint shaderProgramID) {
		objectTexture = loadBMPtoTexture(texturePath);
		TextureID = glGetUniformLocation(shaderProgramID, "myTextureSampler_1");
		printf("Texture1 ID: %i \n", TextureID);
	}


	// Pass normal texture map to buffer
	void loadNormalTextureToBuffer(char * texturePath, GLint shaderProgramID) {
		objectNormalTexture = loadBMP_custom(texturePath);
		NormalTextureID = glGetUniformLocation(shaderProgramID, "normalMap");
		printf("NormalTexture ID: %i \n", NormalTextureID);
		normalMapFlag = 1;
	}

	// Pass all texture data to buffer
	void loadSecondTextureToBuffer(char * texturePath, GLint shaderProgramID) {
		objectTexture2 = loadBMPtoTexture(texturePath);
		TextureID2 = glGetUniformLocation(shaderProgramID, "myTextureSampler_2");
		printf("Texture2 ID: %i \n", TextureID2);
		secondTextureFlag = 1;
	}


	glm::vec3 getGlobalOrigin() {
		glm::vec3 globalOrigin = glm::vec3(0, 0, 0);
		glm::vec4 tempGlobalOrigin = modelRotationMatrix * modelTransformMatrix * modelScalingMatrix * glm::vec4(globalOrigin, 1.0f);
		globalOrigin = glm::vec3(tempGlobalOrigin.x, tempGlobalOrigin.y, tempGlobalOrigin.z);
		return globalOrigin;
	}

	// Model Matrix
	void setModelMatrix(glm::mat4 _modelMatrix) {
		modelMatrix = _modelMatrix;
	}

	// Scaling
	void setScale(glm::vec3 scale) {
		tempScalingMatrix = glm::scale(glm::mat4(), scale);
		modelScalingMatrix = tempScalingMatrix;
	}

	void setOrigin(glm::vec3 origin) {
		modelScalingMatrix = glm::translate(modelMatrix, origin) * tempScalingMatrix;
	}

	// Rotation with self object space
	void setSelfRotate(glm::vec3 axis, float thetaDegree) {
		glm::mat4 selfRotateMatrix = glm::rotate(glm::mat4(), glm::radians(thetaDegree), axis);
		modelScalingMatrix = selfRotateMatrix * modelScalingMatrix;
	}

	// Transformation
	void setTransform(glm::vec3 transform) {
		modelTransformMatrix = glm::translate(modelMatrix, transform);
	}

	// Rotation
	void setRotate(glm::vec3 axis, float thetaDegree) {
		modelRotationMatrix = glm::rotate(glm::mat4(), glm::radians(thetaDegree), axis);
	}

	// Passing all matrices to Shader
	void sendMatrix(GLint shaderProgramID) {
		GLint ScalingMatrixID = glGetUniformLocation(shaderProgramID, "ScalingMatrix");
		GLint TransformMatrixID = glGetUniformLocation(shaderProgramID, "TransformMatrix");
		GLint RotateMatrixID = glGetUniformLocation(shaderProgramID, "RotationMatrix");

		glUniformMatrix4fv(ScalingMatrixID, 1, GL_FALSE, &modelScalingMatrix[0][0]);
		glUniformMatrix4fv(TransformMatrixID, 1, GL_FALSE, &modelTransformMatrix[0][0]);
		glUniformMatrix4fv(RotateMatrixID, 1, GL_FALSE, &modelRotationMatrix[0][0]);

		GLuint normalMapFlagID = glGetUniformLocation(shaderProgramID, "normalMapFlag");
		GLuint secondTextureFlagID = glGetUniformLocation(shaderProgramID, "secondTextureFlag");
		glUniform1i(secondTextureFlagID, secondTextureFlag);
		glUniform1i(normalMapFlagID, normalMapFlag);
	}

	// Bind object buffer and draw on screen
	void renderObject() {
		glBindVertexArray(VAObuffer);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, objectTexture);
		glUniform1i(TextureID, 0);
		if (normalMapFlag == 1) {
			glActiveTexture(GL_TEXTURE1);
			glBindTexture(GL_TEXTURE_2D, objectNormalTexture);
			glUniform1i(NormalTextureID, 1);
		}
		if (secondTextureFlag == 1) {
			glActiveTexture(GL_TEXTURE2);
			glBindTexture(GL_TEXTURE_2D, objectTexture2);
			glUniform1i(TextureID2, 2);
		}
		glDrawArrays(GL_TRIANGLES, 0, drawSize);
	}

protected:
	// Put all variables declaration here
	GLuint VAObuffer, vertexVBO, uvVBO, normalVBO;
	GLuint objectTexture, TextureID, objectTexture2, TextureID2, objectNormalTexture, NormalTextureID;
	GLushort normalMapFlag, secondTextureFlag;

	GLsizei drawSize;

	std::vector <glm::vec3> vertices, normals;
	std::vector <glm::vec2> uvs;

	glm::mat4 modelMatrix;
	glm::mat4 modelScalingMatrix, modelTransformMatrix, modelRotationMatrix, tempScalingMatrix, tempTransformMatrix;
};

class Skybox : public Object {
public:
	// Class constructor
	Skybox() {
		glGenBuffers(1, &VAObuffer);
		glGenBuffers(1, &vertexVBO);
		glGenTextures(1, &skyboxTexture);
		modelScalingMatrix = glm::mat4(1.0f);
		modelTransformMatrix = glm::mat4(1.0f);
		modelRotationMatrix = glm::mat4(1.0f);
	}

	void loadVerticesToBuffer(float size) {
		GLfloat points[] = {
			// positions
			-1.0f,  1.0f, -1.0f,
			-1.0f, -1.0f, -1.0f,
			1.0f, -1.0f, -1.0f,
			1.0f, -1.0f, -1.0f,
			1.0f,  1.0f, -1.0f,
			-1.0f,  1.0f, -1.0f,

			-1.0f, -1.0f,  1.0f,
			-1.0f, -1.0f, -1.0f,
			-1.0f,  1.0f, -1.0f,
			-1.0f,  1.0f, -1.0f,
			-1.0f,  1.0f,  1.0f,
			-1.0f, -1.0f,  1.0f,

			1.0f, -1.0f, -1.0f,
			1.0f, -1.0f,  1.0f,
			1.0f,  1.0f,  1.0f,
			1.0f,  1.0f,  1.0f,
			1.0f,  1.0f, -1.0f,
			1.0f, -1.0f, -1.0f,

			-1.0f, -1.0f,  1.0f,
			-1.0f,  1.0f,  1.0f,
			1.0f,  1.0f,  1.0f,
			1.0f,  1.0f,  1.0f,
			1.0f, -1.0f,  1.0f,
			-1.0f, -1.0f,  1.0f,

			-1.0f,  1.0f, -1.0f,
			1.0f,  1.0f, -1.0f,
			1.0f,  1.0f,  1.0f,
			1.0f,  1.0f,  1.0f,
			-1.0f,  1.0f,  1.0f,
			-1.0f,  1.0f, -1.0f,

			-1.0f, -1.0f, -1.0f,
			-1.0f, -1.0f,  1.0f,
			1.0f, -1.0f, -1.0f,
			1.0f, -1.0f, -1.0f,
			-1.0f, -1.0f,  1.0f,
			1.0f, -1.0f,  1.0f };

		for (int i = 0; i < sizeof(points) / sizeof(float); i++)
			points[i] *= size;

		glGenVertexArrays(1, &VAObuffer);
		glBindVertexArray(VAObuffer);

		// Bind Vertices Data to Buffer
		glGenBuffers(1, &vertexVBO);
		glBindBuffer(GL_ARRAY_BUFFER, vertexVBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(points), &points, GL_STATIC_DRAW);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), 0);

	}

	void create_cube_map(const char* frontPath, const char* backPath, const char* topPath, const char* bottomPath, const char* leftPath, const char* rightPath) {
		vector<const GLchar*> skyfaces;
		skyfaces.push_back(rightPath);  skyfaces.push_back(leftPath);
		skyfaces.push_back(bottomPath); skyfaces.push_back(topPath);
		skyfaces.push_back(backPath);   skyfaces.push_back(frontPath);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_CUBE_MAP, skyboxTexture);
		int width, height;
		unsigned char *image;

		for (GLuint i = 0; i < skyfaces.size(); i++) {
			loadBMP_Data(skyfaces[i], image, width, height);
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
		}

		// format cube map texture
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
		glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
	}

	void renderSkybox() {
		glm::mat4 tempView = View;

		glUseProgram(skyboxProgramID);
		glDepthMask(GL_FALSE);
		glDisable(GL_CULL_FACE);

		View = glm::mat4(glm::mat3(View));

		sendMatrix(skyboxProgramID);
		glBindVertexArray(VAObuffer);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_CUBE_MAP, skyboxTexture);

		glDrawArrays(GL_TRIANGLES, 0, 36);

		glEnable(GL_CULL_FACE);
		glDepthMask(GL_TRUE);
		glUseProgram(programID);

		View = tempView;
	}

private:
	GLuint skyboxTexture;
	GLuint VAObuffer, vertexVBO;
	GLsizei drawSize;
	glm::mat4 modelScalingMatrix, modelTransformMatrix, modelRotationMatrix;
};

class Earth : public Object {
public:
	Earth() : Object() {}

	glm::vec3 getEarthCentre() {
		glm::vec4 origin = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
		glm::vec4 movedOrigin = modelRotationMatrix * modelTransformMatrix * modelScalingMatrix * origin;
		return glm::vec3(movedOrigin);
	}
};

class Airplane : public Object {
public:
	Airplane() : Object() {}

	// Scaling
	void setScale(glm::vec3 scale) {
		modelScalingMatrix = glm::scale(glm::mat4(), scale);
	}

	void setOrigin(glm::vec3 inputOrigin) {
		// modelScalingMatrix = glm::translate(modelMatrix, inputOrigin) * tempScalingMatrix;
		origin = inputOrigin;
	}

	// Rotation with self object space
	void setSelfRotate(glm::vec3 axis, float thetaDegree) {
		glm::mat4 selfRotateMatrix = glm::rotate(glm::mat4(), glm::radians(thetaDegree), axis);
		modelScalingMatrix = selfRotateMatrix * modelScalingMatrix;
	}

	// Transformation
	void setTransform(glm::vec3 transform) {
		modelTransformMatrix = glm::translate(modelMatrix, transform) * glm::translate(modelMatrix, origin);
	}

	// Rotation
	void setRotate(glm::vec3 axis, float thetaDegree) {
		modelRotationMatrix = glm::rotate(glm::mat4(), glm::radians(thetaDegree), axis);
	}

private:
	glm::vec3 origin;
};

class positionArray{
  public:
    positionArray(int size){
      position = (glm::vec3*) malloc(sizeof(glm::vec3) * size);
      for(int i=0; i<size; i++)
        position[i] = glm::vec3(0.0f, 0.0f, 0.0f);
      element = 0;
    }

    void pushElement(glm::vec3 inputVector){
      position[element] = inputVector;
      element += 1;
    }

    glm::vec3 popElement(){
      glm::vec3 tempVector = position[0];
      if(element == 1){
        element -= 1;
        position[element] = glm::vec3(0.0f, 0.0f, 0.0f);
      }
      else{
        for(int i=1; i<element; i++)
          position[i] = position[i-1];
        element -= 1;
        position[element] = glm::vec3(0.0f, 0.0f, 0.0f);
      }
      return tempVector;
    }

    int countElement(){
      return element;
    }

    glm::vec3 getElement(int index){
      return position[index];
    }

  private:
    glm::vec3 *position;
    int element;
};

// Initialize class object pointer globally
// C++ restricted to global class object declaration
Object *jeep = nullptr;
Airplane *airplane = nullptr;
Earth *earth = nullptr;
Object *sun = nullptr;
Object *saturn = nullptr;
Object *moon = nullptr;
Object *rock = nullptr;
glm::mat4* rockMatrices = new glm::mat4[NUMBER_OF_ROCK];
Skybox *background = nullptr;
positionArray *flightTrack = nullptr;
// MUST create new object in main and point to variables

// ring cloud location creation
void createRandomModel(glm::vec3 modelOrigin) {
	rockMatrices = new glm::mat4[NUMBER_OF_ROCK];
	srand(glutGet(GLUT_ELAPSED_TIME)); // initialize random seed
	GLfloat radius = 6.0f;
	GLfloat offset = 0.4f;
	GLfloat displacement;

	for (GLuint i = 0; i < NUMBER_OF_ROCK; i++) {
		glm::mat4 model;
		// 1. Translation: displace along circle with 'radius' in range [-offset, offset]
		GLfloat angle = (GLfloat)i / (GLfloat)NUMBER_OF_ROCK * 360.0f;
		GLfloat displacement = (rand() % (GLint)(2 * offset * 100)) / 100.0f - offset;
		GLfloat x = sin(angle) * radius + displacement;
		displacement = (rand() % (GLint)(2 * offset * 100)) / 50.0f - offset;
		GLfloat y = displacement * 0.4f; // y value has smaller displacement
		displacement = (rand() % (GLint)(2 * offset * 100)) / 100.0f - offset;
		GLfloat z = cos(angle) * radius + displacement;
		GLfloat scale = (rand() % 10) / 100.0f + 0.05;
		GLfloat rotAngle = (rand() % 360);

		model = glm::translate(model, glm::vec3(modelOrigin.x + x, modelOrigin.y + y, modelOrigin.z + z));
		model = glm::scale(model, glm::vec3(scale));
		model = glm::rotate(model, rotAngle, glm::vec3(0.2f, 0.4f, 0.8f));

		rockMatrices[i] = model;
	}
}

// Camera Matrix
void eyeViewMatrix(GLint shaderProgramID) {
	Projection = glm::perspective(glm::radians(45.0f), (float)WINDOW_WIDTH / WINDOW_HEIGHT, 1.0f, 120.0f);
	View = glm::lookAt(
		glm::vec3(cameraPosition.x * cos(cameraPosAngle), cameraPosition.y, cameraPosition.z * sin(cameraPosAngle)), // Camera is at (x,y,z), in World Space
		glm::vec3(0.0f, 0.0f, 0.0f), // and looks at point
		glm::vec3(0.0f, 1.0f, 0.0f)  // Head is up (set to 0,-1,0 to look upside-down)
	);
	glm::mat4 Model = glm::mat4(1.0f);
	glm::mat3 VM = glm::mat3(View * Model);
	glm::mat4 pvm = Projection * View * Model;

	GLuint MatrixID = glGetUniformLocation(shaderProgramID, "PVM");
	glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &pvm[0][0]);

	GLuint ViewID = glGetUniformLocation(shaderProgramID, "viewMatrix");
	GLuint ModelID = glGetUniformLocation(shaderProgramID, "modelMatrix");
	GLuint VMID = glGetUniformLocation(shaderProgramID, "viewmodelMatrix");

	glUniformMatrix4fv(ViewID, 1, GL_FALSE, &View[0][0]);
	glUniformMatrix4fv(ModelID, 1, GL_FALSE, &Model[0][0]);
	glUniformMatrix4fv(VMID, 1, GL_FALSE, &VM[0][0]);
}

void lightControl(GLint shaderProgramID) {
	GLuint LightID = glGetUniformLocation(shaderProgramID, "lightPosition");
	glUniform3f(LightID, lightPosition.x, lightPosition.y, lightPosition.z);

	GLuint diffuseID = glGetUniformLocation(shaderProgramID, "diffuseCoefficient");
	glUniform1f(diffuseID, diffuseCoefficient);

	GLuint specularID = glGetUniformLocation(shaderProgramID, "specularCoefficient");
	glUniform1f(specularID, specularCoefficient);
}

// Load objects to buffer
void objDataToOpenGL() {
	// Standard steps to pass one object and its texture
	// jeep->loadObjToBuffer("resource/jeep/jeep.obj");
	// jeep->loadTextureToBuffer("resource/jeep/jeep_texture.bmp", programID);
	// jeep->setScale(glm::vec3(0.3f, 0.3f, 0.3f));
	// jeep->setTransform(glm::vec3(2.0f, 0.3f, -5.0f));
	//

	// Load earth
	earth->loadObjToBuffer("resource/earth/planet.obj");
	earth->loadTextureToBuffer("resource/earth/earth.bmp", programID);
	earth->loadNormalTextureToBuffer("resource/earth/earth_normal.bmp", programID);
	earth->loadSecondTextureToBuffer("resource/sun/sun.bmp", programID);
	earth->setScale(glm::vec3(1.5f, 1.5f, 1.5f));
	// earth->setTransform(glm::vec3(16.0f, 0.0f, 0.0f));

  // Load moon
	moon->loadObjToBuffer("resource/moon/planet.obj");
	moon->loadTextureToBuffer("resource/moon/moon.bmp", programID);
	moon->loadNormalTextureToBuffer("resource/moon/moon_normal.bmp", programID);
	moon->setScale(glm::vec3(0.4f, 0.4f, 0.4f));
	// moon->setTransform(glm::vec3(16.0f, 0.0f, 0.0f));

  // Load saturn
	saturn->loadObjToBuffer("resource/saturn/planet.obj");
	saturn->loadTextureToBuffer("resource/saturn/saturn.bmp", programID);
	saturn->loadNormalTextureToBuffer("resource/saturn/saturn_normal.bmp", programID);
	saturn->setScale(glm::vec3(1.0f, 1.0f, 1.0f));
	saturn->setTransform(glm::vec3(-9.0f, 7.0f, 0.0f));

	// Load sun
	sun->loadObjToBuffer("resource/sun/planet.obj");
	sun->loadTextureToBuffer("resource/sun/sun.bmp", lightSourceProgramID);
	sun->setScale(glm::vec3(0.8f, 0.8f, 0.8f));
	sun->setTransform(glm::vec3(0.0f, 0.0f, 0.0f));

	// Load airplane
	airplane->loadObjToBuffer("resource/airplane/airplane.obj");
	airplane->loadTextureToBuffer("resource/airplane/airplane.bmp", programID);
	airplane->loadNormalTextureToBuffer("resource/airplane/airplane_normal.bmp", programID);
	airplane->setScale(glm::vec3(0.05f, 0.05f, 0.05f));

	// Load rock
	rock->loadObjToBuffer("resource/rock/rock.obj");
	rock->loadTextureToBuffer("resource/rock/rock.bmp", programID);
	createRandomModel(glm::vec3(-9.0f, 7.0f, 0.0f));


	background->loadVerticesToBuffer(100.0f);
	background->create_cube_map("resource/skybox/orbital-element_ft.bmp",
		"resource/skybox/orbital-element_bk.bmp",
		"resource/skybox/orbital-element_up.bmp",
		"resource/skybox/orbital-element_dn.bmp",
		"resource/skybox/orbital-element_lf.bmp",
		"resource/skybox/orbital-element_rt.bmp");
	background->setSelfRotate(glm::vec3(1, 0, 0), 277.0f);
}

// Initialize OpenGL
void initOpenGL() {
	glEnable(GL_CULL_FACE);
	glEnable(GL_POLYGON_SMOOTH);
	// Enable depth test
	glClearDepth(1.0f);
	glDepthFunc(GL_LESS);
	glEnable(GL_DEPTH_TEST);
	// glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	programID = installShaders("VertexShaderCode.glsl", "FragmentShaderCode.glsl");
	lightSourceProgramID = installShaders("LightSourceVertexShaderCode.glsl", "LightSourceFragmentShader.glsl");
	skyboxProgramID = installShaders("SkyBoxVertexShaderCode.glsl", "SkyBoxFragmentShader.glsl");

	objDataToOpenGL();
}

// Keep looping to draw on screen
void drawScreen() {
	orbitalTheta += 0.01f;
	moonTheta += 0.05f;
	saturnAlpha += 0.044f;
	airplaneTheta += 0.03f;
	glClearColor(0.5f, 0.5f, 0.5f, 1.0f); //specify the background color
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	lightPosition = sun->getGlobalOrigin();

	// Standard step to draw one object
	// eyeViewMatrix(programID);
	// jeep->sendMatrix(programID);
	// jeep->renderObject();
  //

	glUseProgram(programID);
	for (GLuint i = 0; i < NUMBER_OF_ROCK; i++) {
		eyeViewMatrix(programID);
		rock->setModelMatrix(rockMatrices[i]);
		rock->setTransform(glm::vec3(1.5f, 1.0f, 1.5f));
		rock->sendMatrix(programID);
		rock->renderObject();
	}

	glUseProgram(programID);
	eyeViewMatrix(programID);
	lightControl(programID);
	earth->setSelfRotate(glm::vec3(0, 1, 0), 0.3);
	earth->setTransform(glm::vec3(20.0f * cos(orbitalTheta), -1.5f, 16.0f * sin(orbitalTheta)));
	earth->sendMatrix(programID);
	earth->renderObject();
	glm::vec3 earthOrigin = earth->getEarthCentre();
	// cout << "{" << earthOrigin.x << ", "  << earthOrigin.z << "}  " << orbitalTheta << endl;

	glUseProgram(programID);
	eyeViewMatrix(programID);
	lightControl(programID);
	moon->setOrigin(earthOrigin);
	moon->setSelfRotate(glm::vec3(0, 1, 0), -0.4f);
	moon->setTransform(glm::vec3(7.0f * cos(moonTheta), 7.0f * cos(moonTheta), 7.0f * sin(moonTheta)));
	moon->sendMatrix(programID);
	moon->renderObject();

	glUseProgram(programID);
	eyeViewMatrix(programID);
<<<<<<< HEAD
  lightControl(programID);
  airplane->setSelfRotate(glm::vec3(1, 0, 0), 1.37f);
  airplane->setOrigin(earthOrigin);
  // airplane->setTransform(glm::vec3(0.0f, 0.0f, 0.0f));
  airplane->setTransform(glm::vec3(0.0f, -9.0f * cos(airplaneTheta), -9.0f * sin(airplaneTheta)));
||||||| merged common ancestors
  lightControl(programID);
  airplane->setSelfRotate(glm::vec3(1, 0, 0), 1.25f);
  airplane->setOrigin(earthOrigin);
  // airplane->setTransform(glm::vec3(0.0f, 0.0f, 0.0f));
  airplane->setTransform(glm::vec3(0.0f, -9.0f * cos(airplaneTheta), -9.0f * sin(airplaneTheta)));
=======
	lightControl(programID);
	airplane->setSelfRotate(glm::vec3(1, 0, 0), 1.25f);
	airplane->setOrigin(earthOrigin);
	// airplane->setTransform(glm::vec3(0.0f, 0.0f, 0.0f));
	airplane->setTransform(glm::vec3(0.0f, -9.0f * cos(airplaneTheta), -9.0f * sin(airplaneTheta)));
>>>>>>> 25cecf604cc4de261f0feec6c431619e22f77b2c
	airplane->sendMatrix(programID);
	airplane->renderObject();

	glUseProgram(programID);
	eyeViewMatrix(programID);
	lightControl(programID);
	saturn->setSelfRotate(glm::vec3(0, 1, 0), -0.2);
	// saturn->setTransform(glm::vec3(-20.0f * cos(orbitalTheta), 7.0f, 12.0f * sin(orbitalTheta)));
	saturn->sendMatrix(programID);
	saturn->renderObject();

	glUseProgram(lightSourceProgramID);
	eyeViewMatrix(lightSourceProgramID);
	lightControl(lightSourceProgramID);
	sun->setSelfRotate(glm::vec3(0, 1, 0), 0.1);
	sun->sendMatrix(lightSourceProgramID);
	sun->renderObject();

	// Order of sending matrices must NOT be changed
	glUseProgram(skyboxProgramID);
	eyeViewMatrix(skyboxProgramID);
	background->renderSkybox();

	glutSwapBuffers();
	glFlush();
	glutPostRedisplay();
}

void timerFunction(int id) {
	earth_innRot_Degree += 0.0003f;

	glutPostRedisplay();
	glutTimerFunc(700.0f / 60.0f, timerFunction, 1);
}

int main(int argc, char *argv[]) {
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGBA);
	// For Mac OS
	glutInitDisplayMode(GLUT_3_2_CORE_PROFILE | GLUT_RGB | GLUT_SINGLE | GLUT_DEPTH | GLUT_MULTISAMPLE);

	// For Windows
	   //glutInitDisplayMode(GLUT_RGB | GLUT_SINGLE | GLUT_DEPTH | GLUT_MULTISAMPLE);

	glutInitWindowSize(WINDOW_WIDTH, WINDOW_HEIGHT);
	glutCreateWindow("CSCI3260 Project");
	glewInit();

	// Create object and point to global variables
	jeep = new Object;
	airplane = new Airplane;
	earth = new Earth;
	sun = new Object;
	saturn = new Object;
	moon = new Object;
	rock = new Object;
	background = new Skybox;
  flightTrack = new positionArray(20);

	initOpenGL();
	glutDisplayFunc(drawScreen);

	// Mouse and keyboard functions are in Library/gesture.cpp
	glutKeyboardFunc(keyboardClick);
	glutSpecialFunc(arrowKey);
	glutPassiveMotionFunc(mouseCoordinate);
	glutMouseFunc(mouseWheelFunc);

	glutTimerFunc(700.0f / 60.0f, timerFunction, 1);

	glutMainLoop();
	return 0;
}
