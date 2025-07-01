#include "Checksum.h"

QByteArray calculateFileHash(const QString &filePath, QCryptographicHash::Algorithm algorithm) {
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly)) {
        qWarning() << "Failed to open file:" << filePath;
        return QByteArray();
    }

    QCryptographicHash hash(algorithm);
    if (!hash.addData(&file)) {
        qWarning() << "Failed to read file for hashing.";
        return QByteArray();
    }

    return hash.result();  // Raw bytes
}

QString sha256(const QString &filePath) {
    QByteArray hashBytes = calculateFileHash(filePath, QCryptographicHash::Sha256);
    if (hashBytes.isEmpty()) {
        return "";
    }
    return hashBytes.toHex();
}
