#ifndef DATAANALIZER_H
#define DATAANALIZER_H

#include <QThread>

class SensorMap;

class DataAnalizer : public QThread
{
    Q_OBJECT
protected:
    SensorMap *sensorMap;
    bool breakLoop;
    void run(void);
public:
    explicit DataAnalizer(SensorMap *s, QObject *parent = 0);
    ~DataAnalizer();
signals:

public slots:

};

#endif // DATAANALIZER_H
