#pragma once


#include "AtomWorld.h"
#include "Camera.h"
#include "InputHandler.h"

class BaseLevel {
public:
    AtomWorld& atomWorld;
    Camera& camera;
    InputHandler& inputHandler;
    BaseLevel(AtomWorld& atomWorld, Camera& camera, InputHandler& inputHandler) : atomWorld(atomWorld), camera(camera), inputHandler(inputHandler) {
    }

    virtual void onInit() {
        std::cout << "BaseLevel::onInit() called\n";
    }
};