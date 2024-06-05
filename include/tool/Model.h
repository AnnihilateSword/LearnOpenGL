#pragma once
#include <tool/Mesh.h>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#define STB_IMAGE_IMPLEMENTATION
#include <tool/stb_image.h>

// 从文件中加载纹理 ID
unsigned int TextureFromFile(const char* path, const std::string& directory, bool isModel = true, bool gamma = false)
{
    std::string filename = std::string(path);
    filename = directory + '/' + filename;

    unsigned int textureID;
    glGenTextures(1, &textureID);

    int width, height, nrChannels;
    unsigned char* data = stbi_load(filename.c_str(), &width, &height, &nrChannels, 0);
    if (data != nullptr)
    {
        unsigned int internalFormat;
        unsigned int dataFormat;
        if (nrChannels == 1)
        {
            internalFormat = GL_RED;
            dataFormat = GL_RED;
        }
        else if (nrChannels == 3)
        {
            internalFormat = gamma ? GL_SRGB : GL_RGB;
            dataFormat = GL_RGB;
        }
        else if (nrChannels == 4)
        {
            internalFormat = gamma ? GL_SRGB_ALPHA : GL_RGBA;
            dataFormat = GL_RGBA;
        }

        glBindTexture(GL_TEXTURE_2D, textureID);
        // 如果是 rgba 设置过滤 gl_clamp_to_edge，不然会边缘插值，边缘会达不到效果
        // 这里我在main函数中加载纹理和加载模型中使用的函数都是这一个，需要处理一下，模型必须是 GL_REPEAT 否则可能只绘制半边纹理
        // 然后为了实现正确的混合效果，需要做 else 中的判断
        if (isModel)
        {
            glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
            glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        }
        else
        {
            glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, dataFormat == GL_RGBA ? GL_CLAMP_TO_EDGE : GL_REPEAT);
            glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, dataFormat == GL_RGBA ? GL_CLAMP_TO_EDGE : GL_REPEAT);
        }
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, width, height, 0, dataFormat, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else
    {
        std::cout << "[TEXTURE LOAD ERROR]: Failed to load texture at path: " << path << std::endl;
    }
    stbi_image_free(data);

    return textureID;
}

class Model
{
public:
    std::vector<Mesh> Meshes;
    // 模型目录位置
    std::string Directory;
    // 保存加载过的纹理（提高性能）
    std::vector<Texture> TexturesLoaded;
    bool GammaCorrection;

    Model(std::string const& path, bool gamma = false)
        :
        GammaCorrection(gamma)
    {
        LoadMesh(path);
    }

    // 绘制
    void Draw(Shader& shader)
    {
        for (unsigned int i = 0; i < Meshes.size(); i++)
            Meshes[i].Draw(shader); 
    }

private:
    void LoadMesh(std::string const& path)
    {
        Assimp::Importer importer;
        // OpenGL中大部分的图像的 y轴 都是反的，aiProcess_FlipUVs 处理一下
        const aiScene* scene = importer.ReadFile(path, 
            aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_GenSmoothNormals | aiProcess_CalcTangentSpace);
        
        // 检查了它的一个标记(Flag)，来查看返回的数据是不是不完整的
        if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
        {
            std::cout << "[ASSIMP ERROR]: " << importer.GetErrorString() << std::endl;
            return;
        }
        Directory = path.substr(0, path.find_last_of('/'));

        // 处理节点
        ProcessNode(scene->mRootNode, scene);
    }

    // 处理节点
    void ProcessNode(aiNode* node, const aiScene* scene)
    {
        // 1. 处理节点所有的网格
        for (unsigned int i = 0; i < node->mNumMeshes; i++)
        {
            // Scene下的 mMeshes 数组储存了真正的 Mesh 对象，
            // 节点中的 mMeshes 数组保存的只是场景中网格数组的索引。
            aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
            Meshes.push_back(ProcessMesh(mesh, scene));
        }
        // 2. 递归处理子节点
        for (unsigned int i = 0; i < node->mNumChildren; i++)
        {
            ProcessNode(node->mChildren[i], scene);
        }
    }

    // 处理网格
    Mesh ProcessMesh(aiMesh* mesh, const aiScene* scene)
    {
        std::vector<Vertex> vertices;
        std::vector<unsigned int> indices;
        std::vector<Texture> textures;
        
        // 1. 顶点
        for (unsigned int i = 0; i < mesh->mNumVertices; i++)
        {
            Vertex vertex;

            // 1. 顶点位置
            glm::vec3 vector;
            vector.x = mesh->mVertices[i].x;
            vector.y = mesh->mVertices[i].y;
            vector.z = mesh->mVertices[i].z;
            vertex.Position = vector;

            // 2. 法线
            if (mesh->HasNormals())
            {
                vector.x = mesh->mNormals[i].x;
                vector.y = mesh->mNormals[i].y;
                vector.z = mesh->mNormals[i].z;
                vertex.Normal = vector;
            }

            // 3. 纹理坐标
            if (mesh->mTextureCoords[0])  // 网格是否有纹理坐标
            {
                glm::vec2 vec;
                vec.x = mesh->mTextureCoords[0][i].x;
                vec.y = mesh->mTextureCoords[0][i].y;
                vertex.TexCoord = vec;
                // tangent
                vector.x = mesh->mTangents[i].x;
                vector.y = mesh->mTangents[i].y;
                vector.z = mesh->mTangents[i].z;
                vertex.Tangent = vector;
                // bitangent
                vector.x = mesh->mBitangents[i].x;
                vector.y = mesh->mBitangents[i].y;
                vector.z = mesh->mBitangents[i].z;
                vertex.Bitangent = vector;
            }
            else
            {
                vertex.TexCoord = glm::vec2(0.0f, 0.0f);
            }

            vertices.push_back(vertex);
        }

        // 2. 索引（一个面包含了多个索引）
        for (unsigned int i = 0; i < mesh->mNumFaces; i++)
        {
            aiFace face = mesh->mFaces[i];
            for (unsigned int j = 0; j < face.mNumIndices; j++)
                indices.push_back(face.mIndices[j]);
        }

        // 3. 材质
        // 一个网格只包含了一个指向材质对象的索引
        aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];
        // 1. 漫反射贴图
        std::vector<Texture> diffuseMaps = LoadMaterialTexture(material, aiTextureType_DIFFUSE, "TextureDiffuse");
        textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());
        // 2. 镜面反射贴图
        std::vector<Texture> specularMaps = LoadMaterialTexture(material, aiTextureType_SPECULAR, "TextureSpecular");
        textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());
        // 3. 法线贴图
        std::vector<Texture> normalMaps = LoadMaterialTexture(material, aiTextureType_HEIGHT, "TextureNormal");
        textures.insert(textures.end(), normalMaps.begin(), normalMaps.end());
        // 4. 高度图
        std::vector<Texture> heightMaps = LoadMaterialTexture(material, aiTextureType_AMBIENT, "TextureHeight");
        textures.insert(textures.end(), heightMaps.begin(), heightMaps.end());

        return Mesh(vertices, indices, textures);
    }

    // 加载材质纹理
    std::vector<Texture> LoadMaterialTexture(aiMaterial* mat, aiTextureType type, std::string typeName)
    {
        std::vector<Texture> textures;
        for (unsigned int i = 0; i < mat->GetTextureCount(type); i++)
        {
            aiString str;
            mat->GetTexture(type, i, &str);
            bool skip = false;
            for (unsigned int j = 0; j < TexturesLoaded.size(); j++)
            {
                if (std::strcmp(TexturesLoaded[j].Path.data(), str.C_Str()) == 0)
                {
                    textures.push_back(TexturesLoaded[j]);
                    skip = true;
                    break;
                }
            }
            if (!skip)
            {
                // 如果纹理还没有被加载，则加载它
                Texture texture;
                texture.ID = TextureFromFile(str.C_Str(), Directory);
                texture.Type = typeName;
                texture.Path = str.C_Str();
                textures.push_back(texture);
                // 添加到已加载的纹理中
                TexturesLoaded.push_back(texture);
            }
        }
        return textures;
    }
};