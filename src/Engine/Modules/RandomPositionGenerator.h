#pragma once

#include "../Scene/Scene.h"
#include "../utils/Rand.h"

class RandomPositionGenerator {
private:
    Scene &m_scene;
    RNGf& m_gen;
public:
    RandomPositionGenerator(Scene &scene, RNGf& gen) : m_scene(scene), m_gen(gen) {}

    Vector2F get() {
        return Vector2F::cart(m_gen.getInRange(0, m_scene.getSizeF().m_x), m_gen.getInRange(0, m_scene.getSizeF().m_y));
    }
};