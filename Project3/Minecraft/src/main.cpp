#include <glad/glad.h>

#include "Camera.hpp"
#include "Player.hpp"
#include "render/shader_program.hpp"
#include "render/texture_2d.hpp"
#include "window/cross_platform_window.hpp"

#include "world/world.hpp"
#include "world/particle.hpp"


static const std::string vertexSource =
    "#version 330 core\n"
    "\n"
    "layout (location = 0) in vec3 a_position;\n"
    "layout (location = 1) in vec3 a_texture_coordinates;\n" // Remmber that the 3rd element is the ao (ambient occlusion).
    "layout (location = 2) in vec3 a_tint;\n" // Remmber that the 3rd element is the ao (ambient occlusion).
    "\n"
    "out vec3 f_texture_coordinates;\n"
    "out vec3 f_tint;\n"
    "\n"
    "uniform mat4 u_projection;\n"
    "uniform mat4 u_view;\n"
    "\n"
    "void main() {\n"
    "   f_texture_coordinates = a_texture_coordinates;\n"
    "   f_tint = a_tint;\n"
    "   gl_Position = u_projection * u_view * vec4(a_position, 1);\n"
    "}";

static const std::string fragmentSource =
    "#version 330 core\n"
    "\n"
    "out vec4 frag_color;\n"
    "\n"
    "in vec3 f_texture_coordinates;\n"
    "in vec3 f_tint;\n"
    "\n"
    "uniform sampler2D u_texture_atlas;\n"
    "\n"
    "void main() {\n"
    "   vec4 texture_color = texture(u_texture_atlas, f_texture_coordinates.xy);\n"
    "   frag_color = vec4(texture_color.rgb * f_tint, 1);\n"
    "}";

static const std::string particleVertexSrouce =
    "#version 330 core\n"
    "\n"
    "layout (location = 0) in vec3 a_position;\n"
    "layout (location = 1) in float a_size;\n"
    "layout (location = 2) in mat4 instanceModel;\n"
    "\n"
    "uniform mat4 u_projection;\n"
    "uniform mat4 u_view;\n"
    "\n"
    "void main() {\n"
    "   gl_Position = u_projection * u_view * instanceModel * vec4(a_position, 1.0);"
    "}";

static const std::string particleFragmentSource =
    "#version 330 core\n"
    "\n"
    "out vec4 frag_color;\n"
    "\n"
    "uniform sampler2D tex;\n"
    "uniform vec2 u_particle_texture_coords;\n"
    "uniform vec3 u_particle_color;\n"
    "\n"
    "void main() {\n"
    "   frag_color = vec4(u_particle_color, 1);\n"
    "}";


int main() {
    CrossPlatformWindow window(800, 600, "Minecraft");
    window.createContext(3, 3);
    window.setContext();

    // Load OpenGL in this thread (needed).
    gladLoadGL();
    glEnable(GL_DEPTH_TEST); // Only need to enable once, no shadow pass.

    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);

    World world(3, 3);
    world.initChunks();

    // Rendering
    ShaderProgram shaderProgram;
    shaderProgram.load(vertexSource, fragmentSource);

    Texture2D texture("../assets/texture_atlas.png");

    // Player structs
    Camera camera(70, (float)window.getWidth() / (float)window.getHeight(), 0.1f, 1024.0f);
    camera.setPosition(0, 0, 0);
    camera.setRotation(0, 0);

    // camera.setPosition(60, 80, 60);
    Player player(camera, world, 0.1f);

    ShaderProgram particleShader;
    particleShader.load(particleVertexSrouce, particleFragmentSource);
    GLuint quadVAO = createQuadVAO();  // Predefined quad VAO for particle rendering

    ParticleSystem particleSystem;

    std::cout << "The controls: \n";
    std::cout << "WASD moves the player in the 4 spacial directions (x and z with direction accounted).\n";
    std::cout << "Hold middle mouse click and move the mouse to rotate the camera.\n";
    std::cout << "Left click to break blocks.\n";
    std::cout << "Right click to place blocks.\n";
    std::cout << "The block type can be changed with the 't' key on your keyboard.\n";
    std::cout << "Enjoy!\n";

    while (window.isWindowOpen()) {
        // Incase of resize.
        glViewport(0, 0, window.getWidth(), window.getHeight());
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glClearColor(0.4f, 0.5f, 0.6f, 1.0f);

        // Handle mouse and keyboard input for player.
        player.handleMouseInput(window);
        player.handleKeyboardInput(window);
        player.update(world, window, particleSystem);

        shaderProgram.use();
        shaderProgram.setUniformMatrix4("u_projection", camera.getProjectionMatrix().m);
        shaderProgram.setUniformMatrix4("u_view", camera.getViewMatrix().m);

        texture.bind(0);
        shaderProgram.setUniform("u_texture_atlas", 0);

        // Render the world (which handles chunks loading/unloading).
        world.render(camera.getPosition());

        particleShader.use();
        particleShader.setUniformMatrix4("u_projection", camera.getProjectionMatrix().m);
        particleShader.setUniformMatrix4("u_view", camera.getViewMatrix().m);

        Vec3f particleColor = {
            0.35f, 0.35f, 0.35f
        };
        particleShader.setUniform("u_particle_color", particleColor[0], particleColor[1], particleColor[2]);

        glDisable(GL_CULL_FACE);
        particleSystem.update(0.0167f);
        particleSystem.render(particleShader, quadVAO, camera.getPosition() + camera.getForward());
        glEnable(GL_CULL_FACE);

        // Poll events and swap buffers.
        window.pollEvents();
        window.swapBuffers();
    }

    shaderProgram.destroy();

    return 0;
}
