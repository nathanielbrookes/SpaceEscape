#pragma once

#include "ofMain.h"
#include "ofxAssimpModelLoader.h"
#include "ode/ode.h"
#include "modules/camera.h"
#include "modules/player.h"
#include "modules/level.h"
#include "particle.h"

//#include "ofxGui.h"

#ifndef SIMULATION_H
#define SIMULATION_H

class simulation {
private:
    int gamestate = 2; // 0 = Main Gameplay; 1 = Camera Transition; 2 = Start Screen; 3 = End Screen
    float cameraStartTime = 0;
    float cameraEndTime = 0;
    int transitionstate = 0;
    bool keyIsDown[65536];
    bool freezeInputs = true;
    bool showText = false;
    bool showDebug = false;

    ofSoundPlayer music;
    ofSoundPlayer collectFx;
    ofSoundPlayer fallFx;

    Camera cam;
    player* myPlayer;

    dWorldID world;
    dSpaceID space;
    dJointGroupID contactgroup;

    level* myLevel;
    int levelCount = 3;

    ofLight light;
    ofSpherePrimitive lightSphere;

    ofTrueTypeFont passionOne20;
    ofTrueTypeFont passionOne30;
    ofTrueTypeFont passionOne50;
    ofTrueTypeFont passionOne100;

    ofxAssimpModelLoader planet;
    ofxAssimpModelLoader asteroid;
    ofxAssimpModelLoader satellite;
    float planetRotateAngle = 0;

public:
    simulation();
    void Start();
    void Draw();
    void Update();
    void Destroy();
    void HandleInputs();
    void KeyPressed(int key);
    void KeyReleased(int key);
    void Collide(dGeomID o1, dGeomID o2);
};

#endif
