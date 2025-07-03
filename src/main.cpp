#include <QApplication>
// #include <QWindow>
#include <QTimer>

#include "QRenderer2D.h"
#include "SpiritBatch.h"

#include <entt/entt.hpp>

#include "Scene.h"
#include "Components/Input.h"
#include "Systems/KeyboardControlSystem.h"
#include "World.h"
#include "Components/Drawable.h"
#include "Components/Output.h"
#include "Components/PhysicsDesciption.h"
#include "Components/Transform.h"
#include "Events/BodyEvents.h"
#include "Prefab/Player.h"
#include "Systems/PhysicsSystem.h"
auto& registry = World::getInstance().registry;

void Scene::render(SpiritBatch& batch)
{
    const auto view = registry.view<const Drawable, const Transform>();
    view.each([&batch](
        // const entt::entity &entity,
        const Drawable& drawable,
        const Transform& transform)
        {
            batch.draw(*drawable.texture, transform.matrix);
        });
}


int main(int argc, char* argv[])
{
    QApplication a(argc, argv);

    Player p;
    p.build(Matrix::MakeTranslation({0, 1}));

    PhysicsSystem ps;
    Scene scene;
    scene.show();

    auto loop = [&]()
    {
        KeyboardControlSystem::getInstance().update();
        for (const auto view = registry.view<const Input>(); const auto entity : view)
        {
            const auto& input = registry.get<Input>(entity);
            auto& o = registry.get<Output>(entity);
            o.reset();


            constexpr float m_force = 25.0f;
            constexpr float m_impulse = 25.0f;
            if (input.left)
            {
                o.force = {-m_force, 0};
            }
            if (input.right)
            {
                o.force = {m_force, 0};
            }
            if (input.up)
            {
                o.impulse = {0.0f, m_impulse};
            }
        }
        // dump<Transform>();

        ps.update();

        scene.flush();
        // dump(e);
    };

    QTimer timer;
    QObject::connect(&timer, &QTimer::timeout, loop);
    timer.start(16);
    return QApplication::exec();
}
