#pragma once


#include "Scene.h"
#include "Camera.h"
#include "InputHandler.h"

struct LevelContext {
    Scene& scene;
    Camera& camera;
    InputHandler& inputHandler;

    LevelContext(Scene& scene, Camera& camera, InputHandler& inputHandler) : scene(scene), camera(camera), inputHandler(inputHandler) {}
};

class BaseLevel {
public:
    Scene& scene;
    Camera& camera;
    InputHandler& inputHandler;
    explicit BaseLevel(LevelContext levelContext) : scene(levelContext.scene), camera(levelContext.camera), inputHandler(levelContext.inputHandler) {
    }


    virtual void onInit() {}
};