#ifndef RENDERSYSTEM_H
#define RENDERSYSTEM_H

#include "../ECS/ECS.h"
#include "../Components/TransformComponent.h"
#include "../Components/SpriteComponent.h"
#include "../AssetStore/AssetStore.h"
#include <SDL.h>
#include <algorithm>
#include <memory>
#include <vector>

class RenderSystem: public System {
    public:
        RenderSystem() {
            RequireComponent<TransformComponent>();
            RequireComponent<SpriteComponent>();
        }

        void Update(SDL_Renderer* renderer, std::unique_ptr<AssetStore>& assetStore) {
            // Create a vector with both Sprite and Transform component of all entities
            struct RenderableEntity {
                TransformComponent transformComponent;
                SpriteComponent spriteComponent;
            };

            std::vector<RenderableEntity> renderableEntities;
            for (auto entity : GetSystemEntities()) {
                RenderableEntity renderableEntity;
                renderableEntity.spriteComponent = entity.GetComponent<SpriteComponent>();
                renderableEntity.transformComponent = entity.GetComponent<TransformComponent>();
                renderableEntities.emplace_back(renderableEntity);
            }

            // Sort the vector by the z-index value
            std::sort(renderableEntities.begin(), renderableEntities.end(), [](const RenderableEntity& a, const RenderableEntity& b) {
                return a.spriteComponent.zIndex < b.spriteComponent.zIndex;
            });
            
            // Loop all entities that the system is interested in
            for (const auto& entity: renderableEntities) {
                // Set the source rectangle of our original sprite texture
                SDL_Rect srcRect = entity.spriteComponent.srcRect;

                // Set the destination rectangle with the x, y position to be rendered
                SDL_Rect dstRect = {
                    static_cast<int>(entity.transformComponent.position.x),
                    static_cast<int>(entity.transformComponent.position.y),
                    static_cast<int>(entity.spriteComponent.width * entity.transformComponent.scale.x),
                    static_cast<int>(entity.spriteComponent.height * entity.transformComponent.scale.y)
                };

                SDL_RenderCopyEx(
                    renderer,
                    assetStore->GetTexture(entity.spriteComponent.assetId),
                    &srcRect,
                    &dstRect,
                    entity.transformComponent.rotation,
                    nullptr,
                    SDL_FLIP_NONE
                );
            }
        }
};

#endif
