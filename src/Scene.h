//
// Created by kvtoD on 6/30/2025.
//

#ifndef SCENE_H
#define SCENE_H
#include <qevent.h>

#include "QRenderer2D.h"
#include "Events/KeyEvents.h"
#include "Managers/EventManager.h"


class Scene final : public QRenderer2D {
public:
    Scene();
    void render(SpiritBatch &batch) override;
    void timerEvent(QTimerEvent* event);
    void startGameLoop();

    void keyReleaseEvent(QKeyEvent *event) override;

    QBasicTimer timer;
    void keyPressEvent(QKeyEvent *event) override;
};


#endif //SCENE_H
