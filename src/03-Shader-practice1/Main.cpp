#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>

#include <tool/Shader.h>

const int SCREEN_WIDTH = 800;
const int SCREEN_HEIGHT = 600;

void FramebufferSizeCallback(GLFWwindow* window, int width, int height)
{
    glViewport(0u, 0u, width, height);
}

void ProcessInput(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
}

int main(int argc, char** argv)
{
    // initialize glfw and glad
    if (!glfwInit())
        return -1;
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "LearnOpenGL", nullptr, nullptr);
    if (window == nullptr)
    {
        std::cout << "Failed to Create GLFW window!" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to Initialize GLAD!" << std::endl;
        glfwTerminate();
        return -1;
    }

    // set viewport
    glViewport(0u, 0u, SCREEN_WIDTH, SCREEN_HEIGHT);
    // set glfw call back functions
    glfwSetFramebufferSizeCallback(window, FramebufferSizeCallback);

    // 1. vertices data (triangle & rectangle)
    // float vertices[] = {
    //     -0.5f, -0.5f, 0.0f,
    //      0.5f, -0.5f, 0.0f,
    //      0.0f,  0.5f, 0.0f
    // };
    float vertices[] = {
        // position         // color
        -0.5f,  0.5f, 0.0f, 1.0f, 0.0f, 0.0f,
         0.5f,  0.5f, 0.0f, 0.0f, 1.0f, 0.0f,
         0.0f, -0.5f, 0.0f, 0.0f, 0.0f, 1.0f
    };
    // 2. indices data (triangle & rectangle)
    // unsigned int indices[] = {
    //     0, 1, 2
    // };
    unsigned int indices[] = {
        0, 1, 2
    };

    // 3. set shader compile and sahder program link
    Shader shader("./src/03-Shader-practice1/Shaders/VertexShader.glsl", "./src/03-Shader-practice1/Shaders/FragmentShader.glsl");
    shader.Use();

    // 4. set buffer and attribute
    unsigned int vao, vbo, ebo;
    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);
    glGenBuffers(1, &ebo);

    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
    glVertexAttribPointer(0u, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 6, (void*)0);
    glEnableVertexAttribArray(0u);
    glVertexAttribPointer(1u, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 6, (void*)(sizeof(float) * 3));
    glEnableVertexAttribArray(1u);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    // render loop
    while (!glfwWindowShouldClose(window))
    {
        ProcessInput(window);

        // render
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        shader.Use();
        glBindVertexArray(vao);
        glDrawElements(GL_TRIANGLES, 3, GL_UNSIGNED_INT, indices);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // clear resources
    glDeleteBuffers(GL_ARRAY_BUFFER, &vbo);
    glDeleteBuffers(GL_ELEMENT_ARRAY_BUFFER, &ebo);
    glDeleteVertexArrays(1, &vao);
    shader.DeleteShaderProgram();

    glfwTerminate();
    return 0;
}