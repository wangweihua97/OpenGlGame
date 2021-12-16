#include "ResourceManager.h"
#include <iostream>
#include <sstream>
#include <fstream>
#include <boost/system.hpp>

#include "stb_image.h"

// Instantiate static variables
std::map<std::string, Texture2D>    ResourceManager::Textures;
std::map<std::string, Shader>       ResourceManager::Shaders;


Shader ResourceManager::LoadShader(const char* vShaderFile, const char* fShaderFile, const char* gShaderFile, std::string name)
{
    Shaders[name] = loadShaderFromFile(vShaderFile, fShaderFile, gShaderFile);
    return Shaders[name];
}

Shader ResourceManager::GetShader(std::string name)
{
    return Shaders[name];
}

Shader* ResourceManager::GetShaderP(std::string name)
{
    return &Shaders[name];
}

bool ResourceManager::IsFileExistent(const boost::filesystem::path& path) {
    boost::system::error_code error;
    return boost::filesystem::is_regular_file(path, error);
}

Texture2D ResourceManager::LoadTexture(const char* file, bool alpha, std::string name)
{
    //Textures[name] = loadTextureFromFile(file, alpha);
    Textures[name] = loadTextureFromFile(file);
    return Textures[name];
}

Texture2D ResourceManager::LoadTexture(const char* file, std::string name)
{
    Textures[name] = loadTextureFromFile(file);
    return Textures[name];
}

Texture2D ResourceManager::GetTexture(std::string name)
{
    return Textures[name];
}

Texture2D* ResourceManager::GetTextureP(std::string name)
{
    return &Textures[name];
}

bool ResourceManager::HaveTexture(std::string name)
{
    auto find = Textures.find(name);
    return find != Textures.end();
}

void ResourceManager::Clear()
{
    // (properly) delete all shaders	
    for (auto iter : Shaders)
        glDeleteProgram(iter.second.ID);
    // (properly) delete all textures
    for (auto iter : Textures)
        glDeleteTextures(1, &iter.second.ID);
}

Shader ResourceManager::loadShaderFromFile(const char* vShaderFile, const char* fShaderFile, const char* gShaderFile)
{
    // 1. retrieve the vertex/fragment source code from filePath
    std::string vertexCode;
    std::string fragmentCode;
    std::string geometryCode;
    try
    {
        // open files
        std::ifstream vertexShaderFile(vShaderFile);
        std::ifstream fragmentShaderFile(fShaderFile);
        std::stringstream vShaderStream, fShaderStream;
        // read file's buffer contents into streams
        vShaderStream << vertexShaderFile.rdbuf();
        fShaderStream << fragmentShaderFile.rdbuf();
        // close file handlers
        vertexShaderFile.close();
        fragmentShaderFile.close();
        // convert stream into string
        vertexCode = vShaderStream.str();
        fragmentCode = fShaderStream.str();
        // if geometry shader path is present, also load a geometry shader
        if (gShaderFile != nullptr)
        {
            std::ifstream geometryShaderFile(gShaderFile);
            std::stringstream gShaderStream;
            gShaderStream << geometryShaderFile.rdbuf();
            geometryShaderFile.close();
            geometryCode = gShaderStream.str();
        }
    }
    catch (std::exception e)
    {
        std::cout << "ERROR::SHADER: Failed to read shader files" << std::endl;
    }
    const char* vShaderCode = vertexCode.c_str();
    const char* fShaderCode = fragmentCode.c_str();
    const char* gShaderCode = geometryCode.c_str();
    // 2. now create shader object from source code
    Shader shader;
    shader.Compile(vShaderCode, fShaderCode, gShaderFile != nullptr ? gShaderCode : nullptr);
    return shader;
}

//Texture2D ResourceManager::loadTextureFromFile(const char* file, bool alpha)
//{
//    // create texture object
//    Texture2D texture;
//    if (alpha)
//    {
//        texture.Internal_Format = GL_RGBA;
//        texture.Image_Format = GL_RGBA;
//    }
//    // load image
//    int width, height, nrChannels;
//    unsigned char* data = stbi_load(file, &width, &height, &nrChannels, 0);
//    // now generate texture
//    texture.Generate(width, height, data);
//    // and finally free image data
//    stbi_image_free(data);
//    return texture;
//}

Texture2D ResourceManager::loadTextureFromFile(const char* file)
{
    Texture2D texture;
    int width, height, nrChannels;
    unsigned char* data = stbi_load(file, &width, &height, &nrChannels, 0);
    if(data)
    {
        if (nrChannels == 1)
        {
            texture.Internal_Format = GL_RED;
            texture.Image_Format = GL_RED;
        }
        else if (nrChannels == 3)
        {
            texture.Internal_Format = GL_RGB;
            texture.Image_Format = GL_RGB;
        }
        else if (nrChannels == 4)
        {
            texture.Internal_Format = GL_RGBA;
            texture.Image_Format = GL_RGBA;
        }
    }
    else
    {
        std::cout << "Texture failed to load at path: " << file << std::endl;
    }
    // load image
    // now generate texture
    texture.Generate(width, height, data);
    // and finally free image data
    stbi_image_free(data);
    return texture;
}
