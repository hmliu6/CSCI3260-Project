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
    float thresold = 0.4f;
    // Case 1: object A is inside object B
    bool check = objectA->maxCubeVertex.x - thresold <= objectB->maxCubeVertex.x &&
                 objectA->maxCubeVertex.y - thresold <= objectB->maxCubeVertex.y &&
                 objectA->maxCubeVertex.z - thresold <= objectB->maxCubeVertex.z &&
                 objectA->minCubeVertex.x + thresold >= objectB->minCubeVertex.x &&
                 objectA->minCubeVertex.y + thresold >= objectB->minCubeVertex.y &&
                 objectA->minCubeVertex.z + thresold >= objectB->minCubeVertex.z;
    if(check == true)
        return true;
    
    // Case 2: object B is inside object A
    check = objectA->maxCubeVertex.x + thresold >= objectB->maxCubeVertex.x &&
            objectA->maxCubeVertex.y + thresold >= objectB->maxCubeVertex.y &&
            objectA->maxCubeVertex.z + thresold >= objectB->maxCubeVertex.z &&
            objectA->minCubeVertex.x - thresold <= objectB->minCubeVertex.x &&
            objectA->minCubeVertex.y - thresold <= objectB->minCubeVertex.y &&
            objectA->minCubeVertex.z - thresold <= objectB->minCubeVertex.z;
    if(check == true)
        return true;

    // Case 3: Partial collision, i.e. object A max is in between object B min and max
    check = objectA->maxCubeVertex.x + thresold >= objectB->minCubeVertex.x &&
            objectA->maxCubeVertex.y + thresold >= objectB->minCubeVertex.y &&
            objectA->maxCubeVertex.z + thresold >= objectB->minCubeVertex.z &&
            objectA->maxCubeVertex.x - thresold <= objectB->maxCubeVertex.x &&
            objectA->maxCubeVertex.y - thresold <= objectB->maxCubeVertex.y &&
            objectA->maxCubeVertex.z - thresold <= objectB->maxCubeVertex.z;
    if(check == true)
        return true;

    // Case 4: Partial collision, i.e. object A min is in between object B min and max
    check = objectA->minCubeVertex.x + thresold >= objectB->minCubeVertex.x &&
            objectA->minCubeVertex.y + thresold >= objectB->minCubeVertex.y &&
            objectA->minCubeVertex.z + thresold >= objectB->minCubeVertex.z &&
            objectA->minCubeVertex.x - thresold <= objectB->maxCubeVertex.x &&
            objectA->minCubeVertex.y - thresold <= objectB->maxCubeVertex.y &&
            objectA->minCubeVertex.z - thresold <= objectB->maxCubeVertex.z;
    if(check == true)
        return true;

    // Case 5: Partial collision, i.e. object b max is in between object A min and max
    check = objectB->maxCubeVertex.x + thresold >= objectA->minCubeVertex.x &&
            objectB->maxCubeVertex.y + thresold >= objectA->minCubeVertex.y &&
            objectB->maxCubeVertex.z + thresold >= objectA->minCubeVertex.z &&
            objectB->maxCubeVertex.x - thresold <= objectA->maxCubeVertex.x &&
            objectB->maxCubeVertex.y - thresold <= objectA->maxCubeVertex.y &&
            objectB->maxCubeVertex.z - thresold <= objectA->maxCubeVertex.z;
    if(check == true)
        return true;

    // Case 6: Partial collision, i.e. object b min is in between object A min and max
    check = objectB->minCubeVertex.x + thresold >= objectA->minCubeVertex.x &&
            objectB->minCubeVertex.y + thresold >= objectA->minCubeVertex.y &&
            objectB->minCubeVertex.z + thresold >= objectA->minCubeVertex.z &&
            objectB->minCubeVertex.x - thresold <= objectA->maxCubeVertex.x &&
            objectB->minCubeVertex.y - thresold <= objectA->maxCubeVertex.y &&
            objectB->minCubeVertex.z - thresold <= objectA->maxCubeVertex.z;
    if(check == true)
        return true;

    return false;
}