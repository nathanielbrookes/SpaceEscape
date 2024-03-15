#include <vector>
#include "ode/ode.h"

#ifndef PARTICLE_H
#define PARTICLE_H


class particle {
private:
    ofMaterial material;

public:
    particle(glm::vec3 pos, int ID);
    void Draw();
    void SetPosition(glm::vec3 pos);
    glm::vec3 GetPosition();

    glm::vec3 pos;
    bool captured = false;
    int id = 0;
    int assignedTo = -1;
};

#endif // PARTICLE_H
