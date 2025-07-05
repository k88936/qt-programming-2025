//
// Created by kvtoD on 6/30/2025.
//

#include "Scene.h"

#include "World.h"
#include "../Components/Drawable.h"
#include "../Components/Transform.h"
#include "../Events/KeyEvents.h"

void Scene::render(SpiritBatch& batch)
{
    auto& registry = World::getInstance().registry;
    const auto view = registry.view<const Drawable, const Transform>();
    view.each([&batch](
        const Drawable& drawable,
        const Transform& transform)
        {
            assert(drawable.texture);
            batch.draw(*drawable.texture, transform.matrix);
        });
}

void Scene::timerEvent(QTimerEvent* event)
{
    if (event->timerId() == timer.timerId())
    {
        World::getInstance().update();
        flush();
    }
}

void Scene::startGameLoop()
{
    World::getInstance().init();
    timer.start(16, this);
}

void Scene::keyReleaseEvent(QKeyEvent* event)
{
    EventManager::getInstance().dispatcher.trigger(ReleaseKey{static_cast<Key>(event->key())});
}

Scene::Scene()
{
    camera_zoom = 0.02;
}

void Scene::keyPressEvent(QKeyEvent* event)
{
    EventManager::getInstance().dispatcher.trigger(PressKey{static_cast<Key>(event->key())});
}
