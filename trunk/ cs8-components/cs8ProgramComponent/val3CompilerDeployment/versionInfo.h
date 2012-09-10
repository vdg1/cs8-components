#ifndef VERSIONINFO_H
#define VERSIONINFO_H

#include <QObject>
#include <QString>

class VersionInfo : public QObject
{
     Q_OBJECT

public:
    static QString GetFileDescription(const QString& filePath);
    static QString GetVersionString(const QString& filePath);
};

#endif // VERSIONINFO_H
