#include "Bullet.h"
#include "Player.h"

//
// Created by ronik on 29/01/2024.
//

void Bullet::v_onInit() {}

void Bullet::v_onCollision(BaseObject *ptr) {
    auto playerPtr = dynamic_cast<Player *>(ptr);
    auto bulletPtr = dynamic_cast<Bullet *>(ptr);
    if (playerPtr) {
        return;
    }
    if (bulletPtr) {
        return;
    }
    m_scene.removeObject(ptr);
    m_hitsLeft--;
    if (m_hitsLeft <= 0) {
        m_scene.removeObject(this);
    }
}

void Bullet::v_onTick() {
    m_ticksToLive--;
    if (m_ticksToLive <= 0) {
        m_scene.removeObject(this);
    }
}

Bullet::Bullet(ObjectContext context) : BaseObject(context){

}
