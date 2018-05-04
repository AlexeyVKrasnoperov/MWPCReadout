#include "mainwindow.h"
#include "ui_mainwindow.h"
#ifdef PCAPINTERFACE
#include "pcapinterface.h"
#else
#include "udpsocketinterface.h"
#endif
#include  <QUdpSocket>
#include <QDateTime>
#include "programsettings.h"
#include "settingsdialog.h"
#include "rateplot.h"
#include "dataanalizer.h"
#include <QSpinBox>
#include <QLabel>
#include <QMessageBox>
#include "messagesender.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    //
    lastTrigger = -1;
    //
    ui->setupUi(this);
    //
    ratePlot = new RatePlot(ui->frameRate);
    ui->rateLayout->addWidget(ratePlot);
    //
    QLabel *l = new QLabel("Time window",this);
    ui->mainToolBar->insertWidget(ui->mainToolBar->actions().last(),l);
    timeWindow = new QSpinBox(this);
    timeWindow->setRange(0,16);
    timeWindow->setValue(2);
    timeWindow->setToolTip("Time window");
    connect(timeWindow,SIGNAL(valueChanged(int)),this,SLOT(timeWindowChanged(int)));
    ui->mainToolBar->insertWidget(ui->mainToolBar->actions().last(),timeWindow);
    ui->mainToolBar->insertSeparator(ui->mainToolBar->actions().last());
    //
    QWidget* spacer = new QWidget();
    spacer->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    ui->mainToolBar->insertWidget(ui->mainToolBar->actions().last(),spacer);
    //
    for( int c = 0; c < SensorMap::NChambers; c++)
    {
        QTreeWidgetItem *item = new QTreeWidgetItem(ui->treeChambers);
        item->setText(0,tr("Chamber %1 Wire Cnts").arg(c+1));
        item->setData(0,Qt::UserRole,c);
        item->setIcon(0,QIcon(":Icons/histogram-down.png"));
        ui->treeChambers->addTopLevelItem(item);
        for(int i = 0; i < ui->treeChambers->columnCount(); i++)
            item->setTextAlignment(i,Qt::AlignLeft);
    }
    //
    connect(ui->treeStats,SIGNAL(currentItemChanged(QTreeWidgetItem*,QTreeWidgetItem*)),
            this,SLOT(currentItemChangedSlot(QTreeWidgetItem*,QTreeWidgetItem*)));
    connect(ui->treeChambers,SIGNAL(currentItemChanged(QTreeWidgetItem*,QTreeWidgetItem*)),
            this,SLOT(currentItemChangedSlot(QTreeWidgetItem*,QTreeWidgetItem*)));
    //
    dataInterface = 0;
    analizer   =  new DataAnalizer(&sensorMap,this);
    timer.setSingleShot(true);
    timer.setInterval(500);
    connect(&timer,SIGNAL(timeout()),this,SLOT(updateStats()));
    //
    MessageSender *m = MessageSender::Install();
    connect(m,SIGNAL(sendSignal(QString)),ui->textLog,SLOT(append(QString)));
}

MainWindow::~MainWindow()
{

    delete ui;
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    timer.stop();
    if( analizer != 0 )
    {
        delete analizer;
        analizer = 0;
    }
    if( ui->actionRun->isChecked() )
    {
        if( dataInterface != 0 )
        {
            delete dataInterface;
            dataInterface = 0;
            runSlot(false);
            ui->actionRun->setChecked(false);
        }
    }
    ProgramSettings::deleteInstance();
    QMainWindow::closeEvent(event);
}


void MainWindow::runSlot(bool checked)
{
    if( checked )
    {
        if( dataInterface == 0 )
        {
#ifdef PCAPINTERFACE
            dataInterface = new PCapInterface("eth1",&sensorMap);
#else
            dataInterface = new UDPSocketInterface(&sensorMap);
#endif
            if( dataInterface->isRunning() )
                timer.start();
            else
            {
                delete dataInterface;
                dataInterface = 0;
                if( ui->actionRun->isChecked() )
                    ui->actionRun->setChecked(false);
            }
        }
    }
    else
    {
        if( ui->actionRun->isChecked() )
            ui->actionRun->setChecked(false);
        if( dataInterface != 0 )
        {
            delete dataInterface;
            dataInterface = 0;
        }
    }
}

void MainWindow::updateStats(void)
{
    bool newTrigger = false;
    {
        if( ! sensorMap.getMutex()->tryLock() )
        {
            timer.start(10);
            return;
        }
        for(SensorMap::iterator i = sensorMap.begin(); i != sensorMap.end(); ++i)
        {
            Sensor *s = (*i).second;
            updateSensorStats(s);
            for( uint b = 0; b < s->getNBoard(); b++ )
                newTrigger |= boardTriggers.update(s->getAddress(),b,s->getBoardTrg(b));
        }
        sensorMap.getMutex()->unlock();
        timer.start(500);
    }
    //
    static int periodCnts = -1;
    if( newTrigger )
        periodCnts = 0;
    else if( periodCnts != -1 )
        periodCnts++;
    //
    if( periodCnts == 3 )
    {
        uint trg = 0;
        QStringList mes;
        if( ! boardTriggers.checkTriggers(trg,mes) )
        {
            foreach(const QString & s,mes)
                qWarning() << s;
        }
        else
        {
            if( dataInterface != 0 )
                dataInterface->setLastTrigger(trg);
        }
        periodCnts = -1;
        //
        vector<long> branchTriggers;
        for(SensorMap::iterator i = sensorMap.begin(); i != sensorMap.end(); ++i)
            branchTriggers.push_back((*i).second->getBranchTrigger());
        sort(branchTriggers.begin(),branchTriggers.end(),less<long>());
        branchTriggers.erase(unique(branchTriggers.begin(), branchTriggers.end()), branchTriggers.end());
        //        if( branchTriggers.size() > 1 )
        //        {
        //            if(( branchTriggers.front() != -1 )||( branchTriggers.front() != 1 ))
        //            {
        //                runSlot(false);
        //                QMessageBox::critical(this,"Error","Branch Trigger mismatch !!!\nRun terminated !!!!");
        //                return;
        //            }
        //        }
    }
    //
    drawHistogram(timeWindow->value());
    //
    if( dataInterface != 0 )
        timer.start();
}
//
void MainWindow::drawHistogram(int tw)
{
    if( ui->tabWidget->currentIndex() == 0 )
    {
        QTreeWidgetItem *item = ui->treeStats->currentItem();
        if( item != 0 )
        {
            uint id = item->data(0,Qt::UserRole).toUInt();
            QString title = item->text(0);
            QTreeWidgetItem *parent = item->parent();
            while( parent != 0 )
            {
                title.prepend(QString("%1/").arg(parent->text(0)));
                parent = parent->parent();
            }
            ratePlot->Draw(id,title,sensorMap.getHist(id,tw));
        }
        else
            ratePlot->Draw();
    }
    else
    {
        QTreeWidgetItem *item = ui->treeChambers->currentItem();
        if( item != 0 )
        {
            uint chamber = item->data(0,Qt::UserRole).toUInt();
            QString title = item->text(0);
            vector<uint> *current_chamber_data=(vector<uint> *)sensorMap.getChamberProfile(chamber,tw);
            ratePlot->Draw(-1,title,current_chamber_data);
        }
        else
            ratePlot->Draw();
    }
}

//

void MainWindow::updateSensorStats(Sensor *s)
{
    //
    QTreeWidgetItem *item = 0;
    QList<QTreeWidgetItem*> items = ui->treeStats->findItems(s->getName(),Qt::MatchExactly,0);
    if( ! items.empty() )
        item = items.back();
    else
    {
        item = new QTreeWidgetItem(ui->treeStats);
        item->setText(0,s->getName());
        item->setData(0,Qt::UserRole,s->getAddress());
        item->setIcon(0,QIcon(":Icons/histogram-down.png"));
        ui->treeStats->addTopLevelItem(item);
        for(int i = 0; i < ui->treeStats->columnCount(); i++)
            item->setTextAlignment(i,Qt::AlignCenter);
        for(int i = 0; i < ui->treeStats->columnCount(); i++)
            ui->treeStats->resizeColumnToContents(i);
    }
    //
    bool equal = (s->getRecvCnt() == s->getSendCnt());
    item->setText(1,QString("%1 (%2)").arg(s->getRecvCnt()).arg(s->getSendCnt()));
    item->setForeground(1,QBrush(equal?Qt::black:Qt::red));
    item->setText(2,QString::number(s->size()));
    //
    for( uint i = 0; i < s->getNBoard(); i++ )
    {
        QTreeWidgetItem *child0 = 0;
        for( int j = 0; j < item->childCount(); j++)
        {
            if( item->child(j)->data(0,Qt::UserRole).toUInt() == i )
                child0 = item->child(j);
        }
        if( child0 == 0 )
        {
            child0 = new QTreeWidgetItem(item);
            child0->setText(0,QString("Board %1").arg(i));
            child0->setData(0,Qt::UserRole,i);
            child0->setIcon(0,QIcon(":Icons/histogram-down.png"));
            for(int j = 0; j < ui->treeStats->columnCount(); j++)
                child0->setTextAlignment(j,Qt::AlignCenter);
            for(int i = 0; i < ui->treeStats->columnCount(); i++)
                ui->treeStats->resizeColumnToContents(i);
        }
        child0->setText(1,QString("%1").arg(s->getBoardCnt(i)));
        child0->setText(2,QString("%1").arg(s->getBoardTrg(i)));
        //
        if( child0->childCount() == 0 )
        {
            uint id = (s->getIp() << 24) + ((i+1) << 16);
            QTreeWidgetItem *child1 = 0;
            child1 = new QTreeWidgetItem(child0);
            child1->setText(0,QString("Window Cnts"));
            child1->setData(0,Qt::UserRole,id + 0x1000);
            child1->setIcon(0,QIcon(":Icons/histogram-down.png"));
            child1->setTextAlignment(0,Qt::AlignLeft);
            child1 = new QTreeWidgetItem(child0);
            child1->setText(0,QString("Wire Cnts"));
            child1->setData(0,Qt::UserRole,id + 0x2000);
            child1->setIcon(0,QIcon(":Icons/histogram-down.png"));
            child1->setTextAlignment(0,Qt::AlignLeft);
            for(int i = 0; i < ui->treeStats->columnCount(); i++)
                ui->treeStats->resizeColumnToContents(i);
        }
    }
    //
}

void MainWindow::settingsSlot(void)
{
    SettingsDialog dlg;
    dlg.exec();
}

void MainWindow::slotClearHists(void)
{
    sensorMap.clear();
    ui->treeStats->clear();
    // updateStats();
    timer.start(0);
    // drawHistogram(timeWindow->value());
}

void MainWindow::slotSwitchMode(bool test)
{
    sensorMap.getMutex()->lock();
    for(SensorMap::iterator i = sensorMap.begin(); i != sensorMap.end(); ++i)
    {
        Sensor *s = (*i).second;
        s->setTestMode(test);
    }
    sensorMap.getMutex()->unlock();
}

void MainWindow::timeWindowChanged(int w)
{
    drawHistogram(w);
}

//void MainWindow::timeWake(int w)
//{
//    for(SensorMap::iterator i = sensorMap.begin(); i != sensorMap.end(); ++i)
//    {
//        Sensor *s = (*i).second;
//        s->writeDatagram('w');
//    }

//}

void MainWindow::currentItemChangedSlot(QTreeWidgetItem *current, QTreeWidgetItem *previous)
{
    Q_UNUSED(current);
    Q_UNUSED(previous);
    drawHistogram(timeWindow->value());
}

