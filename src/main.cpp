#include <QApplication>
// #include <QWindow>
#include <QBasicTimer>

#include "QRenderer2D.h"
#include "SpiritBatch.h"

#include <entt/entt.hpp>

#include "Scene.h"
#include "Systems/KeyboardControlSystem.h"
#include "Systems/StateMachineSystem.h"
#include "World.h"
#include "Components/Drawable.h"
#include "Components/PhysicsDesciption.h"
#include "Components/Transform.h"
#include "Events/BodyEvents.h"
#include "Prefab/Player.h"
#include "Systems/PhysicsSystem.h"

int main(int argc, char* argv[])
{
    QApplication a(argc, argv);

    Player p;
    Matrix m;
    m.translate(0,1);
    p.build(m);

    Scene scene;
    scene.show();
    scene.startGameLoop();

    return QApplication::exec();
}
