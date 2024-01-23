#ifndef TOOLS_H
#define TOOLS_H

#include <QDir>
#include <QString>
#include <QStringList>


inline bool HasFolder(const QString& path, const QString& what)
{
    QDir directory(path);
    QDir checkingDirectory(directory.absoluteFilePath(what));

    return checkingDirectory.exists();
}


#endif // TOOLS_H
