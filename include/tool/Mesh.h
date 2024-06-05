#pragma once
#include <vector>
#include <glm/glm.hpp>
#include <tool/Shader.h>

#define MAX_BONE_INFLUENCE 4

struct Vertex
{
    glm::vec3 Position;
    glm::vec3 Normal;
    glm::vec2 TexCoord;
    glm::vec3 Tangent;
    glm::vec3 Bitangent;

    // 影响该顶点的骨索引
    int BoneIDs[MAX_BONE_INFLUENCE];
    // 骨权重
    float Weights[MAX_BONE_INFLUENCE];
};

struct Texture
{
    unsigned int ID;
    std::string Type;
    // 我们储存纹理的路径用于与其它纹理进行比较
    std::string Path;
};

class Mesh
{
public:
    std::vector<Vertex> Vertices;
    std::vector<unsigned int> Indices;
    std::vector<Texture> Textures;

    Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices, std::vector<Texture> textures)
    {
        Vertices = vertices;
        Indices = indices;
        Textures = textures;

        SetupMesh();
    }

    // 绘制
    void Draw(Shader shader)
    {
        unsigned int diffuseNr = 1;
        unsigned int specularNr = 1;
        unsigned int normalNr = 1;
        unsigned int heightNr = 1;
        for (unsigned int i = 0; i < Textures.size(); i++)
        {
            // 在绑定之前激活纹理单元
            glActiveTexture(GL_TEXTURE0 + i);
            // 获取纹理序号（uTextureDiffuseN 中的 N）
            std::string number;
            std::string name = Textures[i].Type;
            if (name == "TextureDiffuse")
                number = std::to_string(diffuseNr++);
            else if (name == "TextureSpecular")
                number = std::to_string(specularNr++);
            else if (name == "TextureNormal")
                number = std::to_string(normalNr++);
            else if (name == "TextureHeight")
                number = std::to_string(heightNr++);

            shader.SetInt(("uMaterial." + name + number).c_str(), i);
            std::cout << ("uMaterial." + name + number) << std::endl;
            
            glBindTexture(GL_TEXTURE_2D, Textures[i].ID);
        }

        // 绘制网格
        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, static_cast<unsigned int>(Indices.size()), GL_UNSIGNED_INT, (void*)0);
        glBindVertexArray(0u);

        glActiveTexture(GL_TEXTURE0);
    }

    unsigned int VAO, VBO, EBO;

private:
    // 设置网格
    void SetupMesh()
    {
        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);
        glGenBuffers(1, &EBO);

        glBindVertexArray(VAO);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, Vertices.size() * sizeof(Vertex), &Vertices[0], GL_STATIC_DRAW);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, Indices.size() * sizeof(unsigned int), &Indices[0], GL_STATIC_DRAW);

        // 顶点位置
        glEnableVertexAttribArray(0u);
        glVertexAttribPointer(0u, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
        // 顶点法线
        glEnableVertexAttribArray(1u);
        glVertexAttribPointer(1u, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Normal));
        // 顶点纹理
        glEnableVertexAttribArray(2u);
        glVertexAttribPointer(2u, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, TexCoord));
        // 顶点切线
        glEnableVertexAttribArray(3u);
        glVertexAttribPointer(3u, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Tangent));
        // vertex bitangent
        glEnableVertexAttribArray(4u);
        glVertexAttribPointer(4u, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Bitangent));
        // ids
        glEnableVertexAttribArray(5u);
        glVertexAttribPointer(5u, 4, GL_INT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, BoneIDs));
        // weights
        glEnableVertexAttribArray(6u);
        glVertexAttribPointer(6u, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Weights));

        glBindVertexArray(0u);
    }
};