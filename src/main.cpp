#include <QApplication>
// #include <QWindow>
#include <QTimer>

#include "Scene.h"
#include "SpiritBatch.h"
// #include "TaskScheduler.h"
#include <qevent.h>

#include "TaskScheduler_c.h"
#include "../box2d/src/body.h"
#include "box2d/box2d.h"
#include "box2d/types.h"


Texture *t = new Texture(QImage("assets/test.png"), 1, true, 1);

enum {
    e_maxTasks = 128,
};

typedef struct TaskData {
    b2TaskCallback *box2dTask;
    void *box2dContext;
} TaskData;


enkiTaskScheduler *scheduler;
enkiTaskSet *tasks[e_maxTasks];
TaskData taskData[e_maxTasks];
int taskCount;

static void ExecuteRangeTask(const uint32_t start, const uint32_t end, const uint32_t threadIndex, void *context) {
    const auto data = static_cast<TaskData *>(context);
    data->box2dTask(start, end, threadIndex, data->box2dContext);
}

static void *EnqueueTask(b2TaskCallback *box2dTask, int itemCount, int minRange, void *box2dContext,
                         void *userContext) {
    if (taskCount < e_maxTasks) {
        enkiTaskSet *task = tasks[taskCount];
        TaskData *data = taskData + taskCount;
        data->box2dTask = box2dTask;
        data->box2dContext = box2dContext;

        struct enkiParamsTaskSet params;
        params.minRange = minRange;
        params.setSize = itemCount;
        params.pArgs = data;
        params.priority = 0;

        enkiSetParamsTaskSet(task, params);
        enkiAddTaskSet(scheduler, task);

        ++taskCount;

        return task;
    }

    box2dTask(0, itemCount, 0, box2dContext);
    return nullptr;
}

static void FinishTask(void *userTask, void *userContext) {
    const auto task = static_cast<enkiTaskSet *>(userTask);
    enkiWaitForTaskSet(scheduler, task);
}

bool m_jumping;
float m_radius;
float m_force;
float m_impulse;
float m_jumpDelay;
b2WorldId worldId;
b2BodyId m_playerId;
b2ShapeId m_playerShapeId;
b2BodyId m_movingPlatformId;

bool PreSolve(b2ShapeId shapeIdA, b2ShapeId shapeIdB, b2Vec2 point, b2Vec2 normal, void *context) {
    assert(b2Shape_IsValid(shapeIdA));
    assert(b2Shape_IsValid(shapeIdB));

    float sign = 0.0f;
    if (B2_ID_EQUALS(shapeIdA, m_playerShapeId)) {
        sign = -1.0f;
    } else if (B2_ID_EQUALS(shapeIdB, m_playerShapeId)) {
        sign = 1.0f;
    } else {
        // not colliding with the player, enable contact
        return true;
    }

    if (sign * normal.y > 0.95f) {
        return true;
    }

    // normal points down, disable contact
    return false;
}

static int run() {
    constexpr int workerCount = 4;
    scheduler = enkiNewTaskScheduler();
    struct enkiTaskSchedulerConfig config = enkiGetTaskSchedulerConfig(scheduler);
    config.numTaskThreadsToCreate = workerCount - 1;
    enkiInitTaskSchedulerWithConfig(scheduler, config);
    for (int i = 0; i < e_maxTasks; ++i) {
        tasks[i] = enkiCreateTaskSet(scheduler, ExecuteRangeTask);
    }
    b2WorldDef worldDef = b2DefaultWorldDef();
    worldDef.enqueueTask = EnqueueTask;
    worldDef.finishTask = FinishTask;
    worldDef.workerCount = workerCount;

    worldId = b2CreateWorld(&worldDef);


    b2World_SetPreSolveCallback(worldId, PreSolve, nullptr);
    // Ground
    {
        b2BodyDef bodyDef = b2DefaultBodyDef();
        b2BodyId groundId = b2CreateBody(worldId, &bodyDef);
        b2ShapeDef shapeDef = b2DefaultShapeDef();
        b2Segment segment = {{-20.0f, 0.0f}, {20.0f, 0.0f}};
        b2CreateSegmentShape(groundId, &shapeDef, &segment);
    }

    // Static Platform
    // This tests pre-solve with continuous collision
    {
        b2BodyDef bodyDef = b2DefaultBodyDef();
        bodyDef.type = b2_staticBody;
        bodyDef.position = {-6.0f, 6.0f};
        b2BodyId bodyId = b2CreateBody(worldId, &bodyDef);

        b2ShapeDef shapeDef = b2DefaultShapeDef();

        // Need to turn this on to get the callback
        shapeDef.enablePreSolveEvents = true;

        b2Polygon box = b2MakeBox(2.0f, 0.5f);
        b2CreatePolygonShape(bodyId, &shapeDef, &box);
    }

    // Moving Platform
    {
        b2BodyDef bodyDef = b2DefaultBodyDef();
        bodyDef.type = b2_kinematicBody;
        bodyDef.position = {0.0f, 6.0f};
        bodyDef.linearVelocity = {2.0f, 0.0f};
        m_movingPlatformId = b2CreateBody(worldId, &bodyDef);

        b2ShapeDef shapeDef = b2DefaultShapeDef();

        // Need to turn this on to get the callback
        shapeDef.enablePreSolveEvents = true;

        b2Polygon box = b2MakeBox(3.0f, 0.5f);
        b2CreatePolygonShape(m_movingPlatformId, &shapeDef, &box);
    }

    // Player
    {
        b2BodyDef bodyDef = b2DefaultBodyDef();
        bodyDef.type = b2_dynamicBody;
        bodyDef.motionLocks.angularZ = true;
        bodyDef.linearDamping = 0.5f;
        bodyDef.position = {0.0f, 1.0f};
        m_playerId = b2CreateBody(worldId, &bodyDef);

        m_radius = 0.5f;
        b2Capsule capsule = {{0.0f, 0.0f}, {0.0f, 1.0f}, m_radius};
        b2ShapeDef shapeDef = b2DefaultShapeDef();
        shapeDef.material.friction = 0.1f;

        m_playerShapeId = b2CreateCapsuleShape(m_playerId, &shapeDef, &capsule);
    }

    m_force = 25.0f;
    m_impulse = 25.0f;
    m_jumpDelay = 0.25f;
    m_jumping = false;


    //
    // b2DestroyWorld(worldId);
    //
    // for (int i = 0; i < e_maxTasks; ++i) {
    //     enkiDeleteTaskSet(scheduler, tasks[i]);
    // }
    //
    // enkiDeleteTaskScheduler(scheduler);

    return 0;
}

void step() {
    bool canJump = false;
    b2Vec2 velocity = b2Body_GetLinearVelocity(m_playerId);
    if (m_jumpDelay == 0.0f && m_jumping == false && velocity.y < 0.01f) {
        int capacity = b2Body_GetContactCapacity(m_playerId);
        capacity = b2MinInt(capacity, 4);
        b2ContactData contactData[4];
        int count = b2Body_GetContactData(m_playerId, contactData, capacity);
        for (int i = 0; i < count; ++i) {
            b2BodyId bodyIdA = b2Shape_GetBody(contactData[i].shapeIdA);
            float sign = 0.0f;
            if (B2_ID_EQUALS(bodyIdA, m_playerId)) {
                // normal points from A to B
                sign = -1.0f;
            } else {
                sign = 1.0f;
            }

            if (sign * contactData[i].manifold.normal.y > 0.9f) {
                canJump = true;
                break;
            }
        }
    }

    // A kinematic body is moved by setting its velocity. This
    // ensure friction works correctly.
    b2Vec2 platformPosition = b2Body_GetPosition(m_movingPlatformId);
    if (platformPosition.x < -15.0f) {
        b2Body_SetLinearVelocity(m_movingPlatformId, {2.0f, 0.0f});
    } else if (platformPosition.x > 15.0f) {
        b2Body_SetLinearVelocity(m_movingPlatformId, {-2.0f, 0.0f});
    }


    constexpr float timeStep = 1.0f / 60.0f;


    constexpr int subStepCount = 4;
    b2World_Step(worldId, timeStep, subStepCount);


    b2ContactData contactData = {};
    int contactCount = b2Body_GetContactData(m_movingPlatformId, &contactData, 1);
    printf("Platform contact count = %d, point count = %d \n", contactCount, contactData.manifold.pointCount);
    printf("Can jump = %s \n", canJump ? "true" : "false");

    // if (m_context->hertz > 0.0f) {
    //     m_jumpDelay = b2MaxFloat(0.0f, m_jumpDelay - 1.0f / m_context->hertz);
    // }
}

// This callback must be thread-safe. It may be called multiple times simultaneously.
// Notice how this method is constant and doesn't change any data. It also
// does not try to access any values in the world that may be changing, such as contact data.


class MyScene : public Scene {
    void render(SpiritBatch &batch) override;


    void keyPressEvent(QKeyEvent *event) override;

    void keyReleaseEvent(QKeyEvent *event) override;
};

inline QMatrix4x4 b2q(const b2Transform &transform) {
    float x = transform.p.x;
    float y = transform.p.y;
    float c = transform.q.c;
    float s = transform.q.s;

    return {
        c, s, 0, x,
        -s, c, 0, y,
        0, 0, 1, 0,
        0, 0, 0, 1
    };
}

void MyScene::render(SpiritBatch &batch) {
    step();
    auto tfm = b2Body_GetTransform(m_playerId);

    batch.draw(*t, QMatrix4x4());
    batch.draw(*t, b2q(tfm));
}

void MyScene::keyPressEvent(QKeyEvent *event) {
    switch (event->key()) {
        case Qt::Key_A: {
            b2Body_ApplyForceToCenter(m_playerId, {-m_force, 0.0f}, true);
            break;
        }
        case Qt::Key_D: {
            b2Body_ApplyForceToCenter(m_playerId, {m_force, 0.0f}, true);
            break;
        }
        case Qt::Key_Space: {
            if (!m_jumping) {
                b2Body_ApplyLinearImpulseToCenter(m_playerId, {0.0f, m_impulse}, true);
                m_jumpDelay = 0.5f;
                m_jumping = true;
            }
            break;
        }
        default:
            QWidget::keyPressEvent(event);
            break;
    }
}

void MyScene::keyReleaseEvent(QKeyEvent *event) {
    if (event->key() == Qt::Key_Space) {
        m_jumping = false;
    }
}


int main(int argc, char *argv[]) {
    run();

    QApplication a(argc, argv);
    MyScene bfw;
    bfw.show();
    QTimer timer;

    QObject::connect(&timer, &QTimer::timeout, &bfw, &Scene::flush);
    timer.start(16);
    return QApplication::exec();
}
