#ifndef SCRIPTSYSTEM_H
#define SCRIPTSYSTEM_H

#include "../ECS/ECS.h"
#include "../Components/ScriptComponent.h"
#include "../Components/TransformComponent.h"
#include "../Components/RigidBodyComponent.h"
#include "../Components/AnimationComponent.h"
#include "../Components/ProjectileEmitterComponent.h"
#include <tuple>

std::tuple<double, double> FetchEntityPosition(Entity entity) {
    if (entity.HasComponent<TransformComponent>()) {
        const auto transform = entity.GetComponent<TransformComponent>();
        return std::make_tuple(transform.position.x, transform.position.y);
    } else {
        Logger::Err("Entity has no TransformComponent - cannot fetch position");
        return std::make_tuple(0.0, 0.0);
    }
}

std::tuple<double, double> FetchEntityVelocity(Entity entity) {
    if (entity.HasComponent<RigidBodyComponent>()) {
        const auto rigidbody = entity.GetComponent<RigidBodyComponent>();
        return std::make_tuple(rigidbody.velocity.x, rigidbody.velocity.y);
    } else {
        Logger::Err("Entity has no RigidBodyComponent - cannot fetch velocity");
        return std::make_tuple(0.0, 0.0);
    }
}

void UpdateEntityPosition(Entity entity, double x, double y) {
    if (entity.HasComponent<TransformComponent>()) {
        auto& transform = entity.GetComponent<TransformComponent>();
        transform.position.x = x;
        transform.position.y = y;
    } else {
        Logger::Err("Entity has no TransformComponent - cannot update position");
    }
}

void UpdateEntityVelocity(Entity entity, double x, double y) {
    if (entity.HasComponent<RigidBodyComponent>()) {
        auto& rigidbody = entity.GetComponent<RigidBodyComponent>();
        rigidbody.velocity.x = x;
        rigidbody.velocity.y = y;
    } else {
        Logger::Err("Entity has no RigidBodyComponent - cannot update velocity");
    }
}

void UpdateEntityRotation(Entity entity, double angle) {
    if (entity.HasComponent<TransformComponent>()) {
        auto& transform = entity.GetComponent<TransformComponent>();
        transform.rotation = angle;
    } else {
        Logger::Err("Entity has no TransformComponent - cannot update rotation");
    }
}

void UpdateEntityAnimationFrame(Entity entity, int frame) {
    if (entity.HasComponent<AnimationComponent>()) {
        auto& animation = entity.GetComponent<AnimationComponent>();
        animation.currentFrame = frame;
    } else {
        Logger::Err("Entity has no AnimationComponent - cannot update animation frame");
    }
}

void UpdateProjectileVelocity(Entity entity, double x, double y) {
    if (entity.HasComponent<ProjectileEmitterComponent>()) {
        auto& projectileEmitter = entity.GetComponent<ProjectileEmitterComponent>();
        projectileEmitter.projectileVelocity.x = x;
        projectileEmitter.projectileVelocity.y = y;
    } else {
        Logger::Err("Entity has no ProjectileEmitterComponent - cannot update projectile velocity");
    }
}

class ScriptSystem: public System {
    public:
        ScriptSystem() {
            RequireComponent<ScriptComponent>();
        }

        void CreateLuaBindings(sol::state& lua) {
            // Register the Entity type so Lua scripts can reference entities
            lua.new_usertype<Entity>(
                "entity",
                "get_id", &Entity::GetId,
                "destroy", &Entity::Kill,
                "has_tag", &Entity::HasTag,
                "belongs_to_group", &Entity::BelongsToGroup
            );

            // Expose C++ helper functions to Lua
            lua.set_function("get_position", FetchEntityPosition);
            lua.set_function("get_velocity", FetchEntityVelocity);
            lua.set_function("set_position", UpdateEntityPosition);
            lua.set_function("set_velocity", UpdateEntityVelocity);
            lua.set_function("set_rotation", UpdateEntityRotation);
            lua.set_function("set_projectile_velocity", UpdateProjectileVelocity);
            lua.set_function("set_animation_frame", UpdateEntityAnimationFrame);
        }

        void Update(double deltaTime, int elapsedTime) {
            // Run each entity's Lua script function this frame
            for (auto entity: GetSystemEntities()) {
                const auto script = entity.GetComponent<ScriptComponent>();
                script.func(entity, deltaTime, elapsedTime);
            }
        }
};

#endif
