#include "playlistmanager.h"
#include <QFile>
#include <QTextStream>

void PlaylistManager::save(const QStringList& list, const QString& fileName) {
    QFile file(fileName);
    if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QTextStream out(&file);
        for (const QString& path : list)
            out << path << "\n";
    }
}

QStringList PlaylistManager::load(const QString& fileName) {
    QStringList list;
    QFile file(fileName);
    if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QTextStream in(&file);
        while (!in.atEnd()) {
            QString line = in.readLine().trimmed();
            if (!line.isEmpty())
                list << line;
        }
    }
    return list;
}

void PlaylistManager::clear(const QString& fileName) {
    QFile::remove(fileName);
}
