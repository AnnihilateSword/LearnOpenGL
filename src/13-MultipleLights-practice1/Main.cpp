#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <tool/Shader.h>
#include <tool/Camera.h>
#define STB_IMAGE_IMPLEMENTATION
#include <tool/stb_image.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

const int SCREEN_WIDTH = 800;
const int SCREEN_HEIGHT = 600;

// Camera
Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));
float LastX{};
float LastY{};
bool bFirstMouse{true};

// Delta Time
float DeltaTime{};
float LastFrame{};

// Light
glm::vec3 LightPos{1.2f, 1.0f, 2.0f};

void FramebufferSizeCallback(GLFWwindow* window, int width, int height)
{
    glViewport(0u, 0u, width, height);
}

void ProcessInput(GLFWwindow* window)
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

void CursorPosCallback(GLFWwindow* window, double xpos, double ypos)
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

void ScrollCallback(GLFWwindow* window, double xoffset, double yoffset)
{
    camera.ProcessMouseScroll(yoffset);
}

// 加载纹理
unsigned int LoadTexture(const char* path)
{
    unsigned int textureID;
    glGenTextures(1, &textureID);

    int width, height, nrChannels;
    unsigned char* data = stbi_load(path, &width, &height, &nrChannels, 0);
    if (data != nullptr)
    {
        unsigned int format;
        if (nrChannels == 1)
            format = GL_RED;
        if (nrChannels == 3)
            format = GL_RGB;
        if (nrChannels == 4)
            format = GL_RGBA;

        glBindTexture(GL_TEXTURE_2D, textureID);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        stbi_set_flip_vertically_on_load(true);

        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else
    {
        std::cout << "[TEXTURE LOAD ERROR]: Failed to load texture at path: " << path << std::endl;
    }
    stbi_image_free(data);

    return textureID;
}

int main(int argc, char** argv)
{
    // glfw and glad initialize
    if (!glfwInit())
        return -1;
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "LearnOpenGL", nullptr, nullptr);
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
    glEnable(GL_DEPTH_TEST);

    // 1. shader
    Shader lightingShader("./src/13-MultipleLights-practice1/Shaders/VertexShader.glsl", "./src/13-MultipleLights-practice1/Shaders/FragmentShader.glsl");
    Shader lightCubeShader("./src/13-MultipleLights-practice1/Shaders/LightCubeVertexShader.glsl", "./src/13-MultipleLights-practice1/Shaders/LightCubeFragmentShader.glsl");

    // 2. vertices data
    float vertices[] =
    {
        // positions          // normals           // texture coords
        -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 0.0f,
         0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 0.0f,
         0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 1.0f,
         0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 1.0f,
        -0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 0.0f,

        -0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   0.0f, 0.0f,
         0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   1.0f, 0.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   1.0f, 1.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   1.0f, 1.0f,
        -0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   0.0f, 1.0f,
        -0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   0.0f, 0.0f,

        -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f, 0.0f,
        -0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  1.0f, 1.0f,
        -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
        -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
        -0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  0.0f, 0.0f,
        -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f, 0.0f,

         0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f,
         0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f,
         0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
         0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
         0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 0.0f,
         0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f,

        -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 1.0f,
         0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 1.0f,
         0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 0.0f,
         0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 0.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 0.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 1.0f,

        -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 1.0f,
         0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 1.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 0.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 0.0f,
        -0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 0.0f,
        -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 1.0f
    };

    glm::vec3 cubePositions[] =
    {
        glm::vec3( 0.0f,  0.0f,  0.0f),
        glm::vec3( 2.0f,  5.0f, -15.0f),
        glm::vec3(-1.5f, -2.2f, -2.5f),
        glm::vec3(-3.8f, -2.0f, -12.3f),
        glm::vec3( 2.4f, -0.4f, -3.5f),
        glm::vec3(-1.7f,  3.0f, -7.5f),
        glm::vec3( 1.3f, -2.0f, -2.5f),
        glm::vec3( 1.5f,  2.0f, -2.5f),
        glm::vec3( 1.5f,  0.2f, -1.5f),
        glm::vec3(-1.3f,  1.0f, -1.5f)
    };

    // positions of the point lights
    glm::vec3 pointLightPositions[] =
    {
        glm::vec3( 0.7f,  0.2f,  2.0f),
        glm::vec3( 2.3f, -3.3f, -4.0f),
        glm::vec3(-4.0f,  2.0f, -12.0f),
        glm::vec3( 0.0f,  0.0f, -3.0f)
    };

    glm::vec3 pointLightColors[] =
    {
        glm::vec3(0.2f, 0.2f, 0.6f),
        glm::vec3(0.3f, 0.3f, 0.7f),
        glm::vec3(0.0f, 0.0f, 0.3f),
        glm::vec3(0.4f, 0.4f, 0.4f)
    };

    unsigned int cubeVao, vbo;
    glGenVertexArrays(1, &cubeVao);
    glGenBuffers(1, &vbo);
    glBindVertexArray(cubeVao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0u, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0u);
    glVertexAttribPointer(1u, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1u);
    glVertexAttribPointer(2u, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2u);

    glBindBuffer(GL_ARRAY_BUFFER, 0u);
    glBindVertexArray(0u);

    unsigned int lightCubeVao;
    glGenVertexArrays(1, &lightCubeVao);
    glBindVertexArray(lightCubeVao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glVertexAttribPointer(0u, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0u);
    glBindVertexArray(0u);

    // 3. texture
    // diffuse map & specular map
    unsigned int diffuseMap;
    unsigned int specularMap;
    diffuseMap = LoadTexture("./res/textures/container2.png");
    specularMap = LoadTexture("./res/textures/container2_specular.png");
    
    // 4. render loop
    while (!glfwWindowShouldClose(window))
    {
        float CurrentTime = static_cast<float>(glfwGetTime());
        DeltaTime = CurrentTime - LastFrame;
        LastFrame = CurrentTime;

        ProcessInput(window);

        // render
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // draw cube object
        lightingShader.Use();
        // active texture unit & bind texture
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, diffuseMap);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, specularMap);
        // set object material
        // 设置采样器对应哪个纹理单元（漫反射贴图 & 镜面反射贴图）
        lightingShader.SetInt("uMaterial.diffuse", 0);
        lightingShader.SetInt("uMaterial.specular", 1);
        lightingShader.SetFloat("uMaterial.shininess", 32.0f);

        /*
           Here we set all the uniforms for the 5/6 types of lights we have. We have to set them manually and index 
           the proper PointLight struct in the array to set each uniform variable. This can be done more code-friendly
           by defining light types as classes and set their values in there, or by using a more efficient uniform approach
           by using 'Uniform buffer objects', but that is something we'll discuss in the 'Advanced GLSL' tutorial.
        */
        // Directional light
        glUniform3f(glGetUniformLocation(lightingShader.GetID(), "uDirLight.direction"), -0.2f, -1.0f, -0.3f);		
        glUniform3f(glGetUniformLocation(lightingShader.GetID(), "uDirLight.ambient"), 0.05f, 0.05f, 0.1f);		
        glUniform3f(glGetUniformLocation(lightingShader.GetID(), "uDirLight.diffuse"), 0.2f, 0.2f, 0.7); 
        glUniform3f(glGetUniformLocation(lightingShader.GetID(), "uDirLight.specular"), 0.7f, 0.7f, 0.7f);
        // Point light 1
        glUniform3f(glGetUniformLocation(lightingShader.GetID(), "uPointLights[0].position"), pointLightPositions[0].x, pointLightPositions[0].y, pointLightPositions[0].z);		
        glUniform3f(glGetUniformLocation(lightingShader.GetID(), "uPointLights[0].ambient"), pointLightColors[0].x * 0.1,  pointLightColors[0].y * 0.1,  pointLightColors[0].z * 0.1);		
        glUniform3f(glGetUniformLocation(lightingShader.GetID(), "uPointLights[0].diffuse"), pointLightColors[0].x,  pointLightColors[0].y,  pointLightColors[0].z); 
        glUniform3f(glGetUniformLocation(lightingShader.GetID(), "uPointLights[0].specular"), pointLightColors[0].x,  pointLightColors[0].y,  pointLightColors[0].z);
        glUniform1f(glGetUniformLocation(lightingShader.GetID(), "uPointLights[0].constant"), 1.0f);
        glUniform1f(glGetUniformLocation(lightingShader.GetID(), "uPointLights[0].linear"), 0.09);
        glUniform1f(glGetUniformLocation(lightingShader.GetID(), "uPointLights[0].quadratic"), 0.032);		
        // Point light 2
        glUniform3f(glGetUniformLocation(lightingShader.GetID(), "uPointLights[1].position"), pointLightPositions[1].x, pointLightPositions[1].y, pointLightPositions[1].z);		
        glUniform3f(glGetUniformLocation(lightingShader.GetID(), "uPointLights[1].ambient"), pointLightColors[1].x * 0.1,  pointLightColors[1].y * 0.1,  pointLightColors[1].z * 0.1);		
        glUniform3f(glGetUniformLocation(lightingShader.GetID(), "uPointLights[1].diffuse"), pointLightColors[1].x,  pointLightColors[1].y,  pointLightColors[1].z); 
        glUniform3f(glGetUniformLocation(lightingShader.GetID(), "uPointLights[1].specular"), pointLightColors[1].x,  pointLightColors[1].y,  pointLightColors[1].z);
        glUniform1f(glGetUniformLocation(lightingShader.GetID(), "uPointLights[1].constant"), 1.0f);
        glUniform1f(glGetUniformLocation(lightingShader.GetID(), "uPointLights[1].linear"), 0.09);
        glUniform1f(glGetUniformLocation(lightingShader.GetID(), "uPointLights[1].quadratic"), 0.032);		
        // Point light 3
        glUniform3f(glGetUniformLocation(lightingShader.GetID(), "uPointLights[2].position"), pointLightPositions[2].x, pointLightPositions[2].y, pointLightPositions[2].z);		
        glUniform3f(glGetUniformLocation(lightingShader.GetID(), "uPointLights[2].ambient"), pointLightColors[2].x * 0.1,  pointLightColors[2].y * 0.1,  pointLightColors[2].z * 0.1);		
        glUniform3f(glGetUniformLocation(lightingShader.GetID(), "uPointLights[2].diffuse"), pointLightColors[2].x,  pointLightColors[2].y,  pointLightColors[2].z); 
        glUniform3f(glGetUniformLocation(lightingShader.GetID(), "uPointLights[2].specular") ,pointLightColors[2].x,  pointLightColors[2].y,  pointLightColors[2].z);
        glUniform1f(glGetUniformLocation(lightingShader.GetID(), "uPointLights[2].constant"), 1.0f);
        glUniform1f(glGetUniformLocation(lightingShader.GetID(), "uPointLights[2].linear"), 0.09);
        glUniform1f(glGetUniformLocation(lightingShader.GetID(), "uPointLights[2].quadratic"), 0.032);		
        // Point light 4
        glUniform3f(glGetUniformLocation(lightingShader.GetID(), "uPointLights[3].position"), pointLightPositions[3].x, pointLightPositions[3].y, pointLightPositions[3].z);		
        glUniform3f(glGetUniformLocation(lightingShader.GetID(), "uPointLights[3].ambient"), pointLightColors[3].x * 0.1,  pointLightColors[3].y * 0.1,  pointLightColors[3].z * 0.1);		
        glUniform3f(glGetUniformLocation(lightingShader.GetID(), "uPointLights[3].diffuse"), pointLightColors[3].x,  pointLightColors[3].y,  pointLightColors[3].z); 
        glUniform3f(glGetUniformLocation(lightingShader.GetID(), "uPointLights[3].specular"), pointLightColors[3].x,  pointLightColors[3].y,  pointLightColors[3].z);
        glUniform1f(glGetUniformLocation(lightingShader.GetID(), "uPointLights[3].constant"), 1.0f);
        glUniform1f(glGetUniformLocation(lightingShader.GetID(), "uPointLights[3].linear"), 0.09);
        glUniform1f(glGetUniformLocation(lightingShader.GetID(), "uPointLights[3].quadratic"), 0.032);		
        // uSpotLight
        glUniform3f(glGetUniformLocation(lightingShader.GetID(), "uSpotLight.position"), camera.Position.x, camera.Position.y, camera.Position.z);	
        glUniform3f(glGetUniformLocation(lightingShader.GetID(), "uSpotLight.direction"), camera.Front.x, camera.Front.y, camera.Front.z);
        glUniform3f(glGetUniformLocation(lightingShader.GetID(), "uSpotLight.ambient"), 0.0f, 0.0f, 0.0f);		
        glUniform3f(glGetUniformLocation(lightingShader.GetID(), "uSpotLight.diffuse"), 1.0f, 1.0f, 1.0f); 
        glUniform3f(glGetUniformLocation(lightingShader.GetID(), "uSpotLight.specular"), 1.0f, 1.0f, 1.0f);
        glUniform1f(glGetUniformLocation(lightingShader.GetID(), "uSpotLight.constant"), 1.0f);
        glUniform1f(glGetUniformLocation(lightingShader.GetID(), "uSpotLight.linear"), 0.009);
        glUniform1f(glGetUniformLocation(lightingShader.GetID(), "uSpotLight.quadratic"), 0.0032);				
        glUniform1f(glGetUniformLocation(lightingShader.GetID(), "uSpotLight.cutOff"), glm::cos(glm::radians(10.0f)));
        glUniform1f(glGetUniformLocation(lightingShader.GetID(), "uSpotLight.outerCutOff"), glm::cos(glm::radians(12.5f)));
        
        // MVP
        glm::mat4 Model(1.0f);
        glm::mat4 View = camera.GetViewMatrix();
        glm::mat4 Projection = glm::perspective(glm::radians(camera.Fov), static_cast<float>(SCREEN_WIDTH)/SCREEN_HEIGHT, 0.1f, 100.0f);
        lightingShader.SetMat4f("uModel", Model);
        lightingShader.SetMat4f("uView", View);
        lightingShader.SetMat4f("uProjection", Projection);

        glBindVertexArray(cubeVao);
        for(unsigned int i = 0; i < 10; i++)
        {
            Model = glm::mat4(1.0f);
            Model = glm::translate(Model, cubePositions[i]);
            float angle = 20.0f * i;
            Model = glm::rotate(Model, glm::radians(angle), glm::vec3(1.0f, 0.3f, 0.5f));
            lightingShader.SetMat4f("uModel", Model);

            glDrawArrays(GL_TRIANGLES, 0, 36);
        }

        // draw light
        lightCubeShader.Use();
        // MVP
        lightCubeShader.SetMat4f("uView", View);
        lightCubeShader.SetMat4f("uProjection", Projection);
        glBindVertexArray(lightCubeVao);

        for (unsigned int i = 0; i < 4; i++)
        {
            Model = glm::mat4(1.0f);
            Model = glm::translate(Model, pointLightPositions[i]);
            Model = glm::scale(Model, glm::vec3(0.2f));
            lightCubeShader.SetMat4f("uModel", Model);
            glDrawArrays(GL_TRIANGLES, 0, 36);
        }

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // clear resources
    glDeleteBuffers(1, &vbo);
    glDeleteVertexArrays(1, &cubeVao);
    glDeleteVertexArrays(1, &lightCubeVao);
    lightingShader.DeleteShaderProgram();
    lightingShader.DeleteShaderProgram();

    glfwTerminate();
    return 0;
}