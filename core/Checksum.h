#ifndef CHECKSUM_H
#define CHECKSUM_H

#include <QFile>
#include <QCryptographicHash>
#include <QDebug>

QByteArray calculateFileHash(const QString &filePath, QCryptographicHash::Algorithm algorithm = QCryptographicHash::Sha256);
QString sha256(const QString &filePath);

#endif // CHECKSUM_H
