#include "programsettings.h"
#include <QSettings>
#include <QDir>

ProgramSettings * ProgramSettings::Instance = 0;

ProgramSettings::ProgramSettings()
{
    enableDataWriting = true;
    enableCompression = true;
    rateHistorySize = 60;
    outputDirName = QDir::homePath();
    readSettings();
}

ProgramSettings::~ProgramSettings()
{
    writeSettings();
}

void ProgramSettings::readSettings(void)
{
    QSettings settings;
    settings.beginGroup("ProgramSettings");
    enableDataWriting = settings.value("enableDataWriting",enableDataWriting).toBool();
    enableCompression = settings.value("enableCompression",enableCompression).toBool();
    rateHistorySize = settings.value("rateHistorySize",rateHistorySize).toInt();
    outputDirName = settings.value("outputDirName",outputDirName).toString();
    if( ! QDir(outputDirName).exists() )
        outputDirName = QDir::homePath();
    settings.endGroup();
}

void ProgramSettings::writeSettings(void)
{
    QSettings settings;
    settings.beginGroup("ProgramSettings");
    settings.setValue("enableDataWriting",enableDataWriting);
    settings.setValue("outputDirName",outputDirName);
    settings.setValue("enableCompression",enableCompression);
    settings.setValue("rateHistorySize",rateHistorySize);
    settings.endGroup();
}
