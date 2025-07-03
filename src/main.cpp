#include <QApplication>
// #include <QWindow>
#include <QTimer>

#include "QRenderer2D.h"
#include "SpiritBatch.h"
// #include "TaskScheduler.h"
#include <qevent.h>

#include <entt/entt.hpp>
#include <stdexcept>

#include "Scene.h"
#include "Managers/EventManager.h"
#include "Compoments/Input.h"
#include "Systems/KeyboardControlSystem.h"
#include "Compoments/Keymap.h"
#include "TaskScheduler_c.h"
#include "World.h"
#include "../box2d/src/body.h"
#include "box2d/box2d.h"
#include "box2d/types.h"
#include "Compoments/Body.h"
#include "Compoments/Drawable.h"
#include "Compoments/Output.h"
#include "Compoments/PhysicsDesciption.h"
#include "Compoments/Tags.h"
#include "Compoments/Transform.h"
#include "Events/BodyEvents.h"
#include "Events/KeyEvents.h"
#include "Prefab/Player.h"
#include "Systems/PhysicsSystem.h"
#include "Type/Errors.h"

#include "Utils/Dumper.h"
#include "Utils/wrapper.h"


auto t = new Texture(QImage("assets/test.png"), 1, true, 1);
//
// struct TextureLoader {
//     using result_type = std::shared_ptr<Texture>;
//
//     template<typename Args>
//     result_type operator()(Args &&... args) const {
//         return std::make_shared<Texture>(std::forward<Args>(args));
//     }
// };
//
// using TextureCache = entt::resource_cache<Texture, TextureLoader>;
// TextureCache tc{};
//


// This callback must be thread-safe. It may be called multiple times simultaneously.
// Notice how this method is constant and doesn't change any data. It also
// does not try to access any values in the world that may be changing, such as contact data.

Scene::Scene()
{
    camera_zoom = 0.02;
}

auto& registry = World::getInstance().registry;

void Scene::render(SpiritBatch& batch)
{
    auto view = registry.view<const Drawable, const Transform>();
    view.each([&batch](
        // const entt::entity &entity,
        // const Drawable &drawable,
        const Transform& transform)
        {
            batch.draw(*t, transform.matrix);
        });
    batch.draw(*t, QMatrix4x4());
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


            float m_force = 25.0f;
            float m_impulse = 25.0f;
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
