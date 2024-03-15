#include "player.h"
#include "ode/ode.h"
#include "ofxAssimpModelLoader.h"

player::player(dWorldID worldID, dSpaceID spaceID, glm::vec3 pos) {
    this->world = worldID;
    this->space = spaceID;
    this->initPos = pos;

    // Add player to world (as a box)
    this->width = 0.3;
    this->height = 0.3;
    this->depth = 0.3;

    this->body = dBodyCreate(this->world);
    this->SetPosition(pos);

    dMass m;
    dMassSetBox(&m, 1, this->width, this->height, this->depth);
    dMassAdjust (&m, 0.5);

    this->box = dCreateBox(this->space, this->width, this->height, this->depth);
    dGeomSetBody(this->box, this->body);
    dBodySetMass (this->body, &m);

    // Initialise Player Particle Effect
    this->particlesMesh.setMode(OF_PRIMITIVE_POINTS);

    // Load Jump Sound Effect
    this->jumpFx.load("jump.mp3");
    this->jumpFx.setVolume(0.3);

    this->model.loadModel("Robot.obj", true);
    this->model.setScale(0.06, 0.06, 0.06);
    this->model.setPosition(0, -5, 0);
    this->model.setRotation(0, 180, 0, 1, 0);
}

void player::Jump() {
    if (!this->isJumping && this->jumpAllowed) {
        this->isJumping = true;

        const dReal *vel = dBodyGetLinearVel(this->body);

        double force = -1000.0;
        dBodyAddForce(this->body, 0.0, force, 0.0);

        this->jumpFx.play();
    }
}

void player::Reset() {
    this->isDying = false;

    this->rotation = 0;
    this->rotationVelocity = 0;

    ofQuaternion playerOrientation;
    playerOrientation.makeRotate(rotation, {0, 1, 0});
    this->SetOrientation(playerOrientation);

    this->SetPosition(this->initPos);
}

void player::Die() {
    this->isDead = true;
    this->Reset();
}

void player::SetPosition(glm::vec3 pos) {
    dBodySetPosition(this->body, pos.x, pos.y, pos.z);
    dBodySetLinearVel(this->body, 0, 0, 0);
}

glm::vec3 player::GetPosition(){
    const dReal *pos = dBodyGetPosition(this->body);

    // {x, y, z}
    return glm::vec3(pos[0], pos[1], pos[2]);
}

void player::SetOrientation(ofQuaternion quat){
    dQuaternion q = {quat.w(), quat.x(), quat.y(), quat.z()};
    dBodySetQuaternion(this->body, q);
}

ofQuaternion player::GetOrientation(){
    const dReal *orient = dBodyGetQuaternion(this->body);

    // {i, j, k, l}
    return ofQuaternion(orient[1], orient[2], orient[3], orient[0]);
}

void player::SetPlatformVelocity(glm::vec3 vel) {
    this->platformVelocity = vel;
}

glm::vec3 player::GetPlatformVelocity() {
    return this->platformVelocity;
}

void player::Draw() {
    if (this->hide) return;

    glm::vec3 playerPos = this->GetPosition();
    ofQuaternion playerOrient = this->GetOrientation();

    ofBoxPrimitive b;

    // scale it to be unit w, h, d * the actual size:
    b.setScale(this->width, this->height, this->depth);

    // Simply set the orientation based on ODE's quaternion. Since we are using glm::quat
    // this time, the ordering is the same as ODE:
    b.setGlobalOrientation(glm::quat(playerOrient[0],playerOrient[1],playerOrient[2],playerOrient[3]));

    // Now set the box's position according to ODE physics:
    b.setGlobalPosition(glm::vec3(playerPos.x, playerPos.y-15, playerPos.z));

    // Draw it:
    ofPushMatrix();
    ofTranslate(playerPos.x, playerPos.y, playerPos.z);
    ofRotateRad(2 * glm::acos(playerOrient.w()), playerOrient.x(), playerOrient.y(), playerOrient.z());

    this->model.drawFaces();

    ofPopMatrix();

    ofSetColor(255, 255, 255);
    //b.draw();

    // Draw Player Particle Effect
    this->particlesMesh.addVertex(ofPoint(playerPos.x, playerPos.y, playerPos.z));
    this->particlesMesh.addColor(ofColor(255, 255, 255));

    glEnable(GL_POINT_SMOOTH); // use circular points instead of square points
    glPointSize(5); // make the points bigger

    this->particlesMesh.drawVertices();

    // Limit number of vertices to 50
    if (this->particlesMesh.getNumVertices() > 50) {
        this->particlesMesh.removeVertex(0);
        this->particlesMesh.removeColor(0);
    }

}

void player::Update() {
    glm::vec3 pos = this->GetPosition();

    // Lock Rotation
    dMatrix3 R;
    dRFromAxisAndAngle(R, 0,0,0,0);
    dBodySetRotation(this->body, R);

    rotation += this->rotationVelocity;

    // Update Rotation
    ofQuaternion playerOrientation = this->GetOrientation();
    playerOrientation.makeRotate(rotation, {0, 1, 0});
    this->SetOrientation(playerOrientation);

    //orientation.makeRotate(rotation, {0, 1, 0});
    //this->setRotation(orientation);

    // Calculate DX and DZ based on rotation angle
    float dx = this->velocity * sin(rotation * PI/180);
    float dz = this->velocity * cos(rotation * PI/180);

    const dReal *vel = dBodyGetLinearVel(this->body);
    float dy = vel[1] / 100;

    // Add platform velocity
    glm::vec3 platformVel = this->GetPlatformVelocity();

    // Add velocity to position
    glm::vec3 newPosition = {pos.x, pos.y, pos.z};
    newPosition.x += dx + platformVel.x;
    newPosition.y += dy + platformVel.y;
    newPosition.z += dz + platformVel.z;

    // Update position
    if (this->isDead) {
        this->Die();
        this->isDead = false;
    }
    else {
        dBodySetPosition(this->body, newPosition.x, newPosition.y, newPosition.z);
        dBodySetLinearVel(this->body, 0, vel[1], 0);
    }

    // Add Friction
    this->rotation *= 0.90;
    this->velocity *= 0.95;
}
