#include "include/sys/DPICheck.hpp"

#include <QApplication>
#include <QDate>
#include <QDir>
#include <QFile>
#include <QProcess>
#include <QStandardPaths>

namespace
{
    QString getStateFilePath()
    {
        const QString dirPath = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
        QDir().mkpath(dirPath);
        return dirPath + "/last_dpi_check.txt";
    }

    bool wasRunToday()
    {
        QFile file(getStateFilePath());

        if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
            return false;

        const QString lastRunDate = QString::fromUtf8(file.readAll()).trimmed();
        return lastRunDate == QDate::currentDate().toString(Qt::ISODate);
    }

    void saveRunDate()
    {
        QFile file(getStateFilePath());

        if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
            return;

        file.write(QDate::currentDate().toString(Qt::ISODate).toUtf8());
    }
}

namespace DpiCheck
{
    void TryRunDaily()
    {
        if (wasRunToday())
            return;

#ifdef Q_OS_WIN
        const QString checkerPath = QApplication::applicationDirPath() + "/dpi-checker/dpi_launch.exe";
#else
        const QString checkerPath = QApplication::applicationDirPath() + "/dpi-checker/dpi_launch";
#endif

        if (!QFile::exists(checkerPath))
            return;

        const bool started = QProcess::startDetached(checkerPath);

        if (started)
            saveRunDate();
    }
}