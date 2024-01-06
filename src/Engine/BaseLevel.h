#pragma once


#include "AtomWorld.h"
#include "Camera.h"
#include "InputHandler.h"

struct LevelContext {
    AtomWorld& atomWorld;
    Camera& camera;
    InputHandler& inputHandler;

    LevelContext(AtomWorld& atomWorld, Camera& camera, InputHandler& inputHandler) : atomWorld(atomWorld), camera(camera), inputHandler(inputHandler) {}
};

class BaseLevel {
public:
    AtomWorld& atomWorld;
    Camera& camera;
    InputHandler& inputHandler;
    explicit BaseLevel(LevelContext levelContext) : atomWorld(levelContext.atomWorld), camera(levelContext.camera), inputHandler(levelContext.inputHandler) {
    }


    virtual void onInit() {}
};