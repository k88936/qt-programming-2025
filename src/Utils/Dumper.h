//
// Created by kvtoD on 7/1/2025.
//

#ifndef DUMPER_H
#define DUMPER_H
#include "../World.h"


inline std::ostream &operator <<(std::ostream &os, const entt::entity &entity) {
    return os << static_cast<std::uintptr_t>(entity);
}

// Helper to check if a type has operator<< defined
template<typename T>
concept Printable = requires(std::ostream &os, const T &t)
{
    os << t;
};

// Helper function to safely print a component
template<typename T>
std::string componentToString(const T &component) {
    if constexpr (Printable<T>) {
        std::ostringstream oss;
        oss << component;
        return oss.str();
    } else {
        return "[Component of type " + std::string(typeid(T).name()) + "]";
    }
}

template<typename T>
inline void dump() {
    World::getInstance().registry.view<const T>().each([](const auto entity, const auto &component) {
        std::cout << entity << " " << componentToString(component) << std::endl;
    });
}

inline void dump(entt::entity entity) {
    auto &registry = World::getInstance().registry;
    // Iterate through all storage types in the registry
    for (auto [id, storage] : registry.storage()) {
        if (storage.contains(entity)) {
            // This is tricky because we don't know the component type at compile time
            // You'd need to maintain a type registry or use reflection
            std::cout << entity << " [Component type: " << id << "]" << std::endl;
        }
    }
}

#endif //DUMPER_H