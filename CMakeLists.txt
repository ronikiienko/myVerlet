cmake_minimum_required(VERSION 3.22)

set(PROJECT_NAME myVerlet)
set(SOURCES src/main.cpp
        src/Engine/utils/Vector.h
        src/Engine/Graphics.h
        src/Engine/Scene.h
        src/Engine/BaseObject.h
        src/Engine/EngineConsts.h
        src/Engine/Physics.h
        src/Engine/utils/Rand.h
        src/Engine/utils/Rectangle.h
        src/Engine/utils/Angle.h
        src/Engine/utils/ThreadsTest.h
        src/Engine/utils/Grid.h
        src/Engine/utils/ThreadPool.h
        src/Engine/PerformanceMonitor.h
        src/addons/RandomSpawner.h
        src/Engine/Camera.h
        src/Engine/InputHandler.h
        src/Engine/BaseLevel.h
        src/Engine/BaseGame.h
        src/Game.h
        src/Level.h
        src/addons/ExplosionHandler.h
        src/Objects/Player.h
        src/addons/Shooter.h
        src/Objects/Bullet.h
        src/Engine/EventBus.h
        src/Engine/SoundManager.h
)

project(${PROJECT_NAME})

set(SFML_DIR "C:/Users/ronik/Downloads/SFML-2.6.1-windows-gcc-13.1.0-mingw-64-bit/SFML-2.6.1/lib/cmake/SFML")
find_package(SFML 2.6.1 COMPONENTS system window graphics audio REQUIRED)
#first to generate profile second to startLoop
#set(CMAKE_CXX_FLAGS_RELEASE  "-Ofast -frename-registers -funroll-loops -fprofile-generate -fprofile-correction")
#set(CMAKE_CXX_FLAGS_RELEASE  "-Ofast -frename-registers -funroll-loops -fprofile-use -fprofile-correction")
set(CMAKE_CXX_STANDARD 23)

add_executable(${PROJECT_NAME} ${SOURCES})
target_link_libraries(${PROJECT_NAME} sfml-system sfml-window sfml-graphics sfml-audio)
target_link_options(${PROJECT_NAME} PRIVATE -static-libgcc -static-libstdc++)