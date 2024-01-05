//#pragma once
//
//#include "../utils/Vector.h"
//#include "../AtomWorld/AtomWorld.h"
//#include "../consts.h"
//
//class Square {
//public:
//    Square(AtomWorld& world, Vector2 startPos, float size) {
//        int ind1 = world.addObject(VerletObject(startPos) );
//        int ind2 = world.addObject(VerletObject(startPos + Vector2::fromCartesian(size,0)) );
//        int ind3 = world.addObject(VerletObject(startPos + Vector2::fromCartesian(size,size)) );
//        int ind4 = world.addObject(VerletObject(startPos + Vector2::fromCartesian(0,size)) );
//
//        world.addStick(ind1, ind2);
//        world.addStick(ind2, ind3);
//        world.addStick(ind3, ind4);
//        world.addStick(ind4, ind1);
//        world.addStick(ind1, ind3);
//        world.addStick(ind4, ind2);
//    }
//};