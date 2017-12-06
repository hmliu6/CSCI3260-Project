#ifdef __APPLE__
	#include <GLUT/glut.h>
#elif defined _WIN32 || defined _WIN64
	#include "Dependencies/freeglut/freeglut.h"
#endif

#include <iostream>
#include <cmath>
#include "Dependencies/glm/glm.hpp"
#include "Dependencies/glm/gtc/matrix_transform.hpp"
#include "constant.hpp"

struct boundingBox{
    glm::vec3 minCubeVertex;
    glm::vec3 maxCubeVertex;
};

bool collisionTest(boundingBox *objectA, boundingBox *objectB){
    // Case 1: object A is inside object B
    bool check = objectA->maxCubeVertex.x <= objectB->maxCubeVertex.x &&
                 objectA->maxCubeVertex.y <= objectB->maxCubeVertex.y &&
                 objectA->maxCubeVertex.z <= objectB->maxCubeVertex.z &&
                 objectA->minCubeVertex.x >= objectB->minCubeVertex.x &&
                 objectA->minCubeVertex.y >= objectB->minCubeVertex.y &&
                 objectA->minCubeVertex.z >= objectB->minCubeVertex.z;
    if(check == true)
        return true;
    
    // Case 2: object B is inside object A
    check = objectA->maxCubeVertex.x >= objectB->maxCubeVertex.x &&
            objectA->maxCubeVertex.y >= objectB->maxCubeVertex.y &&
            objectA->maxCubeVertex.z >= objectB->maxCubeVertex.z &&
            objectA->minCubeVertex.x <= objectB->minCubeVertex.x &&
            objectA->minCubeVertex.y <= objectB->minCubeVertex.y &&
            objectA->minCubeVertex.z <= objectB->minCubeVertex.z;
    if(check == true)
        return true;

    // Case 3: Partial collision, i.e. object A max is in between object B min and max
    check = objectA->maxCubeVertex.x >= objectB->minCubeVertex.x &&
            objectA->maxCubeVertex.y >= objectB->minCubeVertex.y &&
            objectA->maxCubeVertex.z >= objectB->minCubeVertex.z &&
            objectA->maxCubeVertex.x <= objectB->maxCubeVertex.x &&
            objectA->maxCubeVertex.y <= objectB->maxCubeVertex.y &&
            objectA->maxCubeVertex.z <= objectB->maxCubeVertex.z;
    if(check == true)
        return true;

    // Case 4: Partial collision, i.e. object b max is in between object A min and max
    check = objectB->maxCubeVertex.x >= objectA->minCubeVertex.x &&
            objectB->maxCubeVertex.y >= objectA->minCubeVertex.y &&
            objectB->maxCubeVertex.z >= objectA->minCubeVertex.z &&
            objectB->maxCubeVertex.x <= objectA->maxCubeVertex.x &&
            objectB->maxCubeVertex.y <= objectA->maxCubeVertex.y &&
            objectB->maxCubeVertex.z <= objectA->maxCubeVertex.z;
    if(check == true)
        return true;
    
    return false;
}