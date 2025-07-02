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

bool m_jumping;
float m_radius;
float m_force;
float m_impulse;
float m_jumpDelay;
b2WorldId worldId;
b2BodyId m_movingPlatformId;

bool PreSolve(b2ShapeId shapeIdA, b2ShapeId shapeIdB, b2Vec2 point, b2Vec2 normal, void *context) {
    assert(b2Shape_IsValid(shapeIdA));
    assert(b2Shape_IsValid(shapeIdB));

    // // entt::entity ea = entityWrapper(b2Shape_GetUserData(shapeIdA));
    // //
    // float sign = 0.0f;
    // if (B2_ID_EQUALS(shapeIdA, m_playerShapeId)) {
    //     sign = -1.0f;
    // } else if (B2_ID_EQUALS(shapeIdB, m_playerShapeId)) {
    //     sign = 1.0f;
    // } else {
    //     // not colliding with the player, enable contact
    //     return true;
    // }
    //
    // if (sign * normal.y > 0.95f) {
    //     return true;
    // }

    return true;
    // normal points down, disable contact
    return false;
}

class PhysicsSystem : System<PhysicsSystem> {
public:
    constexpr static int e_maxTasks = 128;
    enkiTaskScheduler *scheduler;
    enkiTaskSet *tasks[e_maxTasks];

    typedef struct TaskData {
        b2TaskCallback *box2dTask;
        void *box2dContext;
    } TaskData;

    TaskData taskData[e_maxTasks];
    int taskCount = 0;


    PhysicsSystem() {
        constexpr int workerCount = 4;
        scheduler = enkiNewTaskScheduler();
        struct enkiTaskSchedulerConfig config = enkiGetTaskSchedulerConfig(scheduler);
        config.numTaskThreadsToCreate = workerCount - 1;
        enkiInitTaskSchedulerWithConfig(scheduler, config);
        for (int i = 0; i < e_maxTasks; ++i) {
            tasks[i] = enkiCreateTaskSet(scheduler, ExecuteRangeTask);
        }
        b2WorldDef worldDef = b2DefaultWorldDef();
        worldDef.userTaskContext = this;
        worldDef.enqueueTask = &PhysicsSystem::EnqueueTask;
        worldDef.finishTask = &PhysicsSystem::FinishTask;
        worldDef.workerCount = workerCount;

        worldId = b2CreateWorld(&worldDef);
        initWorld(worldId);
    }

    void initWorld(b2WorldId worldId) {
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


        m_force = 25.0f;
        m_impulse = 25.0f;
        m_jumpDelay = 0.25f;
        m_jumping = false;
    }

    void applyEffect() {
        auto &registry = World::getInstance().registry;
        const auto view = registry.view<const Body, const Output>();
        view.each([](const entt::entity &entity, const Body &body, const Output &output) {
            auto &bodyId = body.bodyID;
            b2Body_ApplyForceToCenter(bodyId, output.force, true);
            b2Body_ApplyLinearImpulseToCenter(bodyId, output.impulse, true);
        });
    }

    /* more crazy method
    std::vector<std::pair<entt::entity, b2BodySim*>> bodySimCache;

    // 初始化缓存（在物理初始化系统中调用一次）
    void buildBodySimCache(entt::registry& registry) {
        bodySimCache.clear();

        auto view = registry.view<PhysicsBody>();
        for (auto entity : view) {
            const auto& physics = registry.get<PhysicsBody>(entity);
            b2World* world = b2GetWorld(physics.id.world0);
            b2Body* body = b2GetBodyFullId(world, physics.id);

            b2SolverSet* set = b2SolverSetArray_Get(&world->solverSets, body->setIndex);
            b2BodySim* bodySim = b2BodySimArray_Get(&set->bodySims, body->localIndex);

            bodySimCache.emplace_back(entity, bodySim);
        }
    }

    // 同步时使用缓存
    void physicsSyncSystemFast() {
        for (const auto& [entity, bodySim] : bodySimCache) {
            auto& transform = registry.get<Transform>(entity);
            std::memcpy(&(transform.matrix), &(bodySim->transform), sizeof(b2Transform));
        }
    }
     */

    void syncData() {
        auto &registry = World::getInstance().registry;
        const auto view = registry.view<const Body, Transform>();

        view.each([](const entt::entity &entity, const Body &body, Transform &transform) {
            auto &bodyId = body.bodyID;
            transform.matrix = b2Body_GetTransform(bodyId);
        });
    }

    void update() override {
        createBody();
        applyEffect();
        step();
        syncData();
    };


    void createBody() {
        auto &registry = World::getInstance().registry;
        const auto view = registry.view<TagCreation, const Transform>();
        view.each([&](const entt::entity entity, const Transform &transform) {
            b2BodyDef bodyDef = b2DefaultBodyDef();
            if (const auto &movementDesc = registry.try_get<PhysicsDes_Movement>(entity)) {
                switch (movementDesc->type) {
                    case PhysicsDes_Movement::Dynamic:
                        bodyDef.type = b2_dynamicBody;
                        break;
                    case PhysicsDes_Movement::Static:
                        bodyDef.type = b2_staticBody;
                    case PhysicsDes_Movement::Kinematic:
                        bodyDef.type = b2_kinematicBody;
                }
                bodyDef.isBullet = movementDesc->isBullet;
                bodyDef.linearDamping = movementDesc->linearDamping;
                bodyDef.motionLocks.angularZ = movementDesc->rotationLocked;
                bodyDef.position = transform.matrix.p;
                bodyDef.userData = EntityWrapper(entity);
            } else {
                throw ComponentNotFoundException("PhysicsDes_Movement component not found");
            }
            const b2BodyId bodyId = b2CreateBody(worldId, &bodyDef);
            b2ShapeDef shapeDef = b2DefaultShapeDef();
            shapeDef.userData = EntityWrapper(entity);
            shapeDef.enablePreSolveEvents = true;
            b2ShapeId shapeId;
            if (const auto &capsuleDesc = registry.try_get<PhysicsDes_CapsuleShapeDesc>(entity)) {
                shapeDef.material.friction = capsuleDesc->material.friction;
                const b2Capsule shape = {
                    .center1 = transform.matrix.p + Vector{0, capsuleDesc->halfHeight},
                    .center2 = transform.matrix.p - Vector{0, -capsuleDesc->halfHeight},
                    .radius = capsuleDesc->radius
                };
                shapeId = b2CreateCapsuleShape(bodyId, &shapeDef, &shape);
            } else if (const auto &boxDesc = registry.try_get<PhysicsDes_BoxShapeDesc>(entity)) {
                shapeDef.material.friction = boxDesc->material.friction;
                const b2Polygon shape = b2MakeOffsetBox(boxDesc->halfWidth, boxDesc->halfHeight, transform.matrix.p,
                                                        transform.matrix.q);

                shapeId = b2CreatePolygonShape(bodyId, &shapeDef, &shape);
            } else {
                throw ComponentNotFoundException("Shape Desc component not found");
            }

            registry.emplace<Body>(entity, Body{.bodyID = bodyId, .shapeID = shapeId});
            registry.erase<TagCreation>(entity);
        });
    }

    void step() {
        // bool canJump = false;
        // b2Vec2 velocity = b2Body_GetLinearVelocity(m_playerId);
        // if (m_jumpDelay == 0.0f && m_jumping == false && velocity.y < 0.01f) {
        //     int capacity = b2Body_GetContactCapacity(m_playerId);
        //     capacity = b2MinInt(capacity, 4);
        //     b2ContactData contactData[4];
        //     int count = b2Body_GetContactData(m_playerId, contactData, capacity);
        //     for (int i = 0; i < count; ++i) {
        //         b2BodyId bodyIdA = b2Shape_GetBody(contactData[i].shapeIdA);
        //         float sign = 0.0f;
        //         if (B2_ID_EQUALS(bodyIdA, m_playerId)) {
        //             // normal points from A to B
        //             sign = -1.0f;
        //         } else {
        //             sign = 1.0f;
        //         }
        //
        //         if (sign * contactData[i].manifold.normal.y > 0.9f) {
        //             canJump = true;
        //             break;
        //         }
        //     }
        // }

        // A Kinematic body is moved by setting its velocity. This
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
        // int contactCount = b2Body_GetContactData(m_movingPlatformId, &contactData, 1);

        // if (m_context->hertz > 0.0f) {
        //     m_jumpDelay = b2MaxFloat(0.0f, m_jumpDelay - 1.0f / m_context->hertz);
        // }
    }

    ~PhysicsSystem() {
        b2DestroyWorld(worldId);
        for (int i = 0; i < e_maxTasks; ++i) {
            enkiDeleteTaskSet(scheduler, tasks[i]);
        }
        enkiDeleteTaskScheduler(scheduler);
    }

    static void ExecuteRangeTask(const uint32_t start, const uint32_t end, const uint32_t threadIndex, void *context) {
        const auto data = static_cast<TaskData *>(context);
        data->box2dTask(start, end, threadIndex, data->box2dContext);
    }

    static void *EnqueueTask(b2TaskCallback *box2dTask, int itemCount, int minRange, void *box2dContext,
                             void *userContext) {
        return static_cast<PhysicsSystem *>(userContext)->EnqueueTaskImpl(box2dTask, itemCount, minRange, box2dContext);
    }

    static void FinishTask(void *userTask, void *userContext) {
        static_cast<PhysicsSystem *>(userContext)->FinishTaskImpl(userTask);
    }

    void *EnqueueTaskImpl(b2TaskCallback *box2dTask, int itemCount, int minRange, void *box2dContext) {
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

    void FinishTaskImpl(void *userTask) const {
        const auto task = static_cast<enkiTaskSet *>(userTask);
        enkiWaitForTaskSet(scheduler, task);
    }
};


// This callback must be thread-safe. It may be called multiple times simultaneously.
// Notice how this method is constant and doesn't change any data. It also
// does not try to access any values in the world that may be changing, such as contact data.

Scene::Scene() {
    camera_zoom = 0.02;
}

auto &registry = World::getInstance().registry;

void Scene::render(SpiritBatch &batch) {
    auto view = registry.view<const Drawable, const Transform>();
    view.each([&batch](
    // const entt::entity &entity,
    // const Drawable &drawable,
    const Transform &transform) {
            batch.draw(*t, transform.matrix);
        });
    batch.draw(*t, QMatrix4x4());
}


int main(int argc, char *argv[]) {
    QApplication a(argc, argv);

    Player p;
    p.build(Matrix::MakeTranslation({0, 1}));

    PhysicsSystem ps;
    Scene scene;
    scene.show();

    auto loop = [&]() {
        KeyboardControlSystem::getInstance().update();
        for (const auto view = registry.view<const Input>(); const auto entity: view) {
            const auto &input = registry.get<Input>(entity);
            auto &o = registry.get<Output>(entity);
            o.reset();

            if (input.left) {
                o.force = {-m_force, 0};
            }
            if (input.right) {
                o.force = {m_force, 0};
            }
            if (input.up) {
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
