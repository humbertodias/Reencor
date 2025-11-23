#include "AssetLoader.h"
#include "Renderer.h"

#include <algorithm>
#include <SDL2/SDL_image.h>
#include <iostream>
#include <filesystem>
#include <fstream>

namespace fs = std::filesystem;

void AssetLoader::loadAssets(const std::string& assetsPath,
                             std::unordered_map<std::string, GLuint>& imageDict,
                             std::unordered_map<std::string, Mix_Chunk*>& soundDict,
                             std::unordered_map<std::string, void*>& objectDict)
{
    if (!fs::exists(assetsPath) || !fs::is_directory(assetsPath)) {
        std::cerr << "Assets path does not exist: " << assetsPath << std::endl;
        return;
    }

    // Walk through all files in assets directory
    for (const auto& entry : fs::recursive_directory_iterator(assetsPath)) {
        if (!entry.is_regular_file()) continue;

        std::string filepath = entry.path().string();
        std::string extension = entry.path().extension().string();
        std::string key = getParentKey(filepath, assetsPath);

        // Convert extension to lowercase
        std::transform(extension.begin(), extension.end(), extension.begin(), ::tolower);

        try {
            if (extension == ".png" || extension == ".jpg" || extension == ".jpeg") {
                GLuint texture = loadImage(filepath);
                if (texture != 0) {
                    imageDict[key] = texture;
                }
            }
            else if (extension == ".wav" || extension == ".ogg" || extension == ".mp3") {
                Mix_Chunk* sound = loadSound(filepath);
                if (sound) {
                    soundDict[key] = sound;
                }
            }
            else if (extension == ".json" || extension == ".xml") {
                void* data = loadJSON(filepath);
                if (data) {
                    objectDict[key] = data;
                }
            }
        }
        catch (const std::exception& e) {
            std::cerr << "Failed to load asset " << filepath << ": " << e.what() << std::endl;
        }
    }

    std::cout << "Loaded " << imageDict.size() << " images, "
              << soundDict.size() << " sounds, "
              << objectDict.size() << " objects" << std::endl;
}

GLuint AssetLoader::loadImage(const std::string& path) {
    SDL_Surface* surface = IMG_Load(path.c_str());
    if (!surface) {
        std::cerr << "Failed to load image " << path << ": " << IMG_GetError() << std::endl;
        return 0;
    }

    std::cout << "Loading image: " << path << " (format: " << SDL_GetPixelFormatName(surface->format->format) 
              << ", size: " << surface->w << "x" << surface->h << ")" << std::endl;

    // Convert surface to a consistent format (RGBA32)
    SDL_Surface* formattedSurface = SDL_ConvertSurfaceFormat(surface, SDL_PIXELFORMAT_RGBA32, 0);
    SDL_FreeSurface(surface);
    
    if (!formattedSurface) {
        std::cerr << "Failed to convert surface format for " << path << ": " << SDL_GetError() << std::endl;
        return 0;
    }

    GLuint texture;
    glGenTextures(1, &texture);
    Renderer::checkGLError("glGenTextures");
    
    glBindTexture(GL_TEXTURE_2D, texture);
    Renderer::checkGLError("glBindTexture");
    
    // Always use RGBA since we converted to RGBA32
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, formattedSurface->w, formattedSurface->h, 0,
                 GL_RGBA, GL_UNSIGNED_BYTE, formattedSurface->pixels);
    Renderer::checkGLError("glTexImage2D");
    
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    Renderer::checkGLError("glTexParameteri");

    SDL_FreeSurface(formattedSurface);
    
    std::cout << "Successfully loaded texture from " << path << " (ID: " << texture << ")" << std::endl;
    
    return texture;
}

Mix_Chunk* AssetLoader::loadSound(const std::string& path) {
    Mix_Chunk* sound = Mix_LoadWAV(path.c_str());
    if (!sound) {
        std::cerr << "Failed to load sound " << path << ": " << Mix_GetError() << std::endl;
    }
    return sound;
}

void* AssetLoader::loadJSON(const std::string& path) {
    // TODO: Implement JSON parsing
    // For now, return nullptr as we need a proper JSON library
    std::ifstream file(path);
    if (!file.is_open()) {
        std::cerr << "Failed to open JSON file: " << path << std::endl;
        return nullptr;
    }
    return nullptr;
}

std::string AssetLoader::getParentKey(const std::string& filepath, const std::string& basePath) {
    fs::path path(filepath);
    fs::path base(basePath);
    
    fs::path relative = fs::relative(path, base);
    std::string key = relative.parent_path().filename().string() + "/" + 
                     relative.stem().string();
    
    // Replace backslashes with forward slashes
    std::replace(key.begin(), key.end(), '\\', '/');
    
    return key;
}
