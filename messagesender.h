#ifndef MESSAGESENDER_H
#define MESSAGESENDER_H

#include <QObject>

class MessageSender : public QObject
{
    Q_OBJECT
private:
    static MessageSender *mSender;
    MessageSender(QObject *parent = 0): QObject(parent){}
    virtual ~MessageSender(){}
    void sendPrivate(const QString & str)
    {
        emit sendSignal(str);
    }
public:
    static MessageSender * Install(MessageSender *m = new MessageSender())
    {
        if( mSender != 0 )
            delete mSender;
        mSender = m;
        return mSender;
    }
    static void send(const QString & str);
signals:
    void sendSignal(const QString & str);
};

#endif // MESSAGESENDER_H
