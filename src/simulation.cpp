 #include "ofApp.h"
#include "ofxAssimpModelLoader.h"
#include "simulation.h"
#include "particle.h"
#include <string>

simulation::simulation() {
    // Load Game Music
    music.load("music.mp3");
    music.setLoop(true);
    music.setVolume(0.2);

    // Load Particle Collect Sound
    collectFx.load("collect.mp3");
    collectFx.setVolume(1);

    // Load Player Fall Sound
    fallFx.load("fall.mp3");
    fallFx.setVolume(0.5);

    ofSetVerticalSync(true);
    ofSetFrameRate(60);
    ofSetWindowShape(1920,1080);
    ofSetWindowPosition(0,0);
    ofSetWindowTitle("Space Escape!");
    ofDisableArbTex();
    ofSetSmoothLighting(true);
    ofSetBoxResolution(16);

    // Initialise ODE
    dInitODE2(0);

    // Create World and Space
    world = dWorldCreate();
    //space = dSimpleSpaceCreate(0);
    space = dHashSpaceCreate(0);

    // Create Contact Group
    contactgroup = dJointGroupCreate(0);
    dWorldSetGravity(world, 0, 50, 0);

    dAllocateODEDataForThread(dAllocateMaskAll);

    // Create Player
    myPlayer = new player(world, space, glm::vec3(0, -100, 0));

    dWorldSetAutoDisableFlag(this->world, 0);

    // Create Light
    light.setSpotlight();
    light.setSpotlightCutOff(50);
    light.setSpotConcentration(15);
    light.setAttenuation(1,0,0);
    light.setSpecularColor(ofColor(255));
    light.setDiffuseColor(ofColor(255));
    light.setAmbientColor(ofColor(255));

    lightSphere.setRadius(10);
    lightSphere.setPosition(light.getPosition());

    glEnable(GL_POLYGON_SMOOTH);
    glShadeModel(GL_SMOOTH);

    // Load fonts
    ofTrueTypeFont::setGlobalDpi(72);
    passionOne20.load("PassionOne-Regular.ttf", 20, true, true);
    passionOne30.load("PassionOne-Regular.ttf", 30, true, true);
    passionOne50.load("PassionOne-Regular.ttf", 50, true, true);
    passionOne100.load("PassionOne-Regular.ttf", 100, true, true);

    // Load 3D Blender Models
    satellite.loadModel("Satellite.dae", true);
    satellite.setPosition(0, 0, 0);
    satellite.setRotation(0, 90.0, 0, 1, 0);
    satellite.setScale(0.2,0.2,0.2);

    planet.loadModel("Jupiter.obj", true);
    planet.setPosition(1000, -150, 1000);
    planet.setRotation(0, 0., 0, 1, 0);
    planet.setScale(1,1,1);

    asteroid.loadModel("Asteroid.obj", true);
    asteroid.setPosition(0, 0, 0);
    asteroid.setRotation(0, 0., 0, 1, 0);
    asteroid.setScale(0.2,0.2,0.2);

    // Play Music
    if (!this->music.isPlaying()) {
        this->music.play();
    }

}

void simulation::Start() {
    this->gamestate = 0;
    this->transitionstate = 0;
    this->freezeInputs = false;
    this->myPlayer->hide = false;
    this->myPlayer->Reset();
    this->cam.Reset();

    this->myLevel = new level(world, space, 1);
    this->myLevel->Start();
}

void simulation::Destroy(){
    dJointGroupDestroy(this->contactgroup);
    dSpaceDestroy(this->space);
    dWorldDestroy(this->world);
    dCloseODE();
}

void simulation::KeyPressed(int key) {
    keyIsDown[key] = true;

    if (key == OF_KEY_TAB) {
        this->showText = !this->showText;
    }
}

void simulation::KeyReleased(int key) {
    keyIsDown[key] = false;

    // Allows the player to start/restart the game
    if (this->gamestate == 2 || this->gamestate == 3) {
        this->Start();
    }
}

void simulation::HandleInputs() {
    if (freezeInputs) return;

    // Level Reset
    if (keyIsDown['q'] == true) {
        if (showDebug) cout << "RESET" << endl;
        myPlayer->Reset();
        myLevel->Reset();
    }

    // Player Move Forward / Backward
    if (keyIsDown['w'] == true) {
        if (showDebug) cout << "Moving Forward" << endl;
        myPlayer->velocity += 0.35;
    }
    else if (keyIsDown['s'] == true) {
        if (showDebug) cout << "Moving Backward" << endl;
        myPlayer->velocity -= 0.35;
    }

    // Player Rotate Left / Right
    if (keyIsDown['a'] == true) {
        if (showDebug) cout << "Rotating Left" << endl;
        myPlayer->rotationVelocity += 0.1;
    }
    else if (keyIsDown['d'] == true) {
        if (showDebug) cout << "Rotating Right" << endl;
        myPlayer->rotationVelocity -= 0.1;
    }

    // Camera Pitch Down / Up
    if (keyIsDown[OF_KEY_LEFT] == true) {
        if (showDebug) cout << "Pitch Down" << endl;
        cam.SetPitch(cam.GetPitch()+1);
    }
    else if (keyIsDown[OF_KEY_RIGHT] == true) {
        if (showDebug) cout << "Pitch Up" << endl;
        cam.SetPitch(cam.GetPitch()-1);
    }

    // Camera Zoom Up / Down
    if (keyIsDown[OF_KEY_UP] == true) {
        if (showDebug) cout << "Zoom In" << endl;
        cam.SetZoomDistance(cam.GetZoomDistance()-5);
    }
    else if (keyIsDown[OF_KEY_DOWN] == true) {
        if (showDebug) cout << "Zoom Out" << endl;
        cam.SetZoomDistance(cam.GetZoomDistance()+5);
    }

    // Spacebar
    if (keyIsDown[32] == true) {
        if (showDebug) cout << "Jump!" << endl;

        if (!myPlayer->isJumping) {
            myPlayer->Jump();
        }
        else {
            myPlayer->isJumping = false;
        }
    }
    else {
        myPlayer->isJumping = false;
    }
}

void simulation::Update() {
    if (this->gamestate == 0) { // Main Gameplay Loop
        this->HandleInputs();

        // Check If Player Is Dead
        glm::vec3 pos = myPlayer->GetPosition();
        if (pos[1] > 100) {
            this->cam.SetPitch(min(this->cam.GetPitch()+1, 89.0f));

            if (!myPlayer->isDying) {
                this->cam.SetPitch(-25);
                myPlayer->isDying = true;
                fallFx.play();
                this->freezeInputs = true;
            }
        }

        if (pos[1] > 1250) {
            myPlayer->Die();
            myLevel->Reset();
            this->freezeInputs = false;
            this->cam.SetPitch(-25);
        }

        // Check If Player Has 'Captured' A Particle
        for (int i = 0; i < myLevel->particles.size(); i++) {
            particle p = myLevel->particles[i];

            if (p.captured) continue;

            glm::vec3 particlePos = p.pos;
            float distance = sqrt(pow(pos[0]-particlePos[0], 2) + pow(pos[1]-particlePos[1], 2) + pow(pos[2]-particlePos[2], 2));

            if (distance < 45) {
                collectFx.play();
                myLevel->particles[i].captured = true;
                myLevel->particlesCaptured += 1;
            }
        }

        // Check If Current Level Is Complete
        if (myLevel->complete) {
            this->freezeInputs = true;
            this->gamestate = 1;
            this->cameraStartTime = ofGetElapsedTimef();
        }

        // Reset Player Jump Flag
        this->myPlayer->jumpAllowed = false;
        this->myPlayer->SetPlatformVelocity({0, 0, 0});

        for (int p = 0; p < this->myLevel->platforms.size(); p++) {
            this->myLevel->platforms[p].detect = false;
        }

        // ODE - Call Collision Detection
        dSpaceCollide(this->space, 0, &nearCallback);

        // ODE - Take a Simulation Step
        dWorldQuickStep(this->world, 0.05);

        myPlayer->Update(); // Update Player Movements


        glm::vec3 playerPos = myPlayer->GetPosition();

        float camHorizontalDistance = cam.CalculateHorizontalDistance();
        float camVerticalDistance = cam.CalculateVerticalDistance();

        // Calculate Camera Position

        float theta = myPlayer->GetOrientation().getEuler().y + cam.GetAngleAroundTarget(); // Player Rotation in Degrees

        float offsetX = camHorizontalDistance * sin(theta * PI/180);
        float offsetZ = camHorizontalDistance * cos(theta * PI/180);

        float camX = playerPos.x - offsetX;
        float camY = playerPos.y + camVerticalDistance;
        float camZ = playerPos.z - offsetZ;

        glm::vec3 cameraPos = {camX, camY, camZ};
        cam.SetPosition(cameraPos);

        // Update Camera Orientation
        cam.LookAt(playerPos);

        // Remove All Contact Joints
        dJointGroupEmpty(this->contactgroup);

        // Update Level
        if (!myLevel->complete) {
            myLevel->Update();
        }
    }
    else if (this->gamestate == 1) { // Level End Transition
        this->cameraEndTime = ofGetElapsedTimef();

        // Hide player
        myPlayer->hide = true;

        // Get Spaceship Location
        glm::vec3 spaceshipPos = myLevel->ship->GetPosition();

        float elapsedTime = this->cameraEndTime-this->cameraStartTime;
        if (elapsedTime < 2) {
            this->transitionstate = 0;
        }
        else if (elapsedTime < 4) {
            this->transitionstate = 1;
        }
        else if (elapsedTime < 6) {
            this->transitionstate = 2;
        }
        else if (elapsedTime < 7) {
            this->transitionstate = 3;
        }
        else if (elapsedTime < 9) {
            this->transitionstate = 4;
        }

        if (this->transitionstate == 0) {
            cam.SetPitch(0);
            cam.SetZoomDistance(800);
            cam.SetAngleAroundTarget(90);

            float camHorizontalDistance = cam.CalculateHorizontalDistance();
            float camVerticalDistance = cam.CalculateVerticalDistance();

            // Calculate Camera Position
            float theta = cam.GetAngleAroundTarget();

            float offsetX = camHorizontalDistance * sin(theta * PI/180);
            float offsetZ = camHorizontalDistance * cos(theta * PI/180);

            float camX = spaceshipPos[0] - offsetX;
            float camZ = spaceshipPos[2] - offsetZ;
            float camY = spaceshipPos[1] + camVerticalDistance;

            glm::vec3 cameraPos = {camX, camY, camZ};
            cam.SetPosition(cameraPos);

        }
        else if (this->transitionstate == 1) {
            float progressTime = elapsedTime-2;

            cam.SetPitch(min(max(-progressTime*10, -89.0f), 0.0f));
            cam.SetZoomDistance(max(800.0f, cam.GetZoomDistance()-10));
            cam.SetAngleAroundTarget(90 + progressTime*15);

            myLevel->ship->SetPosition(glm::vec3(spaceshipPos.x, spaceshipPos.y-10, spaceshipPos.z));

            float camHorizontalDistance = cam.CalculateHorizontalDistance();
            float camVerticalDistance = cam.CalculateVerticalDistance();

            // Calculate Camera Position

            float theta = cam.GetAngleAroundTarget();

            float offsetX = camHorizontalDistance * sin(theta * PI/180);
            float offsetZ = camHorizontalDistance * cos(theta * PI/180);

            float camX = spaceshipPos[0] - offsetX;
            float camZ = spaceshipPos[2] - offsetZ;
            float camY = spaceshipPos[1] + camVerticalDistance;

            glm::vec3 cameraPos = {camX, camY, camZ};
            cam.SetPosition(cameraPos);
        }
        else if (this->transitionstate == 2) {
            myLevel->ship->SetPosition(glm::vec3(spaceshipPos.x, spaceshipPos.y-100, spaceshipPos.z));
        }

        // Update Camera Orientation
        cam.LookAt(glm::vec3(spaceshipPos[0], spaceshipPos[1], spaceshipPos[2]));

        // Advance to Next Level
        if (this->transitionstate == 4) {
            if (myLevel->levelNo < this->levelCount) {
                myLevel = new level(world, space, myLevel->levelNo+1);
                this->gamestate = 0;
                this->transitionstate = 0;
                this->freezeInputs = false;
                this->myPlayer->hide = false;
                this->myPlayer->Reset();
                this->cam.Reset();
                this->myLevel->Start();

                if (showDebug) cout << "END OF LEVEL!" << endl;

            }
            else {
                this->freezeInputs = true;
                this->gamestate = 3;

                if (showDebug) cout << "GAME IS OVER!" << endl;
            }
        }
    }
    else if (this->gamestate == 2) {
        glm::vec3 pos = glm::vec3(0, -150, 100);

        this->cam.SetZoomDistance(300);
        this->cam.SetPitch(-10);

        float camHorizontalDistance = cam.CalculateHorizontalDistance();
        float camVerticalDistance = cam.CalculateVerticalDistance();

        // Calculate Camera Position

        float theta = cam.GetAngleAroundTarget();

        float offsetX = camHorizontalDistance * sin(theta * PI/180);
        float offsetZ = camHorizontalDistance * cos(theta * PI/180);

        float camX = pos.x - offsetX;
        float camY = pos.y + camVerticalDistance;
        float camZ = pos.z - offsetZ;

        glm::vec3 cameraPos = {camX, camY, camZ};
        cam.SetPosition(cameraPos);

        // Update Camera Orientation
        cam.LookAt(pos);
    }
}

void simulation::Draw() {
    glm::vec3 playerPos = myPlayer->GetPosition();

    if (this->gamestate == 0 || this->gamestate == 1) {
        ofBackground(0, 0, 0);

        light.setPosition(playerPos.x, playerPos.y - 500, playerPos.z);
        light.setOrientation(ofVec3f(90, 0, 0));
        light.enable();

        glm::vec3 planetPos = glm::vec3(1000, -150, 1000);
        glm::vec3 asteroidPos = glm::vec3(-500, -150, 3000+ofApp::VaryValue(-10, 10, 1));
        glm::vec3 asteroidPos2 = glm::vec3(1000, -300, 7500+ofApp::VaryValue(-10, 10, 1));
        glm::vec3 satellitePos = glm::vec3(-1000, -150, 2000);
        glm::vec3 satellitePos2 = glm::vec3(1000, -250, 4750);

        // Change position of objects in Scene for different levels
        if (this->myLevel->levelNo == 2) {
            planetPos = glm::vec3(8000, -150, 5000);
            asteroidPos = glm::vec3(-2000, 150, 3000+ofApp::VaryValue(-10, 10, 1));
            asteroidPos2 = glm::vec3(1000, -300, 6500+ofApp::VaryValue(-10, 10, 1));
            satellitePos = glm::vec3(1000, -250, 2000);
            satellitePos2 = glm::vec3(-1250, -350, 3250);
        }
        else if (this->myLevel->levelNo == 3) {
            planetPos = glm::vec3(-100, 4500, 8500);
            asteroidPos = glm::vec3(-3000, 250, 1000+ofApp::VaryValue(-10, 10, 1));
            asteroidPos2 = glm::vec3(2000, 3150, 6000+ofApp::VaryValue(-10, 10, 1));
            satellitePos = glm::vec3(1500, -50, 2200);
            satellitePos2 = glm::vec3(-1450, -350, 3250);
        }

        // Create Scenary Lights
        ofLight sceneLight1;
        sceneLight1.setSpotlight();
        sceneLight1.setSpotlightCutOff(50);
        sceneLight1.setSpotConcentration(15);
        sceneLight1.setAttenuation(1,0,0);
        sceneLight1.setSpecularColor(ofColor(255));
        sceneLight1.setDiffuseColor(ofColor(255));
        sceneLight1.setAmbientColor(ofColor(255));
        sceneLight1.setPosition(0, -50, 0);
        sceneLight1.lookAt(planetPos);
        sceneLight1.enable();

        ofLight sceneLight2;
        sceneLight2.setSpotlight();
        sceneLight2.setSpotlightCutOff(50);
        sceneLight2.setSpotConcentration(15);
        sceneLight2.setAttenuation(1,0,0);
        sceneLight2.setSpecularColor(ofColor(255));
        sceneLight2.setDiffuseColor(ofColor(255));
        sceneLight2.setAmbientColor(ofColor(255));
        sceneLight2.setPosition(0, asteroidPos.y-100, asteroidPos.z-100);
        sceneLight2.lookAt(asteroidPos);
        sceneLight2.enable();

        ofLight sceneLight3;
        sceneLight3.setSpotlight();
        sceneLight3.setSpotlightCutOff(50);
        sceneLight3.setSpotConcentration(15);
        sceneLight3.setAttenuation(1,0,0);
        sceneLight3.setSpecularColor(ofColor(255));
        sceneLight3.setDiffuseColor(ofColor(255));
        sceneLight3.setAmbientColor(ofColor(255));
        sceneLight3.setPosition(0, asteroidPos2.y-100, asteroidPos2.z-100);
        sceneLight3.lookAt(asteroidPos2);
        sceneLight3.enable();

        ofLight sceneLight4;
        sceneLight4.setSpotlight();
        sceneLight4.setSpotlightCutOff(50);
        sceneLight4.setSpotConcentration(15);
        sceneLight4.setAttenuation(1,0,0);
        sceneLight4.setSpecularColor(ofColor(255));
        sceneLight4.setDiffuseColor(ofColor(255));
        sceneLight4.setAmbientColor(ofColor(255));
        sceneLight4.setPosition(0, satellitePos.y-100, satellitePos.z-100);
        sceneLight4.lookAt(satellitePos);
        sceneLight4.enable();

        ofLight sceneLight5;
        sceneLight5.setSpotlight();
        sceneLight5.setSpotlightCutOff(50);
        sceneLight5.setSpotConcentration(15);
        sceneLight5.setAttenuation(1,0,0);
        sceneLight5.setSpecularColor(ofColor(255));
        sceneLight5.setDiffuseColor(ofColor(255));
        sceneLight5.setAmbientColor(ofColor(255));
        sceneLight5.setPosition(0, satellitePos2.y-100, satellitePos2.z-100);
        sceneLight5.lookAt(satellitePos2);
        sceneLight5.enable();

        ofEnableDepthTest();
        ofEnableLighting();
        cam.Start();

        planetRotateAngle += 0.1;

        // Draw 3D Scenary - Planets:
        planet.setPosition(planetPos.x, planetPos.y, planetPos.z);
        planet.setRotation(0, planetRotateAngle, 0, 1, 0);
        ofPushMatrix();
        ofTranslate(planetPos.x, planetPos.y, planetPos.z);
        ofRotate(planetRotateAngle, 1, 0, 0);
        ofTranslate(-planetPos.x, -planetPos.y, -planetPos.z);
        planet.drawFaces();
        ofPopMatrix();

        // Draw 3D Scenary - Asteroids:
        asteroid.setPosition(asteroidPos.x, asteroidPos.y, asteroidPos.z);
        asteroid.setRotation(0, planetRotateAngle, 0, 1, 0);
        ofPushMatrix();
        ofTranslate(asteroidPos.x, asteroidPos.y, asteroidPos.z);
        ofRotate(planetRotateAngle, 1, 0, 0);
        ofTranslate(-asteroidPos.x, -asteroidPos.y, -asteroidPos.z);
        asteroid.drawFaces();
        ofPopMatrix();

        asteroid.setPosition(asteroidPos2.x, asteroidPos2.y, asteroidPos2.z);
        asteroid.setRotation(0, planetRotateAngle, 0, 1, 0);
        ofPushMatrix();
        ofTranslate(asteroidPos2.x, asteroidPos2.y, asteroidPos2.z);
        ofRotate(planetRotateAngle, 1, 0, 0);
        ofTranslate(-asteroidPos2.x, -asteroidPos2.y, -asteroidPos2.z);
        asteroid.drawFaces();
        ofPopMatrix();

        // Draw 3D Scenary - Satellites:
        satellite.setPosition(satellitePos.x, satellitePos.y, satellitePos.z);
        satellite.setRotation(0, planetRotateAngle, 0, 1, 0);
        ofPushMatrix();
        ofTranslate(satellitePos.x, satellitePos.y, satellitePos.z);
        ofRotate(planetRotateAngle, 1, 0, 0);
        ofTranslate(-satellitePos.x, -satellitePos.y, -satellitePos.z);
        satellite.drawFaces();
        ofPopMatrix();

        satellite.setPosition(satellitePos2.x, satellitePos2.y, satellitePos2.z);
        satellite.setRotation(0, planetRotateAngle, 0, 1, 0);
        ofPushMatrix();
        ofTranslate(satellitePos2.x, satellitePos2.y, satellitePos2.z);
        ofRotate(planetRotateAngle, 1, 0, 0);
        ofTranslate(-satellitePos2.x, -satellitePos2.y, -satellitePos2.z);
        satellite.drawFaces();
        ofPopMatrix();

        // Draw Player
        myPlayer->Draw();

        // Draw Level
        myLevel->Draw();

        // Draw some cubes
        ofSetColor(0, 0, 255);
        for (int i = 0; i < 25; i++) {
            ofDrawBox(sin(i*50)*100, cos(i*50)*100, 0, 10);
        }

        // Draw Light Sphere
        if (this->showText) {
            ofSetColor(0, 255, 0);
            lightSphere.setPosition(light.getPosition());
            lightSphere.draw();
        }

        light.disable();
        sceneLight1.disable();
        sceneLight2.disable();
        sceneLight3.disable();
        sceneLight4.disable();
        sceneLight5.disable();
        ofDisableLighting();

        // Draw Space Nebula
        ofMesh nebulaMesh;
        nebulaMesh.setMode(OF_PRIMITIVE_POINTS);
        float t = ofGetElapsedTimef();
        for (int i = 0; i < 1000; i++) {

            float radius = 5000;

            float newX = ofNoise(i * 0.1, t * 0.001)*2 - 1;
            float newY = ofNoise(i * 0.1, t * 0.001 + 1000)*2 - 1;
            float newZ = ofNoise(i * 0.1, t * 0.001 + 2000)*2 - 1;

            float x = newX * (1/sqrt(newX*newX + newY*newY + newZ*newZ)) * radius + playerPos.x;
            float y = newY * (1/sqrt(newX*newX + newY*newY + newZ*newZ)) * radius + playerPos.y;
            float z = newZ * (1/sqrt(newX*newX + newY*newY + newZ*newZ)) * radius + playerPos.z;

            float rValue = ofNoise(x * 0.001, y * 0.001, z * 0.001);
            float gValue = ofNoise((x+1000) * 0.001, y * 0.001, z * 0.001);
            float bValue = ofNoise((x+2000) * 0.001, y * 0.001, z * 0.001);

            nebulaMesh.addVertex(ofPoint(x, y, z));
            nebulaMesh.addColor(ofColor(rValue*255, gValue*255, bValue*255));
        }
        glEnable(GL_POINT_SMOOTH);
        glPointSize(3);
        nebulaMesh.drawVertices();

        cam.End();
        ofDisableDepthTest();

        // Draw On-Screen Text

        // Level Title
        string levelText = "LEVEL " + to_string(myLevel->levelNo);

        float padding = 50;
        ofRectangle bounds = passionOne50.getStringBoundingBox(levelText, 0, 0);
        ofSetColor(20, 20, 20, 200);
        ofDrawRectangle(0, 0, ofGetScreenWidth(), bounds.height + padding + 50);
        ofSetColor(255);
        passionOne50.drawString(levelText, (ofGetScreenWidth()-bounds.width)/2, bounds.height + padding/2);

        string text = "Time Elapsed: " + to_string((int)((myLevel->endTime-myLevel->startTime)/1000)) + " s";
        bounds = passionOne20.getStringBoundingBox(text, 0, 0);
        passionOne20.drawString(text, (ofGetScreenWidth()-bounds.width)/2, 100);

        text = "Orbs Captured: " + to_string(myLevel->particlesCaptured) + " / " + to_string(myLevel->particles.size());
        bounds = passionOne20.getStringBoundingBox(text, 0, 0);
        passionOne20.drawString(text, (ofGetScreenWidth()-bounds.width)/2, 125);

        // Display Fail Text
        if (myPlayer->isDying) {
            string text = "OH NO!";
            bounds = passionOne100.getStringBoundingBox(text, 0, 0);
            ofSetColor(255);
            passionOne100.drawString(text, (ofGetScreenWidth()-bounds.width)/2, 350);
        }
    }
    else if (this->gamestate == 2) {
        // Display Start Screen
        ofBackground(0, 0, 0);

        planetRotateAngle += 0.1;

        glm::vec3 satellitePos = glm::vec3(0, -100, 100);

        // Create Scenery Light
        ofLight sceneLight;
        sceneLight.setSpotlight();
        sceneLight.setSpotlightCutOff(50);
        sceneLight.setSpotConcentration(15);
        sceneLight.setAttenuation(1,0,0);
        sceneLight.setSpecularColor(ofColor(255));
        sceneLight.setDiffuseColor(ofColor(255));
        sceneLight.setAmbientColor(ofColor(255));
        sceneLight.setPosition(0, satellitePos.y-100, satellitePos.z-300);
        sceneLight.lookAt(satellitePos);
        sceneLight.enable();

        ofEnableDepthTest();
        ofEnableLighting();
        cam.Start();

        // Draw 3D Scenery - Satellitte:
        satellite.setPosition(satellitePos.x, satellitePos.y, satellitePos.z);
        satellite.setRotation(0, planetRotateAngle, 0, 1, 0);
        ofPushMatrix();
        ofTranslate(satellitePos.x, satellitePos.y, satellitePos.z);
        ofRotate(planetRotateAngle, 1, 0, 0);
        ofTranslate(-satellitePos.x, -satellitePos.y, -satellitePos.z);
        satellite.drawFaces();
        ofPopMatrix();

        sceneLight.disable();
        ofDisableLighting();
        cam.End();
        ofDisableDepthTest();

        string text = "Space Escape!";
        ofRectangle bounds = passionOne100.getStringBoundingBox(text, 0, 0);
        ofSetColor(255);
        passionOne100.drawString(text, (ofGetScreenWidth()-bounds.width)/2, 350);

        text = "Press Any Key To Start";
        bounds = passionOne50.getStringBoundingBox(text, 0, 0);
        passionOne50.drawString(text, (ofGetScreenWidth()-bounds.width)/2, 450);
    }
    else if (this->gamestate == 3) {
        // Display Start Screen
        ofBackground(0, 0, 0);

        string text = "Space Escape!";
        ofRectangle bounds = passionOne100.getStringBoundingBox(text, 0, 0);
        ofSetColor(255);
        passionOne100.drawString(text, (ofGetScreenWidth()-bounds.width)/2, 350);

        text = "Press Any Key To Play Again";
        bounds = passionOne50.getStringBoundingBox(text, 0, 0);
        passionOne50.drawString(text, (ofGetScreenWidth()-bounds.width)/2, 450);
    }

    if (this->gamestate == 1) {
        // Fading Effects
        if (this->transitionstate == 3) {
            float elapsedTime = (this->cameraEndTime-this->cameraStartTime)-6;
            float fadeAmount = min(elapsedTime*255, 255.0f);

            ofSetColor(0, 0, 0, fadeAmount);
            ofDrawRectangle(0, 0, ofGetWidth(), ofGetHeight());
        }
        else if (this->transitionstate == 4) {
            float elapsedTime = (this->cameraEndTime-this->cameraStartTime)-7;
            float fadeAmount = max(255-(elapsedTime*255), 0.0f);

            ofSetColor(0, 0, 0, fadeAmount);
            ofDrawRectangle(0, 0, ofGetWidth(), ofGetHeight());
        }

        // Cinematic Bar Effects
        if (this->transitionstate < 4) {
            float transitionTime = (this->cameraEndTime-this->cameraStartTime);
            float distance = min(transitionTime*100, 200.0f);

            ofSetColor(0);
            ofDrawRectangle(0, 0, ofGetWidth(), distance);
            ofDrawRectangle(0, ofGetHeight()-distance, ofGetWidth(), 200);

            // Display Success Text
            string text = "WELL DONE!";
            ofRectangle bounds = passionOne100.getStringBoundingBox(text, 0, 0);
            ofSetColor(255);
            passionOne100.drawString(text, (ofGetScreenWidth()-bounds.width)/2, 350);

            text = "LEVEL " + to_string(myLevel->levelNo) + " COMPLETE";
            bounds = passionOne50.getStringBoundingBox(text, 0, 0);
            passionOne50.drawString(text, (ofGetScreenWidth()-bounds.width)/2, 400);

            text = "Time Taken: " + to_string((int)((myLevel->endTime-myLevel->startTime)/1000)) + " s";
            bounds = passionOne30.getStringBoundingBox(text, 0, 0);
            passionOne30.drawString(text, (ofGetScreenWidth()-bounds.width)/2, 750);

            text = "Orbs Captured: " + to_string(myLevel->particlesCaptured) + " / " + to_string(myLevel->particles.size());
            bounds = passionOne30.getStringBoundingBox(text, 0, 0);
            passionOne30.drawString(text, (ofGetScreenWidth()-bounds.width)/2, 775);

        }
        else if (this->transitionstate == 4) {
            float transitionTime = (this->cameraEndTime-this->cameraStartTime)-7;
            float distance = max(200-(transitionTime*100), 0.0f);

            ofSetColor(0);
            ofDrawRectangle(0, 0, ofGetWidth(), distance);
            ofDrawRectangle(0, ofGetHeight()-distance, ofGetWidth(), 200);
        }
    }

    // Debug Text:
    if (this->showText) {
        ofSetColor(255);
        passionOne20.drawString("FPS: " + to_string(ofGetFrameRate()), 30, 100);
        passionOne20.drawString("Player Pos [x,y,z]: [" + to_string(playerPos.x) + ", " + to_string(playerPos.y) + ", " + to_string(playerPos.z) + "]", 30, 150);
        passionOne20.drawString("Player Rotation: " + to_string(myPlayer->rotation), 30, 200);
        passionOne20.drawString("Player On Ground: " + to_string(myPlayer->jumpAllowed), 30, 250);
        passionOne20.drawString("Pitch Angle: " + to_string(cam.GetPitch()), 30, 350);
        passionOne20.drawString("Angle Around Target: " + to_string(cam.GetAngleAroundTarget()), 30, 400);
        passionOne20.drawString("Zoom Distance: " + to_string(cam.GetZoomDistance()), 30, 450);
    }
}

// Collision Code
void simulation::Collide(dGeomID o1, dGeomID o2) {
    // Temporary index for each contact
    int i;
    int MAX_CONTACTS = 10;

    // Get the dynamics body for each geom
    dBodyID b1 = dGeomGetBody(o1);
    dBodyID b2 = dGeomGetBody(o2);

    // Create an array of dContact objects to hold the contact joints
    dContact contact[MAX_CONTACTS];

    // Now we set the joint properties of each contact. Going into the full details here would require a tutorial of its
    // own. I'll just say that the members of the dContact structure control the joint behaviour, such as friction,
    // velocity and bounciness. See section 7.3.7 of the ODE manual and have fun experimenting to learn more.

    for (i = 0; i < MAX_CONTACTS; i++) {
        contact[i].surface.mode = dContactBounce | dContactSoftCFM;
        contact[i].surface.mu = 0;
        contact[i].surface.mu2 = 0;

        contact[i].surface.bounce = 0.0;
        contact[i].surface.bounce_vel = 0.5;

        contact[i].surface.slip1 = 0.1;
        contact[i].surface.slip2 = 0.1;
        contact[i].surface.soft_erp = 0.1;
        contact[i].surface.soft_cfm = 1e-4;
    }

    // Here we do the actual collision test by calling dCollide. It returns the number of actual contact points or zero
    // if there were none. As well as the geom IDs, max number of contacts we also pass the address of a dContactGeom
    // as the fourth parameter. dContactGeom is a substructure of a dContact object so we simply pass the address of
    // the first dContactGeom from our array of dContact objects and then pass the offset to the next dContactGeom
    // as the fifth paramater, which is the size of a dContact structure. That made sense didn't it?

    if (int numc = dCollide(o1, o2, MAX_CONTACTS, &contact[0].geom, sizeof(dContact))) {
        // To add each contact point found to our joint group we call dJointCreateContact which is just one of the many
        // different joint types available.

        if (o1 == this->myPlayer->box || o2 == this->myPlayer->box) {

            dGeomID platformID = o2;
            int platformIndex = this->myLevel->GetPlatformIndex(platformID);

            if (platformIndex != -1) {
                // Collision With Platform
                this->myPlayer->jumpAllowed = true;
                this->myLevel->platforms[platformIndex].detect = true;
                this->myLevel->platforms[platformIndex].Collide();

                // Ensure Player Moves With Moving Platforms
                glm::vec3 platformVel = this->myLevel->platforms[platformIndex].GetVelocity();
                this->myPlayer->SetPlatformVelocity(platformVel);
            }
            else {
                return;
            }
        }

        for (i = 0; i < numc; i++) {
            // dJointCreateContact needs to know which world and joint group to work with as well as the dContact
            // object itself. It returns a new dJointID which we then use with dJointAttach to finally create the
            // temporary contact joint between the two geom bodies.
            dJointID c = dJointCreateContact(this->world, this->contactgroup, contact + i);
            dJointAttach(c, b1, b2);
        }
    }
}

static void nearCallback(void *data, dGeomID o1, dGeomID o2) {
    myApp->game->Collide(o1, o2);
}
