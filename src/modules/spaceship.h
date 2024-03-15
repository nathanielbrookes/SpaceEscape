#include <vector>
#include "ofApp.h"
#include "ofxAssimpModelLoader.h"

#ifndef SPACESHIP_H
#define SPACESHIP_H


class spaceship
{
public:
    spaceship(glm::vec3 pos);
    void Draw();
    void SetPosition(glm::vec3 pos);
    glm::vec3 GetPosition();

private:
    ofxAssimpModelLoader ship;
    glm::vec3 pos = {0, 0, 0}; // Position
};

#endif // SPACESHIP_H
