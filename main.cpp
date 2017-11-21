/*********************************************************
FILE : main.cpp (csci3260 2017-2018 Assignment 2)
*********************************************************/
/*********************************************************
Student Information
Student ID: 1155081383
Student Name: Yau Yui Pan
*********************************************************/
#define _CRT_SECURE_NO_DEPRECATE
#include "Dependencies\glew\glew.h"
#include "Dependencies\freeglut\freeglut.h"
#include "Dependencies\glm\glm.hpp"
#include "Dependencies\glm\gtc\matrix_transform.hpp"
#include <iostream>
#include <fstream>
#include <vector>
#include <time.h>
#include <ctime>

#define WINDOW_WIDTH 1024
#define WINDOW_HEIGHT 768

#define NUM_OF_OBJ 8
#define INDEX_OF_GROUND 0
#define INDEX_OF_JEEP 1
#define INDEX_OF_AIRPLANE 2
#define INDEX_OF_TOWER 3
#define INDEX_OF_JEEP2 4
#define INDEX_OF_JEEP3 5
#define INDEX_OF_JEEP4 6
#define INDEX_OF_JEEP5 7

typedef struct CameraPosition {
	float x, y, z,
		startX, startY, startZ;
}CameraPosition;

using namespace std;
using glm::vec3;
using glm::mat4;

GLint programID;
// Could define the Vao&Vbo and interaction parameter here
GLuint vaoIDs[NUM_OF_OBJ];
GLuint vboIDs[NUM_OF_OBJ];
GLuint uvboIDs[NUM_OF_OBJ];
GLuint norboIDs[NUM_OF_OBJ];

GLuint textureIDs[NUM_OF_OBJ];
//GLuint groundTexture;
GLuint sizeOfVertices[NUM_OF_OBJ];
GLuint TextureID;
int startTime = 0;
int gc = 0;

std::clock_t start;

// Camera position
CameraPosition cameraPosition = {
	15.0f, 15.0f, 7.0f,
	0.0f, 0.0f, 0.0f,
};

//typedef struct CricleAnimation {
//	float cx, cy,
//		radius,
//		speed, speedScale;
//}CricleAnimation;
//
//
//CricleAnimation cricleAnimation = {
//	4.0f,4.0f,
//	3.0f,
//	2.0f, (0.001 * 2 * Math.PI) / 2.0f,
//};

//mouse

// --State of control-------- 

GLushort passiveMouseMode = 0;
GLushort animationMode = 1;
float animationAngle = 1;

GLushort contollingObjID = 1;
GLushort rotatingAxisIsX = 0;
GLushort rotatingAxisIsY = 1;
GLushort rotatingAxisIsZ = 0;

float x_delta[NUM_OF_OBJ] = { 0.1f };
float y_delta[NUM_OF_OBJ] = { 0.1f };
float z_delta[NUM_OF_OBJ] = { 0.1f };
int x_press_num[NUM_OF_OBJ] = { 0 };
int y_press_num[NUM_OF_OBJ] = { 0 };
int z_press_num[NUM_OF_OBJ] = { 0 };

float jeep_x = 0.0f;
float jeep_y = 0.0f;
float jeep_rotation_delta = 1.0f;
int jeep_rotation_press_num = 0;

float scale_delta[NUM_OF_OBJ] = { 0.5f };
int scale_press_num[NUM_OF_OBJ] = { 0 };

float rotation_delta[NUM_OF_OBJ] = { 3.0f };
float rotation_theta[NUM_OF_OBJ] = { 0 };

// light
float diffuseLightIntensity = 4.8f;
int diffuseLightIntensity_press_num = 0;
float diffuseLightIntensity_delta = 0.1f;

float specularLightIntensity = -0.4f;
int specularLightIntensity_press_num = 0;
float specularLightIntensity_delta = 0.1f;


// -------------------------



bool checkStatus(
	GLuint objectID,
	PFNGLGETSHADERIVPROC objectPropertyGetterFunc,
	PFNGLGETSHADERINFOLOGPROC getInfoLogFunc,
	GLenum statusType)
{
	GLint status;
	objectPropertyGetterFunc(objectID, statusType, &status);
	if (status != GL_TRUE)
	{
		GLint infoLogLength;
		objectPropertyGetterFunc(objectID, GL_INFO_LOG_LENGTH, &infoLogLength);
		GLchar* buffer = new GLchar[infoLogLength];

		GLsizei bufferSize;
		getInfoLogFunc(objectID, infoLogLength, &bufferSize, buffer);
		cout << buffer << endl;

		delete[] buffer;
		return false;
	}
	return true;
}

bool checkShaderStatus(GLuint shaderID)
{
	return checkStatus(shaderID, glGetShaderiv, glGetShaderInfoLog, GL_COMPILE_STATUS);
}

bool checkProgramStatus(GLuint programID)
{
	return checkStatus(programID, glGetProgramiv, glGetProgramInfoLog, GL_LINK_STATUS);
}

string readShaderCode(const char* fileName)
{
	ifstream meInput(fileName);
	if (!meInput.good())
	{
		cout << "File failed to load..." << fileName;
		exit(1);
	}
	return std::string(
		std::istreambuf_iterator<char>(meInput),
		std::istreambuf_iterator<char>()
	);
}

void installShaders()
{
	GLuint vertexShaderID = glCreateShader(GL_VERTEX_SHADER);
	GLuint fragmentShaderID = glCreateShader(GL_FRAGMENT_SHADER);

	const GLchar* adapter[1];
	string temp = readShaderCode("VertexShaderCode.glsl");
	adapter[0] = temp.c_str();
	glShaderSource(vertexShaderID, 1, adapter, 0);
	temp = readShaderCode("FragmentShaderCode.glsl");
	adapter[0] = temp.c_str();
	glShaderSource(fragmentShaderID, 1, adapter, 0);

	glCompileShader(vertexShaderID);
	glCompileShader(fragmentShaderID);

	if (!checkShaderStatus(vertexShaderID) || !checkShaderStatus(fragmentShaderID))
		return;

	programID = glCreateProgram();
	glAttachShader(programID, vertexShaderID);
	glAttachShader(programID, fragmentShaderID);
	glLinkProgram(programID);

	if (!checkProgramStatus(programID))
		return;

	glDeleteShader(vertexShaderID);
	glDeleteShader(fragmentShaderID);

	glUseProgram(programID);
}

void resetControl() {
	// reset those object into initial point
	for (int i = 0; i < NUM_OF_OBJ; i++) {
		x_delta[i] = 0.1f;
		y_delta[i] = 0.1f;
		z_delta[i] = 0.1f;
		x_press_num[i] = 0;
		y_press_num[i] = 0;
		z_press_num[i] = 0;

		scale_delta[i] = 0.1f;
		scale_press_num[i] = 0;

		rotation_delta[i] = 3.0f;
		rotation_theta[i] = 0;
	}
	diffuseLightIntensity_press_num = 0;
	specularLightIntensity_press_num = 0;

	jeep_x = 0.0f;
	jeep_y = 0.0f;
	jeep_rotation_delta = 1.0f;
	jeep_rotation_press_num = 0;
}

void displayControl() {
	// reset those object into initial point
	for (int i = 0; i < NUM_OF_OBJ; i++) {
		printf("------- Obj %i ----------\n", i);
		printf("X delta %f * %i = %f\n", x_delta[i], x_press_num[i], x_delta[i] * x_press_num[i]);
		printf("Y delta %f * %i = %f\n", y_delta[i], y_press_num[i], y_delta[i] * y_press_num[i]);
		printf("Z delta %f * %i = %f\n", z_delta[i], z_press_num[i], z_delta[i] * z_press_num[i]);

		printf("Scale delta %f * %i = %f\n", scale_delta[i], scale_press_num[i], scale_delta[i] * scale_press_num[i]);
		printf("Rotation delta %f * %i = %f\n", rotation_delta[i], rotation_theta[i], rotation_delta[i] * rotation_theta[i]);
	}
	printf("------- Light----------\n");
	printf("Specular light: %f\n", specularLightIntensity + specularLightIntensity_press_num * specularLightIntensity_delta);
	printf("Diffuse light: %f\n", diffuseLightIntensity + diffuseLightIntensity_press_num * diffuseLightIntensity_delta);
	printf("------- Airplane----------\n");
	printf("Airplane angle: %f\n", animationAngle);
	printf("------- Camera Position----------\n");
	printf("Camera Position: %f, %f\n", cameraPosition.x, cameraPosition.y);
}

void keyboard(unsigned char key, int x, int y)
{
	//TODO:
	// Comment in readme.txt

	if (key == '0')
	{
		contollingObjID = INDEX_OF_GROUND;
	}

	if (key == '1')
	{
		contollingObjID = INDEX_OF_JEEP;
	}
	if (key == '2')
	{
		contollingObjID = INDEX_OF_AIRPLANE;
	}

	if (key == '3')
	{
		contollingObjID = INDEX_OF_TOWER;
	}

	if (key == '4')
	{
		contollingObjID = 4;
	}

	if (key == '5')
	{
		contollingObjID = 5;
	}

	if (key == '6')
	{
		contollingObjID = 6;
	}

	if (key == '7')
	{
		contollingObjID = 7;
	}

	// light
	if (key == ' ')
	{
		if (passiveMouseMode == 0) {
			passiveMouseMode = 1;
		}
		else {
			passiveMouseMode = 0;
		}
	}

	if (key == 'q')
	{
		diffuseLightIntensity_press_num += 1;
	}
	if (key == 'w')
	{
		if (diffuseLightIntensity + diffuseLightIntensity_press_num * diffuseLightIntensity_delta > -2.0f)
			diffuseLightIntensity_press_num -= 1;
		else
			diffuseLightIntensity_press_num = diffuseLightIntensity;
	}

	if (key == 'z')
	{
		specularLightIntensity_press_num += 1;
	}
	if (key == 'x')
	{
		if (specularLightIntensity + specularLightIntensity_delta * specularLightIntensity_press_num > -2.0f)
			specularLightIntensity_press_num -= 1;
		else
			specularLightIntensity_press_num = specularLightIntensity;

	}

	// model view

	if (key == 'j')
	{
		x_press_num[contollingObjID] -= 1;
	}
	if (key == 'l')
	{
		x_press_num[contollingObjID] += 1;
	}

	if (key == 'i')
	{
		y_press_num[contollingObjID] += 1;
	}

	if (key == 'k')
	{
		y_press_num[contollingObjID] -= 1;
	}

	if (key == 'u')
	{
		z_press_num[contollingObjID] += 1;
	}

	if (key == 'o')
	{
		z_press_num[contollingObjID] -= 1;
	}


	if (key == 'r')
	{
		rotation_theta[contollingObjID] += rotation_delta[contollingObjID];
	}

	if (key == 't')
	{
		rotation_theta[contollingObjID] -= rotation_delta[contollingObjID];
	}

	if (key == ',')
	{
		rotatingAxisIsX = 1;
		rotatingAxisIsY = 0;
		rotatingAxisIsZ = 0;
	}
	if (key == '.')
	{
		rotatingAxisIsX = 0;
		rotatingAxisIsY = 1;
		rotatingAxisIsZ = 0;
	}
	if (key == '/')
	{
		rotatingAxisIsX = 0;
		rotatingAxisIsY = 0;
		rotatingAxisIsZ = 1;
	}

	if (key == 'f')
	{
		scale_press_num[contollingObjID] += 1;
	}

	if (key == 'g')
	{
		scale_press_num[contollingObjID] -= 1;
	}

	if (key == 'c') {
		resetControl();
	}

	if (key == '-') {
		displayControl();
	}
	else if (key == 's') {
		if (animationMode == 0) {
			animationMode = 1;
		}
		else {
			animationMode = 0;
		}
	}


}

void move(int key, int x, int y)
{
	//TODO: Use arrow keys to do interactive events and animation
	switch (key) {
	case GLUT_KEY_UP:
		jeep_x += 0.1f * cos(jeep_rotation_delta * jeep_rotation_press_num);
		jeep_y += 0.1f * sin(jeep_rotation_delta * jeep_rotation_press_num);
		break;
	case GLUT_KEY_DOWN:
		jeep_x -= 0.1f * cos(jeep_rotation_delta * jeep_rotation_press_num);
		jeep_y -= 0.1f * sin(jeep_rotation_delta * jeep_rotation_press_num);
		break;
	case GLUT_KEY_LEFT:
		jeep_rotation_press_num += 1;
		break;
	case GLUT_KEY_RIGHT:
		jeep_rotation_press_num -= 1;
		break;
	}
}

void PassiveMouse(int x, int y)
{
	//TODO: Use Mouse to do interactive events and animation
	if (passiveMouseMode == 1) {
		cameraPosition.x = glm::clamp((WINDOW_WIDTH / 2.0f - x) / 5.0f, -50.0f, 50.0f);
		cameraPosition.y = glm::clamp((WINDOW_HEIGHT / 2.0f - y) / 5.0f, 0.0f, 70.0f);
	}
}

bool loadOBJ(
	const char * path,
	std::vector<glm::vec3> & out_vertices,
	std::vector<glm::vec2> & out_uvs,
	std::vector<glm::vec3> & out_normals
) {
	printf("Loading OBJ file %s...\n", path);

	std::vector<unsigned int> vertexIndices, uvIndices, normalIndices;
	std::vector<glm::vec3> temp_vertices;
	std::vector<glm::vec2> temp_uvs;
	std::vector<glm::vec3> temp_normals;


	FILE * file = fopen(path, "r");
	if (file == NULL) {
		printf("Impossible to open the file ! Are you in the right path ? See Tutorial 1 for details\n");
		getchar();
		return false;
	}

	while (1) {

		char lineHeader[128];
		int res = fscanf(file, "%s", lineHeader);
		if (res == EOF)
			break; // EOF = End Of File. Quit the loop.

				   // else : parse lineHeader

		if (strcmp(lineHeader, "v") == 0) {
			glm::vec3 vertex;
			fscanf(file, "%f %f %f\n", &vertex.x, &vertex.y, &vertex.z);
			temp_vertices.push_back(vertex);
		}
		else if (strcmp(lineHeader, "vt") == 0) {
			glm::vec2 uv;
			fscanf(file, "%f %f\n", &uv.x, &uv.y);
			uv.y = -uv.y;
			temp_uvs.push_back(uv);
		}
		else if (strcmp(lineHeader, "vn") == 0) {
			glm::vec3 normal;
			fscanf(file, "%f %f %f\n", &normal.x, &normal.y, &normal.z);
			temp_normals.push_back(normal);
		}
		else if (strcmp(lineHeader, "f") == 0) {
			std::string vertex1, vertex2, vertex3;
			unsigned int vertexIndex[3], uvIndex[3], normalIndex[3];
			int matches = fscanf(file, "%d/%d/%d %d/%d/%d %d/%d/%d\n", &vertexIndex[0], &uvIndex[0], &normalIndex[0], &vertexIndex[1], &uvIndex[1], &normalIndex[1], &vertexIndex[2], &uvIndex[2], &normalIndex[2]);
			if (matches != 9) {
				printf("File can't be read by our simple parser :-( Try exporting with other options\n");
				return false;
			}
			vertexIndices.push_back(vertexIndex[0]);
			vertexIndices.push_back(vertexIndex[1]);
			vertexIndices.push_back(vertexIndex[2]);
			uvIndices.push_back(uvIndex[0]);
			uvIndices.push_back(uvIndex[1]);
			uvIndices.push_back(uvIndex[2]);
			normalIndices.push_back(normalIndex[0]);
			normalIndices.push_back(normalIndex[1]);
			normalIndices.push_back(normalIndex[2]);
		}
		else {
			char stupidBuffer[1000];
			fgets(stupidBuffer, 1000, file);
		}

	}

	// For each vertex of each triangle
	for (unsigned int i = 0; i < vertexIndices.size(); i++) {

		// Get the indices of its attributes
		unsigned int vertexIndex = vertexIndices[i];
		unsigned int uvIndex = uvIndices[i];
		unsigned int normalIndex = normalIndices[i];

		// Get the attributes thanks to the index
		glm::vec3 vertex = temp_vertices[vertexIndex - 1];
		glm::vec2 uv = temp_uvs[uvIndex - 1];
		glm::vec3 normal = temp_normals[normalIndex - 1];

		// Put the attributes in buffers
		out_vertices.push_back(vertex);
		out_uvs.push_back(uv);
		out_normals.push_back(normal);

	}

	return true;
}

GLuint loadBMP_custom(const char * imagepath) {

	printf("Reading image %s\n", imagepath);

	unsigned char header[54];
	unsigned int dataPos;
	unsigned int imageSize;
	unsigned int width, height;
	unsigned char * data;

	FILE * file = fopen(imagepath, "rb");
	if (!file) { printf("%s could not be opened. Are you in the right directory ? Don't forget to read the FAQ !\n", imagepath); getchar(); return 0; }

	if (fread(header, 1, 54, file) != 54) {
		printf("Not a correct BMP file\n");
		return 0;
	}
	if (header[0] != 'B' || header[1] != 'M') {
		printf("Not a correct BMP file\n");
		return 0;
	}
	if (*(int*)&(header[0x1E]) != 0) { printf("Not a correct BMP file\n");    return 0; }
	if (*(int*)&(header[0x1C]) != 24) { printf("Not a correct BMP file\n");    return 0; }

	dataPos = *(int*)&(header[0x0A]);
	imageSize = *(int*)&(header[0x22]);
	width = *(int*)&(header[0x12]);
	height = *(int*)&(header[0x16]);
	if (imageSize == 0)    imageSize = width*height * 3;
	if (dataPos == 0)      dataPos = 54;

	data = new unsigned char[imageSize];
	fread(data, 1, imageSize, file);
	fclose(file);


	GLuint textureID;
	//TODO: Create one OpenGL texture and set the texture parameter 
	glGenTextures(1, &textureID);

	glBindTexture(GL_TEXTURE_2D, textureID);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_BGR, GL_UNSIGNED_BYTE, data);
	delete[] data;
	glGenerateMipmap(GL_TEXTURE_2D);




	return textureID;
}

void lightHanlding(GLuint object_id) {
	float intensity = 0.0f;
	// -- Light Handling
	GLint ambientLightUniformLocation = glGetUniformLocation(programID, "ambientLight");
	GLint diffuseLightUniformLocation = glGetUniformLocation(programID, "diffuseLightColor");
	GLint specularLightUniformLocation = glGetUniformLocation(programID, "specularLightColor");
	GLint eyePositiontUniformLocation = glGetUniformLocation(programID, "eyePositionWorld");
	GLint lightPositiontUniformLocation = glGetUniformLocation(programID, "lightPositionWorld");
	// ambient light
	vec3 ambientLight = vec3(0.1f, 0.1f, 0.1f);
	if (object_id == INDEX_OF_JEEP2) {
		ambientLight = vec3(0.1f, 0.3f, 0.1f);
	}else if (object_id == INDEX_OF_JEEP3) {
		ambientLight = vec3(0.1f, 0.3f, 0.3f);
	}
	else if (object_id == INDEX_OF_JEEP4) {
		ambientLight = vec3(0.3f, 0.3f, 0.1f);
	}
	else if (object_id == INDEX_OF_JEEP5) {
		ambientLight = vec3(0.4f, 0.1f, 0.3f);
	}
	glUniform3fv(ambientLightUniformLocation, 1, &ambientLight[0]);
	// diffuse light
	intensity = diffuseLightIntensity + diffuseLightIntensity_press_num * diffuseLightIntensity_delta;
	vec3 diffuseLight = vec3(intensity, intensity, intensity);
	glUniform3fv(diffuseLightUniformLocation, 1, &diffuseLight[0]);
	// specular light
	intensity = specularLightIntensity + specularLightIntensity_press_num * specularLightIntensity_delta;
	vec3 specularLight = vec3(intensity, intensity, intensity);
	glUniform3fv(specularLightUniformLocation, 1, &specularLight[0]);
	// eye position
	vec3 eyePosition(cameraPosition.x, cameraPosition.y, cameraPosition.z);
	glUniform3fv(eyePositiontUniformLocation, 1, &eyePosition[0]);
	// diffuse light
	vec3 lighPosition(-7.5f, 10.5f, 0.5f);
	glUniform3fv(lightPositiontUniformLocation, 1, &lighPosition[0]);

}

void ModelWorldProjectionTransform(GLuint object_id) {
	// Model Part

	int diff = std::clock() - start;

	glm::mat4 modelTransformMatrix = glm::mat4(1.0f);
	glm::mat4 modelRotateMatrix = glm::mat4(1.0f);
	glm::mat4 modelScaleMatrix = glm::mat4(1.0f);

	if (object_id != INDEX_OF_GROUND) {
		//if (object_id != -1) {
		float initial_x_shift = 0;
		float initial_y_shift = 0;
		float initial_z_shift = 0;
		float initial_scale = 1.0f;
		float initial_rotation = 0;
		// for world space initial point adjust
		// --Scale Rotate Translate Part
		if (object_id == INDEX_OF_JEEP) {
			//initial_x_shift = -4.5f;
			initial_y_shift = -0.85f;
			//initial_z_shift = -2.6f;
			initial_scale = 0.5f;
			modelTransformMatrix = glm::translate(glm::mat4(),
				glm::vec3(initial_x_shift + jeep_x,
					initial_y_shift + y_delta[object_id] * y_press_num[object_id],
					initial_z_shift + jeep_y)
			);
			mat4 selfRotation = glm::rotate(glm::mat4(), glm::radians(jeep_rotation_delta*jeep_rotation_press_num),
				glm::vec3((float)0, (float)rotatingAxisIsY, (float)0));

			float scale_index = initial_scale + scale_press_num[object_id] * scale_delta[object_id];
			modelScaleMatrix = selfRotation * glm::scale(glm::mat4(),
				glm::vec3(scale_index, scale_index, scale_index));
		}
		else if (object_id == INDEX_OF_AIRPLANE) {
			initial_x_shift = 0.5f;
			initial_y_shift = 7.3f;
			initial_z_shift = -0.1f;
			initial_scale = 0.2f;
			//CricleAnimation
			glm::mat4 selfRotation = glm::rotate(glm::mat4(), -glm::radians(animationAngle), glm::vec3(0, 1, 0));

			modelTransformMatrix = glm::translate(glm::mat4(),
				glm::vec3(
					initial_x_shift + 12.0f * cos(glm::radians(animationAngle)),
					initial_y_shift + y_delta[object_id] * y_press_num[object_id],
					initial_z_shift + 10.0f *sin(glm::radians(animationAngle))
				)
			);
			modelRotateMatrix = glm::rotate(glm::mat4(), glm::radians(0.0f),
				glm::vec3((float)0, (float)rotatingAxisIsY, (float)0));

			float scale_index = initial_scale + scale_press_num[object_id] * scale_delta[object_id];
			modelScaleMatrix = selfRotation *
				glm::scale(glm::mat4(), glm::vec3(scale_index, scale_index, scale_index));
		}
		else {
			if (object_id == INDEX_OF_TOWER) {
				initial_x_shift = -17.5f;
				initial_y_shift = 0.05f;
				initial_z_shift = -3.50f;
				initial_scale = 1.0f;
				initial_rotation = 0.0f;
			}else if (object_id == INDEX_OF_JEEP2) {
				initial_x_shift = -6.4f;
				initial_y_shift = -0.85f;
				initial_z_shift = 3.4f;
				initial_scale = 0.5f;
				initial_rotation = 20.0f;
			}else if (object_id == INDEX_OF_JEEP3) {
				initial_x_shift = -7.2f;
				initial_y_shift = -0.85f;
				initial_z_shift = 0.0f;
				initial_scale = 0.5f;
				initial_rotation = 14.0f;
			}else if (object_id == INDEX_OF_JEEP4) {
				initial_x_shift = -9.2f;
				initial_y_shift = -0.85f;
				initial_z_shift = -3.0f;
				initial_scale = 0.5f;
				initial_rotation = 14.0f;
			}
			else if (object_id == INDEX_OF_JEEP5) {
				initial_x_shift = -4.3f;
				initial_y_shift = -0.85f;
				initial_z_shift = 6.8f;
				initial_scale = 0.5f;
				initial_rotation = 14.0f;
			}
			modelTransformMatrix = glm::translate(glm::mat4(),
				glm::vec3(initial_x_shift + x_delta[object_id] * x_press_num[object_id],
					initial_y_shift + y_delta[object_id] * y_press_num[object_id],
					initial_z_shift + z_delta[object_id] * z_press_num[object_id])
			);
			modelRotateMatrix = glm::rotate(glm::mat4(), glm::radians(initial_rotation + rotation_theta[object_id]),
				glm::vec3((float)rotatingAxisIsX, (float)rotatingAxisIsY, (float)rotatingAxisIsZ));

			float scale_index = initial_scale + scale_press_num[object_id] * scale_delta[object_id];
			modelScaleMatrix = glm::scale(glm::mat4(),
				glm::vec3(scale_index, scale_index, scale_index));
		}

	}
	else {
		modelTransformMatrix = glm::translate(glm::mat4(), glm::vec3(-3.1f, -1.5f, -1.1f));;
		modelRotateMatrix = glm::rotate(glm::mat4(), glm::radians(18.0f), glm::vec3(0, 1, 0));
		modelScaleMatrix = glm::scale(glm::mat4(), glm::vec3(2.5f, 2.0f, 2.5f));
	}

	//printf("diff time: %i\n", diff);
	lightHanlding(object_id);


	// --View Projection Part sqr-

	//matrix : 90° Field of View
	glm::mat4 Projection = glm::perspective(
		glm::radians(50.0f), // Field of View
		1.0f, // ratio
		2.0f,
		100.0f);
	glm::mat4 View = glm::lookAt(
		glm::vec3(cameraPosition.x, cameraPosition.y, cameraPosition.z), // Camera is at (x,y,z), in World Space
		glm::vec3(0, 0.1f, 0), // and looks at point
		glm::vec3(0, 1.0f, 0)  // Head is up (set to 0,-1,0 to look upside-down)
	);
	glm::mat4 Model = glm::mat4(1.0f) * modelTransformMatrix * modelRotateMatrix * modelScaleMatrix;;
	glm::mat4 mvp = Projection * View * Model;

	GLuint MVPMatrixID = glGetUniformLocation(programID, "MVP");
	GLuint ModelMatrixID = glGetUniformLocation(programID, "modelMatrix");
	GLuint ViewMatrixID = glGetUniformLocation(programID, "viewMatrix");

	glUniformMatrix4fv(MVPMatrixID, 1, GL_FALSE, &mvp[0][0]);
	glUniformMatrix4fv(ModelMatrixID, 1, GL_FALSE, &Model[0][0]);
	glUniformMatrix4fv(ViewMatrixID, 1, GL_FALSE, &View[0][0]);
}

void importObj(const char* objFile, const char* textutreFile, GLuint index) {
	std::vector< glm::vec3 > vertices;
	std::vector< glm::vec2 > uvs;
	std::vector< glm::vec3 > normals; // Won't be used at the moment.

	printf("MVP OBJ %i...\n", index);
	// Create vertex arrays object and vertex buffers object
	// --First Object
	bool res = loadOBJ(objFile, vertices, uvs, normals);

	glBindVertexArray(vaoIDs[index]);
	glBindBuffer(GL_ARRAY_BUFFER, vboIDs[index]);
	//  vertex position
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(glm::vec3), &vertices[0], GL_STATIC_DRAW);
	//
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void *)0);
	// uv
	glBindBuffer(GL_ARRAY_BUFFER, uvboIDs[index]);
	glBufferData(GL_ARRAY_BUFFER, uvs.size() * sizeof(glm::vec2), &uvs[0], GL_STATIC_DRAW);
	// 
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, (void *)0);
	// normal
	glBindBuffer(GL_ARRAY_BUFFER, norboIDs[index]);
	glBufferData(GL_ARRAY_BUFFER, normals.size() * sizeof(glm::vec3), &normals[0], GL_STATIC_DRAW);
	// 
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, (void *)0);


	sizeOfVertices[index] = vertices.size();
	// texture
	textureIDs[index] = loadBMP_custom(textutreFile);
	TextureID = glGetUniformLocation(programID, "myTextureSampler");
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, textureIDs[index]);
	glUniform1i(TextureID, 0);


	vertices.clear();
	uvs.clear();
	normals.clear();
}

void sendDataToOpenGL()
{
	//TODO:
	resetControl();
	//Load objects and bind to VAO & VBO
	//Load texture
	// Read our .obj file
	glGenVertexArrays(NUM_OF_OBJ, vaoIDs);
	glGenBuffers(NUM_OF_OBJ, vboIDs);
	glGenBuffers(NUM_OF_OBJ, uvboIDs);
	glGenBuffers(NUM_OF_OBJ, norboIDs);

	importObj("plane.obj", "plane_texture2.bmp", INDEX_OF_GROUND);
	importObj("jeep.obj", "jeep_texture.bmp", INDEX_OF_JEEP);
	//importObj("jeep.obj", "jeep_texture.bmp", INDEX_OF_AIRPLANE);
	importObj("airplane.obj", "airplane_texture.bmp", INDEX_OF_AIRPLANE);
	importObj("water_tower.obj", "water_tower.bmp", INDEX_OF_TOWER);
	importObj("jeep.obj", "jeep_texture.bmp", INDEX_OF_JEEP2);
	importObj("jeep.obj", "jeep_texture.bmp", INDEX_OF_JEEP3);
	importObj("jeep.obj", "jeep_texture.bmp", INDEX_OF_JEEP4);
	importObj("jeep.obj", "jeep_texture.bmp", INDEX_OF_JEEP5);



}

void paintGL(void)
{
	glClearColor(0.5f, 0.5f, 0.5f, 0.5f); //specify the background color
	glClear(GL_COLOR_BUFFER_BIT);
	// Clear the screen
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Enable depth test
	glClearDepth(1.0f);
	glDepthFunc(GL_LESS);
	glEnable(GL_DEPTH_TEST);
	// Accept fragment if it closer to the camera than the former one
	//TODO:
	//Set lighting information, such as position and color of lighting source
	//Set transformation matrix
	//Bind different textures

	// first VAO
	for (int index = 0; index < NUM_OF_OBJ; index++) {
		ModelWorldProjectionTransform(index);
		glBindVertexArray(vaoIDs[index]);
		glBindTexture(GL_TEXTURE_2D, textureIDs[index]);
		glDrawArrays(GL_TRIANGLES, 0, sizeOfVertices[index]); //render primitives from array data
															  //printf("Drawing OBJ %i...\n", index);
	}

	if (animationMode == 1) {
		animationAngle += 0.06f;
		if (animationAngle > 360) {
			animationAngle = 0.0f;
		}
	}

	//glutSwapBuffers();
	glFlush();
	glutPostRedisplay();
}

void initializedGL(void)
{
	glewInit();
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);
	glEnable(GL_CULL_FACE);
	installShaders();
	sendDataToOpenGL();
}

int main(int argc, char *argv[])
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGB | GLUT_SINGLE);
	//TODO:
	//Create a window with title specified
	glutInitWindowSize(WINDOW_WIDTH, WINDOW_HEIGHT);
	glutCreateWindow("Assignment 2");
	start = std::clock();
	printf("start time: %i\n", start);
	initializedGL();
	glutDisplayFunc(paintGL);

	glutKeyboardFunc(keyboard);
	glutSpecialFunc(move);
	glutPassiveMotionFunc(PassiveMouse);

	glutMainLoop();


	return 0;
}