#ifndef BOUNDINGBOX_H
#define BOUNDINGBOX_H

struct boundingBox{
    glm::vec3 minCubeVertex;
    glm::vec3 maxCubeVertex;
};

bool collisionTest(boundingBox *objectA, boundingBox *objectB);

#endif