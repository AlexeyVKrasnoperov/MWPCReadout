#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <sensormap.h>
#include <QStringList>
#include <QTimer>
#include <QTreeWidgetItem>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

namespace Ui {
class MainWindow;
}

class MWPCDataInterface;
class RatePlot;
class DataAnalizer;
class QSpinBox;

class BoardTriggers: public map<pair<uint,uint>,uint>
{
public:
    bool update(uint address,uint board, uint counts)
    {
        pair<uint,uint> p(address,board);
        iterator i = find(p);
        if( i != end() )
        {
            if( (*i).second == counts )
                return false;
            (*i).second = counts;
        }
        else
            insert(value_type(p,counts));
        return true;
    }
    //
    bool checkTriggers(uint & trg, QStringList & mes)
    {
        trg = 0;
        bool rv = true;
        for( iterator i = begin(); i != end(); ++i )
        {
            if( trg == 0)
            {
                trg = (*i).second;
                continue;
            }
            if( trg != (*i).second )
            {
                rv = false;
                mes << "Boards trigger mismatch !!!";
                mes << QString("Ip \"%1\" Board %2 Trigger %3 (expected %4)")
                       .arg(inet_ntoa(*((in_addr*)(&(*i).first.first))))
                       .arg((*i).first.second).arg((*i).second).arg(trg);
                break;
            }
        }
        //
        //        if( rv == false )
        //        {
        //            mes << "Boards trigger mismatch !!!";
        //            for( iterator i = begin(); i != end(); ++i )
        //                mes << QString("Ip \"%1\" Board %2 Trigger %3")
        //                       .arg(inet_ntoa(*((in_addr*)(&(*i).first.first))))
        //                       .arg((*i).first.second).arg((*i).second);
        //        }
        //
        return rv;
    }
};

class MainWindow : public QMainWindow
{
    Q_OBJECT
protected:
    long lastTrigger;
    BoardTriggers boardTriggers;
    RatePlot *ratePlot;
    QSpinBox *timeWindow;
    QTimer timer;
    SensorMap sensorMap;
    MWPCDataInterface *dataInterface;
    DataAnalizer  *analizer;
    void closeEvent(QCloseEvent *);
    void updateSensorStats(Sensor *s);
    //
    void drawHistogram(int tw);
    //
public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
private:
    Ui::MainWindow *ui;
protected slots:
    void runSlot(bool checked);
    void updateStats(void);
    void settingsSlot(void);
    void slotClearHists(void);
    void slotSwitchMode(bool test);
    void timeWindowChanged(int w);
    void currentItemChangedSlot(QTreeWidgetItem *current, QTreeWidgetItem *previous);
};

#endif // MAINWINDOW_H
