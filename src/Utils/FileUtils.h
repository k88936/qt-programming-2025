//
// Created on 7/4/25.
//

#ifndef FILEUTILS_H
#define FILEUTILS_H

#include <QDir>
#include <QFileInfo>
#include <string>

namespace FileUtils {
    // Check if a directory exists
    inline bool directoryExists(const std::string& path) {
        return QDir(QString::fromStdString(path)).exists();
    }

    // Check if a file exists
    inline bool fileExists(const std::string& path) {
        return QFileInfo::exists(QString::fromStdString(path));
    }

    // Get full path (for debugging)
    inline std::string getAbsolutePath(const std::string& path) {
        return QFileInfo(QString::fromStdString(path)).absoluteFilePath().toStdString();
    }
}

#endif //FILEUTILS_H
