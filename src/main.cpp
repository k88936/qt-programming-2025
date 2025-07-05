#include <QApplication>
// #include <QWindow>
#include <QBasicTimer>

#include "QRenderer2D.h"
#include "SpiritBatch.h"

#include <entt/entt.hpp>

#include "Core/Scene.h"
#include "Systems/KeyboardControlSystem.h"
#include "Core/World.h"
#include "Components/Drawable.h"
#include "Components/PhysicsDesciption.h"
#include "Components/Transform.h"
#include "Prefab/Player.h"
#include "Scripts/PlayerScript.h"
#include "Systems/PhysicsSystem.h"

int main(int argc, char* argv[])
{
    QApplication a(argc, argv);


    Scene scene;
    scene.show();
    scene.startGameLoop();

    return QApplication::exec();
}
