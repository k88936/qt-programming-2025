//
// Created on 7/4/25.
//

#include "TextureManager.h"
#include <QImage>
#include <algorithm>
#include <QRegularExpression>

TextureManager::~TextureManager() {
    clearCache();
}

Texture* TextureManager::getTexture(const std::string& directory, int index) {
    // Make sure the directory path is normalized
    QString qDir = QString::fromStdString(directory);
    if (!qDir.endsWith('/')) {
        qDir += '/';
    }
    std::string normalizedDir = qDir.toStdString();

    // Get all files in the directory if not already cached
    if (directoryCache.find(normalizedDir) == directoryCache.end()) {
        directoryCache[normalizedDir] = getFilesInDirectory(normalizedDir);
    }

    const auto& files = directoryCache[normalizedDir];

    // Check if index is valid
    if (files.empty()) {
        qWarning() << "TextureManager: No files found in directory:" 
                 << QString::fromStdString(normalizedDir);
        return nullptr;
    }

    if (index < 0 || index >= static_cast<int>(files.size())) {
        qWarning() << "TextureManager: Invalid index" << index 
                 << "for directory:" << QString::fromStdString(normalizedDir)
                 << "(valid range: 0 to" << files.size()-1 << ")";
        return nullptr;
    }

    // Get the file path for the requested index
    std::string filePath = files[index];

    // Check if the texture is already cached
    if (textureCache.find(filePath) != textureCache.end()) {
        return textureCache[filePath];
    }

    // Load the texture
    Texture* texture = loadTexture(filePath);
    if (texture) {
        textureCache[filePath] = texture;
    }

    return texture;
}

std::vector<Texture*> TextureManager::getAllTextures(const std::string& directory) {
    std::vector<Texture*> textures;

    // Make sure the directory path is normalized
    QString qDir = QString::fromStdString(directory);
    if (!qDir.endsWith('/')) {
        qDir += '/';
    }
    std::string normalizedDir = qDir.toStdString();

    // Get all files in the directory if not already cached
    if (directoryCache.find(normalizedDir) == directoryCache.end()) {
        directoryCache[normalizedDir] = getFilesInDirectory(normalizedDir);
    }

    const auto& files = directoryCache[normalizedDir];

    // Load all textures
    for (const auto& file : files) {
        // Check if the texture is already cached
        if (textureCache.find(file) != textureCache.end()) {
            textures.push_back(textureCache[file]);
        } else {
            // Load the texture
            Texture* texture = loadTexture(file);
            if (texture) {
                textureCache[file] = texture;
                textures.push_back(texture);
            }
        }
    }

    return textures;
}

void TextureManager::clearCache() {
    // Delete all cached textures
    for (auto& [path, texture] : textureCache) {
        delete texture;
    }
    textureCache.clear();
    directoryCache.clear();
}

int TextureManager::getTextureCount(const std::string& directory) {
    // Make sure the directory path is normalized
    QString qDir = QString::fromStdString(directory);
    if (!qDir.endsWith('/')) {
        qDir += '/';
    }
    std::string normalizedDir = qDir.toStdString();

    // Get all files in the directory if not already cached
    if (directoryCache.find(normalizedDir) == directoryCache.end()) {
        directoryCache[normalizedDir] = getFilesInDirectory(normalizedDir);
    }

    return static_cast<int>(directoryCache[normalizedDir].size());
}

Texture* TextureManager::loadTexture(const std::string& filePath) {
    QImage image(QString::fromStdString(filePath));
    if (image.isNull()) {
        qWarning() << "TextureManager: Failed to load texture:" << QString::fromStdString(filePath);
        return nullptr;
    }

    // Create a new texture (1 frame, raw size, scale 1.0)
    return new Texture(image, 1, true, 1.0f);
}

bool TextureManager::compareFilenames(const QString& a, const QString& b) {
    // Try to extract numbers from filenames for natural sorting
    QRegularExpression re("(\\d+)");

    // Extract the base name without path or extension
    QString baseA = QFileInfo(a).completeBaseName();
    QString baseB = QFileInfo(b).completeBaseName();

    // Try to find numbers in the filenames
    auto matchA = re.match(baseA);
    auto matchB = re.match(baseB);

    // If both have numbers, compare numerically
    if (matchA.hasMatch() && matchB.hasMatch()) {
        int numA = matchA.captured(1).toInt();
        int numB = matchB.captured(1).toInt();
        return numA < numB;
    }

    // Otherwise, compare lexicographically
    return a < b;
}

std::vector<std::string> TextureManager::getFilesInDirectory(const std::string& directory, const std::string& extension) {
    std::vector<std::string> filePaths;

    QDir dir(QString::fromStdString(directory));
    if (!dir.exists()) {
        qWarning() << "TextureManager: Directory does not exist:" << QString::fromStdString(directory);
        return filePaths;
    }

    // Set file filters
    QStringList filters;
    QString ext = QString::fromStdString(extension);
    if (ext.startsWith('.')) {
        filters << "*" + ext;
    } else {
        filters << "*." + ext;
    }

    dir.setNameFilters(filters);

    // Get list of files
    QFileInfoList fileInfoList = dir.entryInfoList(QDir::Files, QDir::Name);

    // Convert to vector of strings and sort
    QStringList filenames;
    for (const auto& fileInfo : fileInfoList) {
        filenames.append(fileInfo.absoluteFilePath());
    }

    // Sort filenames using our custom comparator for natural sorting
    std::sort(filenames.begin(), filenames.end(), 
              [](const QString& a, const QString& b) { return compareFilenames(a, b); });

    // Convert to std::string
    for (const auto& filename : filenames) {
        filePaths.push_back(filename.toStdString());
    }

    return filePaths;
}
