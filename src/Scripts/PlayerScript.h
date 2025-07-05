//
// Created by root on 7/5/25.
//

#ifndef PLAYERSCRIPT_H
#define PLAYERSCRIPT_H
#include "Script.h"
#include "../Components/Input.h"
#include "../Components/Output.h"
#include "../Components/State.h"
#include "../Events/StateEvents.h"
#include <unordered_map>

class PlayerScript final : public Script<PlayerScript>
{
public:
    PlayerScript();
    ~PlayerScript();

    void update(entt::entity entity) override;

    // Event handling
    void handleGroundContact(const GroundContactEvent& event);

private:
    // Movement constants
    static constexpr float MOVE_FORCE = 25.0f;
    static constexpr float JUMP_IMPULSE = 25.0f;
    static constexpr float ATTACK_FORCE = 10.0f;

    // Track which entities are grounded
    std::unordered_map<entt::entity, bool> groundedEntities;

    // Helper methods
    bool isGrounded(entt::entity entity);
    bool isMovingHorizontally(const Input& input);
    void updateEntityState(entt::entity entity, const Input& input, State& state);
    void applyStateBasedOutput(const State& state, const Input& input, Output& output, entt::entity entity);
};
#endif //PLAYERSCRIPT_H
