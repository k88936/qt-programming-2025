//
// Created on 7/4/25.
//

#ifndef TEXTUREMANAGER_H
#define TEXTUREMANAGER_H

#include <string>
#include <unordered_map>
#include <vector>
#include <memory>
#include <QDir>
#include <QFileInfo>
#include <QDebug>
#include "Texture.h"
#include "../Utils/Singletion.h"

class TextureManager final : public  Singleton<TextureManager>{
private:
    // Private constructor for singleton

    // Cache of loaded textures by path
    std::unordered_map<std::string, Texture*> textureCache;

    // Cache of directories and their contents
    std::unordered_map<std::string, std::vector<std::string>> directoryCache;

    // Sort files in a directory by name
    std::vector<std::string> getFilesInDirectory(const std::string& directory, const std::string& extension = ".png");

    // Helper to get filenames in a directory sorted properly
    static bool compareFilenames(const QString& a, const QString& b);

    // Load a texture from file
    Texture* loadTexture(const std::string& filePath);

public:
    ~TextureManager() override;
    // Get a texture by directory and index
    Texture* getTexture(const std::string& directory, int index);

    // Get all textures in a directory
    std::vector<Texture*> getAllTextures(const std::string& directory);

    // Clear texture cache
    void clearCache();

    // Get texture frame count in a directory
    int getTextureCount(const std::string& directory);
};

#endif //TEXTUREMANAGER_H
