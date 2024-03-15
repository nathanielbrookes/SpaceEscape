#include "ofMain.h"
#include "ode/ode.h"
#include "level.h"
#include "platform.h"
#include "particle.h"
#include "spaceship.h"

level::level(dWorldID worldID, dSpaceID spaceID, int levelNo) {
    this->world = worldID;
    this->space = spaceID;
    this->levelNo = levelNo;

    this->GenerateLevel();
}

void level::GenerateLevel() {
    // Create Platforms using ODE boxes:

    if (this->levelNo == 1) {
        // Simple Static Platforms
        platform *p1 = new platform(this->space, glm::vec3(-150, 0, -50), 300, 1500, 0);
        this->platforms.push_back(*p1);

        platform *p2 = new platform(this->space, glm::vec3(-150, 0, 1550), 300, 300, 1);
        this->platforms.push_back(*p2);

        platform *p3 = new platform(this->space, glm::vec3(-150, 0, 1950), 300, 1500, 2);
        this->platforms.push_back(*p3);

        // Moving Platforms
        for (int i = 0; i < 10; i++) {
            float x = -150;
            float y = -45 - (i*60);
            float z = 3550 + (i*500);

            platform *p = new platform(this->space, glm::vec3(x, y, z), 300, 300, i);
            p->type = platform::MOVING_HORIZONTAL;
            this->platforms.push_back(*p);
        }

        // End Level Platform
        platform *endLevel = new platform(this->space, glm::vec3(-300, -645, 8400), 600, 300, 3);
        endLevel->goal = true;
        this->platforms.push_back(*endLevel);

        // Add Spaceship
        this->ship = new spaceship(glm::vec3(-300, -645, 8450));

        // Add Collectable Particles:
        particle *p = new particle(glm::vec3(0, 0, 1700), 0);
        this->particles.push_back(*p);

        for (int i = 0; i < 3; i++) {
            particle *pa1 = new particle(glm::vec3(0, 0, 0), 0);
            pa1->assignedTo = floor(ofRandom(3, 13));
            this->particles.push_back(*pa1);
        }

    }
    else if (this->levelNo == 2) {
        // Static Starting Platform
        platform *p1 = new platform(this->space, glm::vec3(-150, 0, -50), 300, 300, 0);
        this->platforms.push_back(*p1);

        // Moving Platforms
        for (int j = 0; j < 3; j++) {
            for (int i = 0; i < 5; i++) {
                float x = -150 + (400*j) - 400;
                float y = -(i*60);
                float z = 350 + (i*500);

                platform *p = new platform(this->space, glm::vec3(x, y, z), 300, 300, i);
                p->type = platform::MOVING_HORIZONTAL;
                this->platforms.push_back(*p);
            }
        }

        float x = -150;
        float y = -300;
        float z = 2850;

        // Rotating Platforms
        platform *p2 = new platform(this->space, glm::vec3(-150, -300, 2850), 150, 300, 0);
        p2->type = platform::ROTATING;
        ofQuaternion quat;
        quat.makeRotate(45.0, ofVec3f(0, 1, 0));
        p2->SetOrientation(quat);
        p2->SetRotationVelocity(glm::vec3(0, 15, 0));
        this->platforms.push_back(*p2);

        platform *p3 = new platform(this->space, glm::vec3(-450, -300, 3150), 150, 300, 0);
        p3->type = platform::ROTATING;
        ofQuaternion quat2;
        quat2.makeRotate(45.0, ofVec3f(0, 1, 0));
        p3->SetOrientation(quat2);
        p3->SetRotationVelocity(glm::vec3(0, -15, 0));
        this->platforms.push_back(*p3);

        // End Level Platform
        platform *endLevel = new platform(this->space, glm::vec3(-500, -300, 3550), 600, 300, 0);
        endLevel->goal = true;
        this->platforms.push_back(*endLevel);

        // Add Spaceship
        this->ship = new spaceship(glm::vec3(-500, -300, 3550));

        // Add Collectable Particles:
        for (int i = 0; i < 3; i++) {
            particle *pa1 = new particle(glm::vec3(0, 0, 0), 0);
            pa1->assignedTo = floor(ofRandom(1, 15));
            this->particles.push_back(*pa1);
        }

        ofPoint p3Centre = p3->GetCentre();
        particle *pa2 = new particle(glm::vec3(p3Centre.x, p3Centre.y, p3Centre.z), 0);
        this->particles.push_back(*pa2);
    }
    else if (this->levelNo == 3) {
        // Static Starting Platform
        platform *p1 = new platform(this->space, glm::vec3(-150, 0, -50), 300, 300, 0);
        this->platforms.push_back(*p1);

        // Moving & Disappearing Platforms
        for (int j = 0; j < 3; j++) {
            for (int i = 0; i < 5; i++) {
                float x = -150 + (400*j) - 400;
                float y = -((i*60)+60);
                float z = 350 + (i*500);

                platform *p = new platform(this->space, glm::vec3(x, y, z), 300, 300, i);
                p->type = platform::MOVING_VERTICAL;
                p->disappearing = true;
                this->platforms.push_back(*p);
            }
        }

        // End Level Platform
        platform *endLevel = new platform(this->space, glm::vec3(-500, -300, 2750), 600, 300, 0);
        endLevel->goal = true;
        this->platforms.push_back(*endLevel);

        // Add Spaceship
        this->ship = new spaceship(glm::vec3(-500, -300, 2800));

        // Add Collectable Particles:
        for (int i = 0; i <= 4; i++) {
            particle *pa1 = new particle(glm::vec3(0, 0, 0), 0);
            pa1->assignedTo = floor(ofRandom(1, 15));
            this->particles.push_back(*pa1);
        }
    }
}

void level::Draw() {
    // Draw Platforms
    for (int i = 0; i < this->platforms.size(); i++) {
        platform p = this->platforms[i];
        p.Draw();
    }

    // Draw Particles
    for (int i = 0; i < this->particles.size(); i++) {
        particle p = this->particles[i];
        p.Draw();
    }

    // Draw Spaceship
    this->ship->Draw();
}

void level::Update() {
    // Update Level Timer
    this->endTime = ofGetElapsedTimeMillis();

    // Update Platforms
    for (platform& p : this->platforms) {
        p.Update();

        if (!this->complete && (p.goal == true && p.visited == true && p.collisionCount == 1)) {
            this->Finish();
            p.goal = false;
        }
    }

    // Update Particles
    for (particle& p : this->particles) {
        if (p.assignedTo != -1) {
            ofPoint platformCentre = this->platforms[p.assignedTo].GetCentre();
            p.SetPosition(platformCentre);
        }
    }
}

void level::Start() {
    this->startTime = ofGetElapsedTimeMillis();
}

void level::Finish() {
    this->complete = true;
}

void level::Reset() {
    this->startTime = ofGetElapsedTimeMillis();

    // Reset Particles
    this->particlesCaptured = 0;
    for (int i = 0; i < this->particles.size(); i++) {
        this->particles[i].captured = false;
    }
}

int level::GetPlatformIndex(dGeomID ID) {
    for (int i = 0; i < this->platforms.size(); i++) {
        platform p = this->platforms[i];

        if (p.box == ID) {
            return i;
        }
    }

    return -1;
}
