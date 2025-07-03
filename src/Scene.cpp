//
// Created by kvtoD on 6/30/2025.
//

#include "Scene.h"

// void Scene::render(SpiritBatch &batch) {
// }

void Scene::keyReleaseEvent(QKeyEvent* event)
{
    if (event->key() == Qt::Key_Space)
    {
        EventManager::getInstance().enqueue(AtKey{Key::Key_Space});
    }
}

Scene::Scene()
{
    camera_zoom = 0.02;
}

void Scene::keyPressEvent(QKeyEvent* event)
{
    switch (event->key())
    {
    case Qt::Key_A:
        {
            EventManager::getInstance().enqueue(OnKey{Key_A});
            break;
        }
    case Qt::Key_D:
        {
            EventManager::getInstance().enqueue(OnKey{Key_D});
            break;
        }
    case Qt::Key_W:
        {
            EventManager::getInstance().enqueue(OnKey{Key_W});
            break;
        }
    default:
        QWidget::keyPressEvent(event);
        break;
    }
}
