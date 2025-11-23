#ifndef ASSETLOADER_H
#define ASSETLOADER_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_mixer.h>
#include <GL/gl.h>
#include <string>
#include <unordered_map>

class AssetLoader {
public:
    static void loadAssets(const std::string& assetsPath,
                          std::unordered_map<std::string, GLuint>& imageDict,
                          std::unordered_map<std::string, Mix_Chunk*>& soundDict,
                          std::unordered_map<std::string, void*>& objectDict);

private:
    static GLuint loadImage(const std::string& path);
    static Mix_Chunk* loadSound(const std::string& path);
    static void* loadJSON(const std::string& path);
    static std::string getParentKey(const std::string& filepath, const std::string& basePath);
};

#endif // ASSETLOADER_H
