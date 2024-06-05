#include <iostream>
#include <map>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <tool/Shader.h>
#include <tool/Camera.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <tool/Model.h>

const int SCREEN_WIDTH = 1280;
const int SCREEN_HEIGHT = 720;

// Camera
Camera camera(glm::vec3(0.0f, 2.2f, 4.0f));
float LastX{};
float LastY{};
bool bFirstMouse{true};

// Delta Time
float DeltaTime{};
float LastFrame{};

// Light
glm::vec3 LightPos{1.2f, 1.0f, 2.0f};

void FramebufferSizeCallback(GLFWwindow *window, int width, int height)
{
    glViewport(0u, 0u, width, height);
}

void ProcessInput(GLFWwindow *window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        camera.ProcessKeyboard(FORWARD, DeltaTime);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        camera.ProcessKeyboard(BACKWARD, DeltaTime);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        camera.ProcessKeyboard(LEFT, DeltaTime);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        camera.ProcessKeyboard(RIGHT, DeltaTime);
    if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
        camera.ProcessKeyboard(UP, DeltaTime);
    if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
        camera.ProcessKeyboard(DOWN, DeltaTime);
}

void CursorPosCallback(GLFWwindow *window, double xpos, double ypos)
{
    if (bFirstMouse)
    {
        bFirstMouse = false;
        LastX = xpos;
        LastY = ypos;
    }
    float XOffset = xpos - LastX;
    float YOffset = ypos - LastY;
    LastX = xpos;
    LastY = ypos;

    camera.ProcessMouseMovement(XOffset, -YOffset);
}

void ScrollCallback(GLFWwindow *window, double xoffset, double yoffset)
{
    camera.ProcessMouseScroll(yoffset);
}

// 加载立方体贴图
unsigned int LoadCubemap(std::vector<std::string> &faces)
{
    unsigned int textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

    int width, height, nrChannels;
    for (unsigned int i = 0; i < faces.size(); i++)
    {
        unsigned char *data = stbi_load(faces[i].c_str(), &width, &height, &nrChannels, 0);
        if (data != nullptr)
        {
            unsigned int format;
            if (nrChannels == 1)
                format = GL_RED;
            else if (nrChannels == 3)
                format = GL_RGB;
            else if (nrChannels == 4)
                format = GL_RGBA;

            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, format, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
        }
        else
        {
            std::cout << "[Load Cubemap Error]: Failed to load Cubemap at path: " << faces[i] << std::endl;
        }
        stbi_image_free(data);
    }
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    return textureID;
}

int main(int argc, char **argv)
{
    // glfw and glad initialize
    if (!glfwInit())
        return -1;
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow *window = glfwCreateWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "LearnOpenGL", nullptr, nullptr);
    if (window == nullptr)
    {
        std::cout << "Failed to Create GLFW Widnow!" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to Create GLFW Widnow!" << std::endl;
        glfwTerminate();
        return -1;
    }

    glViewport(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);

    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    // glfw callback functions
    glfwSetFramebufferSizeCallback(window, FramebufferSizeCallback);
    glfwSetCursorPosCallback(window, CursorPosCallback);
    glfwSetScrollCallback(window, ScrollCallback);

    // config global opengl state
    // 1. 启用深度测试
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);

    // Shader
    Shader shader("./src/22-GeometryShader-2-Exploding/Shaders/geometry.vs", "./src/22-GeometryShader-2-Exploding/Shaders/geometry.fs",
                     "./src/22-GeometryShader-2-Exploding/Shaders/geometry.gs");

    // Model
    Model ourModel("./res/models/nanosuit_reflection/nanosuit.obj");

    // render loop
    while (!glfwWindowShouldClose(window))
    {
        float CurrentTime = static_cast<float>(glfwGetTime());
        DeltaTime = CurrentTime - LastFrame;
        LastFrame = CurrentTime;

        ProcessInput(window);

        // render
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // configure transformation matrices
        glm::mat4 projection = glm::perspective(glm::radians(45.0f), (float)SCREEN_WIDTH / (float)SCREEN_HEIGHT, 1.0f, 100.0f);
        glm::mat4 view = camera.GetViewMatrix();;
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::scale(model, glm::vec3(0.2f));
        shader.Use();
        shader.SetMat4f("projection", projection);
        shader.SetMat4f("view", view);
        shader.SetMat4f("model", model);

        // add time component to geometry shader in the form of a uniform
        shader.SetFloat("time", static_cast<float>(glfwGetTime()));

        // draw model
        ourModel.Draw(shader);

        // swap and poll events
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // clear resources

    glfwTerminate();
    return 0;
}