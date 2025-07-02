cmake_minimum_required(VERSION 3.28)
project(lucknight)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

add_definitions(-DENTT_ID_TYPE=std::uintptr_t)
add_subdirectory(box2d)
add_subdirectory(QRenderer2D)
add_subdirectory(entt)
add_subdirectory(enkiTS)
find_package(Qt6 COMPONENTS
        Core
        Gui
        Widgets
        REQUIRED)
add_executable(lucknight src/main.cpp
        src/Systems/System.cpp
        src/Utils/Singletion.cpp
        src/Managers/EventManager.cpp
        src/Systems/KeyboardControlSystem.cpp
        src/World.cpp
        src/Compoments/Body.h
        src/Compoments/Transform.h
        src/Prefab/Prefab.cpp
        src/Prefab/Prefab.h
        src/Type/Matrix.h
        src/Prefab/Player.cpp
        src/Prefab/Player.h
        src/Scene.cpp
        src/Scene.h
        src/Compoments/PhysicsDesciption.h
        src/Compoments/Drawable.h
        src/Events/BodyEvents.h
        src/Events/EntityEvents.h
        src/Type/Errors.h
        src/Compoments/Output.h
        src/Compoments/Tags.h
        src/Utils/Dumper.h
        src/Utils/wrapper.h
)
target_link_libraries(lucknight

        Qt::Core
        Qt::Gui
        Qt::Widgets

        QRenderer2D
        box2d::box2d
        EnTT::EnTT
        enkiTS

)

