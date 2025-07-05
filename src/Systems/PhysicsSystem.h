//
// Created by root on 7/3/25.
//

#ifndef PHYSICSSYSTEM_H
#define PHYSICSSYSTEM_H
#include "System.h"
#include "TaskScheduler_c.h"
#include "../Core/World.h"
#include "../Components/Body.h"
#include "../Components/Transform.h"
#include "../Components/Output.h"
#include "../Components/PhysicsDesciption.h"
#include "../Utils/wrapper.h"
#include "../Components/Tags.h"
#include "../Type/Errors.h"
#include "box2d/box2d.h"
#include "box2d/types.h"


class PhysicsSystem final :public System<PhysicsSystem>
{
public:
    b2WorldId worldId{};
    b2BodyId m_movingPlatformId{};
    constexpr static int e_maxTasks = 128;
    enkiTaskScheduler* scheduler;
    enkiTaskSet* tasks[e_maxTasks]{};

    typedef struct TaskData
    {
        b2TaskCallback* box2dTask;
        void* box2dContext;
    } TaskData;

    TaskData taskData[e_maxTasks]{};
    int taskCount = 0;


    static bool PreSolve(b2ShapeId shapeIdA, b2ShapeId shapeIdB, b2Vec2 point, b2Vec2 normal, void* context);

    PhysicsSystem()
    {
        constexpr int workerCount = 4;
        scheduler = enkiNewTaskScheduler();
        struct enkiTaskSchedulerConfig config = enkiGetTaskSchedulerConfig(scheduler);
        config.numTaskThreadsToCreate = workerCount - 1;
        enkiInitTaskSchedulerWithConfig(scheduler, config);
        for (auto & task : tasks)
        {
            task = enkiCreateTaskSet(scheduler, ExecuteRangeTask);
        }
        b2WorldDef worldDef = b2DefaultWorldDef();
        worldDef.userTaskContext = this;
        worldDef.enqueueTask = &PhysicsSystem::EnqueueTask;
        worldDef.finishTask = &PhysicsSystem::FinishTask;
        worldDef.workerCount = workerCount;

        worldId = b2CreateWorld(&worldDef);
        initWorld(worldId);
    }

    void initWorld(b2WorldId worldId)
    {
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


    }

    void applyEffect();

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

    void syncData();

    void destroyBody();

    void update() override;;


    void createBody() const;

    void step() const;

    ~PhysicsSystem() override;

    static void ExecuteRangeTask(uint32_t start, uint32_t end, uint32_t threadIndex, void* context);

    static void* EnqueueTask(b2TaskCallback* box2dTask, int itemCount, int minRange, void* box2dContext,
                             void* userContext);

    static void FinishTask(void* userTask, void* userContext);

    void* EnqueueTaskImpl(b2TaskCallback* box2dTask, int itemCount, int minRange, void* box2dContext);

    void FinishTaskImpl(void* userTask) const;
};


#endif //PHYSICSSYSTEM_H
