cmake_minimum_required(VERSION 3.26)

set(PROJECT_NAME myVerlet)
set(SOURCES src/main.cpp
        src/modules/Vector.h
        src/modules/Graphics.h
        src/modules/World.h
        src/modules/VerletObject.h
        src/consts.h
        src/modules/Physics.h
        src/modules/Rand.h
        src/modules/Rectangle.h
        src/modules/Angle.h
        src/modules/ThreadsTest.h)

project(${PROJECT_NAME})

include(FetchContent)
set(BUILD_SHARED_LIBS OFF)
fetchcontent_declare(SFML GIT_REPOSITORY https://github.com/SFML/SFML.git GIT_TAG 2.5.1)
fetchcontent_makeavailable(SFML)

set(CMAKE_CXX_STANDARD 23)

add_executable(${PROJECT_NAME} ${SOURCES})
target_link_libraries(${PROJECT_NAME} sfml-system sfml-window sfml-graphics)
