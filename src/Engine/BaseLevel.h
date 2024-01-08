#pragma once


#include "Scene.h"
#include "Camera.h"
#include "InputHandler.h"

struct LevelContext {
    Scene& scene;
    InputHandler& inputHandler;

    LevelContext(Scene& scene, InputHandler& inputHandler) : scene(scene), inputHandler(inputHandler) {}
};

class BaseLevel {
public:
    Scene& scene;
    InputHandler& inputHandler;
    explicit BaseLevel(LevelContext levelContext) : scene(levelContext.scene), inputHandler(levelContext.inputHandler) {
    }


    virtual void onInit() {}
};