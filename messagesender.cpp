#include "messagesender.h"

MessageSender *MessageSender::mSender = 0;

void MessageSender::send(const QString & str)
{
    if( mSender != 0 )
        mSender->sendPrivate(str);
    else
        fprintf(stderr,"%s\n",str.toLocal8Bit().data());
}
