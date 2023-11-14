cmake_minimum_required(VERSION 3.22)

set(PROJECT_NAME myVerlet)
set(SOURCES src/main.cpp
        src/utils/Vector.h
        src/Graphics/Graphics.h
        src/AtomWorld/AtomWorld.h
        src/AtomWorld/VerletObject.h
        src/consts.h
        src/Physics/Physics.h
        src/utils/Rand.h
        src/utils/Rectangle.h
        src/utils/Angle.h
        src/utils/ThreadsTest.h
        src/Physics/Grid.h
        src/AtomWorld/VerletStick.h
        src/utils/ThreadPool.h
        src/PerformanceMonitor/PerformanceMonitor.h
        src/addons/ExplosionHandler.h
        src/addons/Shooter.h
        src/addons/RandomSpawner.h
        src/addons/Chain.h
        src/addons/Square.h
        src/addons/Benchmark.h
        src/World/ComplexObject.h
        src/World/World.h
        src/World/Rocket.h
        src/World/Dumb.h
)

project(${PROJECT_NAME})

#include(FetchContent)
#set(BUILD_SHARED_LIBS OFF)
#fetchcontent_declare(SFML GIT_REPOSITORY https://github.com/SFML/SFML.git GIT_TAG 2.5.1)
#fetchcontent_makeavailable(SFML)
set(SFML_DIR "C:/Users/ronik/Downloads/SFML-2.6.1-windows-gcc-13.1.0-mingw-64-bit/SFML-2.6.1/lib/cmake/SFML")
find_package(SFML 2.6.1 COMPONENTS system window graphics REQUIRED)
#first to generate profile second to run
#set(CMAKE_CXX_FLAGS_RELEASE  "-Ofast -frename-registers -funroll-loops -fprofile-generate -fprofile-correction")
#set(CMAKE_CXX_FLAGS_RELEASE  "-Ofast -frename-registers -funroll-loops -fprofile-use -fprofile-correction")
set(CMAKE_CXX_STANDARD 23)

add_executable(${PROJECT_NAME} ${SOURCES})
target_link_libraries(${PROJECT_NAME} sfml-system sfml-window sfml-graphics)
