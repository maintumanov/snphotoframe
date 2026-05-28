#ifndef PLAYLIST_MANAGER_H
#define PLAYLIST_MANAGER_H

#include <QStringList>

class PlaylistManager {
public:
    static void save(const QStringList& list, const QString& fileName = "playlist.txt");
    static QStringList load(const QString& fileName = "playlist.txt");
    static void clear(const QString& fileName = "playlist.txt");
};

#endif // PLAYLIST_MANAGER_H
