#pragma once

#include "../Engine/Scene.h"
#include "../Engine/EngineConsts.h"
#include "../Engine/utils/Rand.h"
#include "../Objects/EmptyObject.h"

class RandomSpawner {
private:
    Scene &m_scene;
    RNGf &m_gen;
public:
    RandomSpawner(Scene &scene, RNGf &gen) : m_scene(scene), m_gen(gen) {}

    void spawn(int number) {
        for (int i = 0; i < number; i++) {
            auto ptr = m_scene.addObject(

                    EmptyObject(),
                    Vector2F::cart
                            (
                                    m_gen.getInRange
                                            (
                                                    0,
                                                    m_scene.getSizeF().m_x
                                            ),
                                    m_gen.getInRange
                                            (0, m_scene.getSizeF().m_y
                                            )
                            )
            );

            BaseObject &object = *ptr.lock();
            object.m_basicDetails->m_color = sf::Color(
                    static_cast<int>(object.m_basicDetails->m_posCurr.m_x / m_scene.getSizeF().m_x * 255),
                    static_cast<int>(object.m_basicDetails->m_posCurr.m_y / m_scene.getSizeF().m_y * 255), 255);
        }
    }
};