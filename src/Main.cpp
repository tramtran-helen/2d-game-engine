#include <iostream>

// SDL includes
#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>
#include <SDL_mixer.h>

// GLM for math
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

// Dear ImGui
#include <imgui.h>

// Sol2 for Lua
#include <sol/sol.hpp>

int main() {
    std::cout << "Starting library test...\n";

    // ---------------------
    // Test Lua via Sol2
    // ---------------------
    sol::state lua;
    lua.open_libraries(sol::lib::base);

    lua.script(R"(
        print("Hello from Lua!")
    )");

    // ---------------------
    // Test GLM
    // ---------------------
    glm::vec2 velocity(5.0f, -2.5f);
    velocity = glm::normalize(velocity);
    std::cout << "Normalized velocity: (" << velocity.x << ", " << velocity.y << ")\n";

    // ---------------------
    // Test SDL
    // ---------------------
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) != 0) {
        std::cerr << "SDL_Init Error: " << SDL_GetError() << "\n";
        return 1;
    }
    std::cout << "SDL initialized successfully.\n";

    // Quit SDL
    SDL_Quit();

    std::cout << "All dependencies work correctly!" << std::endl;
    return 0;
}