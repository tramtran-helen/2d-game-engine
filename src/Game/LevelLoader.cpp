#include "./LevelLoader.h"
#include "./Game.h"
#include "../Components/TransformComponent.h"
#include "../Components/RigidBodyComponent.h"
#include "../Components/SpriteComponent.h"
#include "../Components/AnimationComponent.h"
#include "../Components/BoxColliderComponent.h"
#include "../Components/KeyboardControlledComponent.h"
#include "../Components/CameraFollowComponent.h"
#include "../Components/ProjectileEmitterComponent.h"
#include "../Components/HealthComponent.h"
#include "../Components/TextLabelComponent.h"
#include "../Components/ScriptComponent.h"
#include <fstream>
#include <string>
#include <sol/sol.hpp>

LevelLoader::LevelLoader() {
    Logger::Log("LevelLoader constructor called!");
}

LevelLoader::~LevelLoader() {
    Logger::Log("LevelLoader destructor called!");
}

void LevelLoader::LoadLevel(sol::state& lua, const std::unique_ptr<Registry>& registry, const std::unique_ptr<AssetStore>& assetStore, SDL_Renderer* renderer, int levelNumber) {
    // Validate the Lua script syntax before executing
    sol::load_result script = lua.load_file("./assets/scripts/Level" + std::to_string(levelNumber) + ".lua");
    if (!script.valid()) {
        sol::error err = script;
        std::string errorMessage = err.what();
        Logger::Err("Failed to load Lua script: " + errorMessage);
        return;
    }

    // Execute the Lua script to populate the Level table
    lua.script_file("./assets/scripts/Level" + std::to_string(levelNumber) + ".lua");

    // Grab the top-level Level table from Lua
    sol::table level = lua["Level"];

    ////////////////////////////////////////////////////////////////////////////
    // Load assets defined in the Lua level file
    ////////////////////////////////////////////////////////////////////////////
    sol::table assets = level["assets"];

    int index = 0;
    while (true) {
        sol::optional<sol::table> hasAsset = assets[index];
        if (hasAsset == sol::nullopt) {
            break;
        }
        sol::table asset = assets[index];
        std::string assetType = asset["type"];
        std::string assetId = asset["id"];
        if (assetType == "texture") {
            assetStore->AddTexture(renderer, assetId, asset["file"]);
            Logger::Log("Texture loaded into asset store with id: " + assetId);
        }
        if (assetType == "font") {
            assetStore->AddFont(assetId, asset["file"], asset["font_size"]);
            Logger::Log("Font loaded into asset store with id: " + assetId);
        }
        index++;
    }

    ////////////////////////////////////////////////////////////////////////////
    // Load tilemap configuration from the Lua level file
    ////////////////////////////////////////////////////////////////////////////
    sol::table map = level["tilemap"];
    std::string mapFilePath = map["map_file"];
    std::string mapTextureAssetId = map["texture_asset_id"];
    int mapNumRows = map["num_rows"];
    int mapNumCols = map["num_cols"];
    int tileSize = map["tile_size"];
    double mapScale = map["scale"];
    std::fstream mapFile;
    mapFile.open(mapFilePath);
    for (int row = 0; row < mapNumRows; row++) {
        for (int col = 0; col < mapNumCols; col++) {
            char ch;
            mapFile.get(ch);
            int srcRectY = std::atoi(&ch) * tileSize;
            mapFile.get(ch);
            int srcRectX = std::atoi(&ch) * tileSize;
            mapFile.ignore();

            Entity tile = registry->CreateEntity();
            tile.AddComponent<TransformComponent>(glm::vec2(col * (mapScale * tileSize), row * (mapScale * tileSize)), glm::vec2(mapScale, mapScale), 0.0);
            tile.AddComponent<SpriteComponent>(mapTextureAssetId, tileSize, tileSize, 0, false, srcRectX, srcRectY);
        }
    }
    mapFile.close();
    Game::mapWidth = mapNumCols * tileSize * mapScale;
    Game::mapHeight = mapNumRows * tileSize * mapScale;

    ////////////////////////////////////////////////////////////////////////////
    // Spawn all entities defined in the Lua level file
    ////////////////////////////////////////////////////////////////////////////
    sol::table entities = level["entities"];
    index = 0;
    while (true) {
        sol::optional<sol::table> hasEntity = entities[index];
        if (hasEntity == sol::nullopt) {
            break;
        }

        sol::table entityData = entities[index];
        Entity createdEntity = registry->CreateEntity();

        // Tag
        sol::optional<std::string> tag = entityData["tag"];
        if (tag != sol::nullopt) {
            createdEntity.Tag(entityData["tag"]);
        }

        // Group
        sol::optional<std::string> group = entityData["group"];
        if (group != sol::nullopt) {
            createdEntity.Group(entityData["group"]);
        }

        // Components
        sol::optional<sol::table> hasComponents = entityData["components"];
        if (hasComponents != sol::nullopt) {
            // Transform
            sol::optional<sol::table> transform = entityData["components"]["transform"];
            if (transform != sol::nullopt) {
                createdEntity.AddComponent<TransformComponent>(
                    glm::vec2(
                        entityData["components"]["transform"]["position"]["x"],
                        entityData["components"]["transform"]["position"]["y"]
                    ),
                    glm::vec2(
                        entityData["components"]["transform"]["scale"]["x"].get_or(1.0),
                        entityData["components"]["transform"]["scale"]["y"].get_or(1.0)
                    ),
                    entityData["components"]["transform"]["rotation"].get_or(0.0)
                );
            }

            // RigidBody
            sol::optional<sol::table> rigidbody = entityData["components"]["rigidbody"];
            if (rigidbody != sol::nullopt) {
                createdEntity.AddComponent<RigidBodyComponent>(
                    glm::vec2(
                        entityData["components"]["rigidbody"]["velocity"]["x"].get_or(0.0),
                        entityData["components"]["rigidbody"]["velocity"]["y"].get_or(0.0)
                    )
                );
            }

            // Sprite
            sol::optional<sol::table> sprite = entityData["components"]["sprite"];
            if (sprite != sol::nullopt) {
                createdEntity.AddComponent<SpriteComponent>(
                    entityData["components"]["sprite"]["texture_asset_id"],
                    entityData["components"]["sprite"]["width"],
                    entityData["components"]["sprite"]["height"],
                    entityData["components"]["sprite"]["z_index"].get_or(1),
                    entityData["components"]["sprite"]["fixed"].get_or(false),
                    entityData["components"]["sprite"]["src_rect_x"].get_or(0),
                    entityData["components"]["sprite"]["src_rect_y"].get_or(0)
                );
            }

            // Animation
            sol::optional<sol::table> animation = entityData["components"]["animation"];
            if (animation != sol::nullopt) {
                createdEntity.AddComponent<AnimationComponent>(
                    entityData["components"]["animation"]["num_frames"].get_or(1),
                    entityData["components"]["animation"]["speed_rate"].get_or(1)
                );
            }

            // BoxCollider
            sol::optional<sol::table> collider = entityData["components"]["boxcollider"];
            if (collider != sol::nullopt) {
                createdEntity.AddComponent<BoxColliderComponent>(
                    entityData["components"]["boxcollider"]["width"],
                    entityData["components"]["boxcollider"]["height"],
                    glm::vec2(
                        entityData["components"]["boxcollider"]["offset"]["x"].get_or(0),
                        entityData["components"]["boxcollider"]["offset"]["y"].get_or(0)
                    )
                );
            }

            // Health
            sol::optional<sol::table> health = entityData["components"]["health"];
            if (health != sol::nullopt) {
                createdEntity.AddComponent<HealthComponent>(
                    static_cast<int>(entityData["components"]["health"]["health_percentage"].get_or(100))
                );
            }

            // ProjectileEmitter
            sol::optional<sol::table> projectileEmitter = entityData["components"]["projectile_emitter"];
            if (projectileEmitter != sol::nullopt) {
                createdEntity.AddComponent<ProjectileEmitterComponent>(
                    glm::vec2(
                        entityData["components"]["projectile_emitter"]["projectile_velocity"]["x"],
                        entityData["components"]["projectile_emitter"]["projectile_velocity"]["y"]
                    ),
                    static_cast<int>(entityData["components"]["projectile_emitter"]["repeat_frequency"].get_or(1)) * 1000,
                    static_cast<int>(entityData["components"]["projectile_emitter"]["projectile_duration"].get_or(10)) * 1000,
                    static_cast<int>(entityData["components"]["projectile_emitter"]["hit_percentage_damage"].get_or(10)),
                    entityData["components"]["projectile_emitter"]["friendly"].get_or(false)
                );
            }

            // CameraFollow
            sol::optional<sol::table> cameraFollow = entityData["components"]["camera_follow"];
            if (cameraFollow != sol::nullopt) {
                createdEntity.AddComponent<CameraFollowComponent>();
            }

            // KeyboardControlled
            sol::optional<sol::table> keyboardControlled = entityData["components"]["keyboard_controller"];
            if (keyboardControlled != sol::nullopt) {
                createdEntity.AddComponent<KeyboardControlledComponent>(
                    glm::vec2(
                        entityData["components"]["keyboard_controller"]["up_velocity"]["x"],
                        entityData["components"]["keyboard_controller"]["up_velocity"]["y"]
                    ),
                    glm::vec2(
                        entityData["components"]["keyboard_controller"]["right_velocity"]["x"],
                        entityData["components"]["keyboard_controller"]["right_velocity"]["y"]
                    ),
                    glm::vec2(
                        entityData["components"]["keyboard_controller"]["down_velocity"]["x"],
                        entityData["components"]["keyboard_controller"]["down_velocity"]["y"]
                    ),
                    glm::vec2(
                        entityData["components"]["keyboard_controller"]["left_velocity"]["x"],
                        entityData["components"]["keyboard_controller"]["left_velocity"]["y"]
                    )
                );
            }

            // Script
            sol::optional<sol::table> script = entityData["components"]["on_update_script"];
            if (script != sol::nullopt) {
                sol::function func = entityData["components"]["on_update_script"][0];
                createdEntity.AddComponent<ScriptComponent>(func);
            }
        }
        index++;
    }
}
