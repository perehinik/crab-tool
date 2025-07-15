#ifndef CONSTANTS_H
#define CONSTANTS_H

#include <QStringList>

#define PROJECT_VERSION APP_VERSION

#define PROJECT_EXTENSION "ctp"
#define TEMP_PROJECT_FILENAME ".crab-tool-tmp." PROJECT_EXTENSION

inline const QStringList IMAGE_EXTENSIONS = {
    "jpg", "jpeg", "png", "bmp", "gif", "webp"
};

inline const QStringList IMAGE_EXTENSION_FILTERS = {
    "*.jpg", "*.jpeg", "*.png", "*.bmp", "*.gif", "*.webp"
};

#endif // CONSTANTS_H
