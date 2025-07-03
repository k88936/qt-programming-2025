//
// Created by root on 7/3/25.
//

#include "PhysicsSystem.h"

bool PhysicsSystem::PreSolve(const b2ShapeId shapeIdA, const b2ShapeId shapeIdB, b2Vec2 point, b2Vec2 normal,
                             void* context)
{
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

void PhysicsSystem::applyEffect()
{
    auto& registry = World::getInstance().registry;
    const auto view = registry.view<const Body, const Output>();
    view.each([](const entt::entity& entity, const Body& body, const Output& output)
    {
        auto& bodyId = body.bodyID;
        b2Body_ApplyForceToCenter(bodyId, output.force, true);
        b2Body_ApplyLinearImpulseToCenter(bodyId, output.impulse, true);
    });
}

void PhysicsSystem::syncData()
{
    auto& registry = World::getInstance().registry;
    const auto view = registry.view<const
                                    Body, Transform>();

    view.each([](const entt::entity& entity, const Body& body, Transform& transform)
    {
        auto& bodyId = body.bodyID;
        transform.matrix = b2Body_GetTransform(bodyId);
    });
}

void PhysicsSystem::destroyBody()
{
    auto& registry = World::getInstance().registry;
    const auto view = registry.view<const
                                    Body, TagDestruction>();
    view.each([&](const entt::entity entity, const Body& body)

    {
        b2DestroyBody(body.bodyID);
        registry.erase<TagDestruction>(entity);
    });
}

void PhysicsSystem::update()
{
    createBody();
    destroyBody();
    applyEffect();
    step();
    syncData();
}

void PhysicsSystem::createBody() const
{
    auto& registry = World::getInstance().registry;
    const auto view = registry.view<TagCreation,
                                    const Transform>
        ();
    view.each([&](const entt::entity entity, const Transform& transform)
    {
        b2BodyDef bodyDef = b2DefaultBodyDef();
        if (const auto& movementDesc = registry.try_get<PhysicsDes_Movement>(entity))
        {
            switch (movementDesc->type)
            {
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
        }
        else
        {
            throw ComponentNotFoundException("PhysicsDes_Movement component not found");
        }
        const b2BodyId bodyId = b2CreateBody(worldId, &bodyDef);
        b2ShapeDef shapeDef = b2DefaultShapeDef();
        shapeDef.userData = EntityWrapper(entity);
        shapeDef.enablePreSolveEvents = true;
        b2ShapeId shapeId;
        if (const auto& capsuleDesc = registry.try_get<PhysicsDes_CapsuleShapeDesc>(entity))
        {
            shapeDef.material.friction = capsuleDesc->material.friction;
            const b2Capsule shape = {
                .center1 = transform.matrix.p + Vector{0, capsuleDesc->halfHeight},
                .center2 = transform.matrix.p - Vector{0, -capsuleDesc->halfHeight},
                .radius = capsuleDesc->radius
            };
            shapeId = b2CreateCapsuleShape(bodyId, &shapeDef, &shape);
        }
        else if (const auto& boxDesc = registry.try_get<PhysicsDes_BoxShapeDesc>(entity))
        {
            shapeDef.material.friction = boxDesc->material.friction;
            const b2Polygon shape = b2MakeOffsetBox(boxDesc->halfWidth, boxDesc->halfHeight, transform.matrix.p,
                                                    transform.matrix.q);

            shapeId = b2CreatePolygonShape(bodyId, &shapeDef, &shape);
        }
        else
        {
            throw ComponentNotFoundException("Shape Desc component not found");
        }

        registry.emplace<Body>(entity, Body{.bodyID = bodyId, .shapeID = shapeId});
        registry.erase<TagCreation>(entity);
    });
}

void PhysicsSystem::step() const
{
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
    if (platformPosition.x < -15.0f)
    {
        b2Body_SetLinearVelocity(m_movingPlatformId, {2.0f, 0.0f});
    }
    else if (platformPosition.x > 15.0f)
    {
        b2Body_SetLinearVelocity(m_movingPlatformId, {-2.0f, 0.0f});
    }


    constexpr float timeStep = 1.0f / 60.0f;


    constexpr int subStepCount = 4;
    b2World_Step(worldId, timeStep, subStepCount);


    // b2ContactData contactData = {};
    // int contactCount = b2Body_GetContactData(m_movingPlatformId, &contactData, 1);

    // if (m_context->hertz > 0.0f) {
    //     m_jumpDelay = b2MaxFloat(0.0f, m_jumpDelay - 1.0f / m_context->hertz);
    // }
}

PhysicsSystem::~PhysicsSystem()
{
    b2DestroyWorld(worldId);
    for (const auto & task : tasks)
    {
        enkiDeleteTaskSet(scheduler, task);
    }
    enkiDeleteTaskScheduler(scheduler);
}

void PhysicsSystem::ExecuteRangeTask(const uint32_t start, const uint32_t end, const uint32_t threadIndex,
                                     void* context)
{
    const auto data = static_cast<TaskData*>(context);
    data->box2dTask(start, end, threadIndex, data->box2dContext);
}

void* PhysicsSystem::EnqueueTask(b2TaskCallback* box2dTask, int itemCount, int minRange, void* box2dContext,
                                 void* userContext)
{
    return static_cast<PhysicsSystem*>(userContext)->EnqueueTaskImpl(box2dTask, itemCount, minRange, box2dContext);
}

void PhysicsSystem::FinishTask(void* userTask, void* userContext)
{
    static_cast<PhysicsSystem*>(userContext)->FinishTaskImpl(userTask);
}

void* PhysicsSystem::EnqueueTaskImpl(b2TaskCallback* box2dTask, int itemCount, int minRange, void* box2dContext)
{
    if (taskCount < e_maxTasks)
    {
        enkiTaskSet* task = tasks[taskCount];
        TaskData* data = taskData + taskCount;
        data->box2dTask = box2dTask;
        data->box2dContext = box2dContext;

        enkiParamsTaskSet params{};
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

void PhysicsSystem::FinishTaskImpl(void* userTask) const
{
    const auto task = static_cast<enkiTaskSet*>(userTask);
    enkiWaitForTaskSet(scheduler, task);
}
