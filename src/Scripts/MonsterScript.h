//
// Created on 7/5/25.
//

#ifndef MONSTERSCRIPT_H
#define MONSTERSCRIPT_H
#include "Script.h"
#include "../Components/Input.h"
#include "../Components/Output.h"
#include "../Components/State.h"
#include "../Events/StateEvents.h"
#include <unordered_map>

class MonsterScript final : public Script<MonsterScript>
{
public:
    MonsterScript();
    ~MonsterScript();

    void update(entt::entity entity) override;

    // Event handling
    void handleGroundContact(const GroundContactEvent& event);

private:
    // Movement constants
    static constexpr float MOVE_FORCE = 15.0f;
    static constexpr float JUMP_IMPULSE = 20.0f;
    static constexpr float ATTACK_FORCE = 8.0f;

    // Track which entities are grounded
    std::unordered_map<entt::entity, bool> groundedEntities;

    // AI behavior parameters
    float detectionRange = 150.0f;
    float attackRange = 50.0f;

    // Helper methods
    bool isGrounded(entt::entity entity);
    void updateEntityState(entt::entity entity, State& state);
    void applyStateBasedOutput(const State& state, Output& output, entt::entity entity);
    void generateAIInput(entt::entity entity, Input& aiInput);
};
#endif //MONSTERSCRIPT_H
