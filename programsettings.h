#ifndef PROGRAMSETTINGS_H
#define PROGRAMSETTINGS_H

#include <QString>

class ProgramSettings
{
protected:
    QString outputDirName;
    bool enableDataWriting;
    bool enableCompression;
    int  rateHistorySize;
    static ProgramSettings *Instance;
    ProgramSettings();
    ~ProgramSettings();
    static ProgramSettings *getInstance(void)
    {
        if( Instance == 0 )
            Instance = new ProgramSettings();
        return Instance;
    }
    void readSettings(void);
    void writeSettings(void);
public:
    static const QString & getOutputDirName(void)
    {
        return getInstance()->outputDirName;
    }
    static void setOutputDirName(const QString & str)
    {
        getInstance()->outputDirName = str;
    }
    static bool isDataWritingEnabled(void)
    {
        return getInstance()->enableDataWriting;
    }
    static void setDataWritingEnabled(bool enable)
    {
        getInstance()->enableDataWriting = enable;
    }
    static bool isCompressionEnabled(void)
    {
        return getInstance()->enableCompression;
    }
    static void setCompressionEnabled(bool enable)
    {
        getInstance()->enableCompression = enable;
    }
    static void setRateHistorySize(int v)
    {
        getInstance()->rateHistorySize = v;
    }
    static int getRateHistorySize(void)
    {
        return getInstance()->rateHistorySize;
    }
    static void deleteInstance(void)
    {
        if( Instance != 0 )
        {
            delete Instance;
            Instance = 0;
        }
    }
};

#endif // PROGRAMSETTINGS_H
