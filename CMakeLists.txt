cmake_minimum_required(VERSION 3.22)

set(PROJECT_NAME myVerlet)
set(SOURCES src/main.cpp
        src/Engine/EngineConsts.cpp
        src/Engine/utils/Vector.h
        src/Engine/Graphics.h
        src/Engine/Scene/Scene.h
        src/Engine/Scene/BaseObject/BaseObject.h
        src/Engine/Physics.h
        src/Engine/utils/Rand.h
        src/Engine/utils/Rectangle.h
        src/Engine/utils/Angle.h
        src/Engine/utils/Grid.h
        src/Engine/utils/ThreadPool.h
        src/Engine/PerformanceMonitor.h
        src/Engine/Camera.h
        src/Engine/BaseLevel.h
        src/Engine/BaseGame.h
        src/GameTest/Game.h
        src/GameTest/Levels/Level1.h
        src/Engine/Modules/ExplosionHandler.h
        src/Engine/SoundManager/SoundManagerImpl.h
        src/Engine/Modules/PerformanceTest/EmptyObject.h
        src/Engine/EventBus/EBHandle.cpp
        src/Engine/TimerManager/TMHandle.h
        src/Engine/TimerManager/TMHandle.cpp
        src/Engine/TimerManager/TimerManager.h
        src/Engine/TimerManager/TimerManagerImpl.h
        src/Engine/InputBus/IBHandle.h
        src/Engine/InputBus/IBHandle.cpp
        src/Engine/InputBus/InputBus.h
        src/Engine/InputBus/InputBusImpl.h
        src/GameTest/Events.h
        src/Engine/Modules/Shooter.h
        src/Engine/Modules/PerformanceTest/PerformanceTest.h
        src/Engine/Scene/BaseObject/BaseObject.cpp
        src/Engine/SoundManager/SoundManager.h
        src/Engine/SoundManager/SMHandle.h
        src/Engine/SoundManager/SMHandle.cpp
        src/Engine/DebugGraphics.h
        src/Engine/utils/lineClip.h
        src/Engine/utils/SparseSet.h
        src/Engine/Modules/RandomPositionGenerator.h
        src/Engine/Modules/CameraControls.h
        src/GameTest/Entities/Player.h
        src/GameTest/Entities/Enemy.h
        src/GameTest/Entities/IDamageable.h
        src/GameTest/Entities/Bullet.h
        src/Engine/Modules/CameraFollowing.h
        src/GameTest/UiRenderer.h
        src/GameTest/Entities/Food.h
        src/Engine/Scene/ObjectStorage.h
        src/Engine/Scene/BaseStick/BaseStick.h
        src/Engine/Scene/StickStorage.h
        src/Engine/Modules/PerformanceTest/EmptyStick.h
        src/Engine/Modules/PerformanceTest/Drawing.h
)

project(${PROJECT_NAME})

set(TGUI_STATIC_LIBRARIES FALSE)
set(TGUI_BACKEND "SFML_GRAPHICS")
set(SFML_DIR "C:/Libraries/SFML-2.6.1-windows-gcc-13.1.0-mingw-64-bit/SFML-2.6.1/lib/cmake/SFML")
find_package(SFML 2.6.1 COMPONENTS system window graphics audio REQUIRED)

set(TGUI_DIR "C:/Libraries/TGUI-1.1/lib/cmake/TGUI")
find_package(TGUI REQUIRED)
include_directories("C:/Libraries/TGUI-1.1/include/TGUI")

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    add_compile_definitions(IT_IS_DEBUG)
endif()

#first to generate profile second to startLoop
#set(CMAKE_CXX_FLAGS_RELEASE  "-Ofast -frename-registers -funroll-loops -fprofile-generate -fprofile-correction")
#set(CMAKE_CXX_FLAGS_RELEASE  "-Ofast -frename-registers -funroll-loops -fprofile-use -fprofile-correction")
set(CMAKE_CXX_STANDARD 23)

add_executable(${PROJECT_NAME} ${SOURCES})
target_link_libraries(${PROJECT_NAME} PUBLIC sfml-system sfml-window sfml-graphics sfml-audio)
target_link_libraries(${PROJECT_NAME} PRIVATE TGUI::TGUI)