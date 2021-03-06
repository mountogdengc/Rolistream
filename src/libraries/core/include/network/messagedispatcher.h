#ifndef MESSAGEDISPATCHER_H
#define MESSAGEDISPATCHER_H

#include "channelmodel.h"
#include "network_global.h"
#include "networkmessage.h"
#include "serverconnection.h"
#include <QByteArray>
#include <QObject>

class NetworkLink;
class NetworkMessageReader;
/**
 * @brief The MessageDispatcher class
 */
class NETWORK_EXPORT MessageDispatcher : public QObject
{
    Q_OBJECT

public:
    explicit MessageDispatcher(QObject* parent= nullptr);

    static QString cat2String(NetworkMessageHeader* head);
    static QString act2String(NetworkMessageHeader* head);
signals:
    void messageForAdmin(NetworkMessageReader*, Channel* channel, ServerConnection* emitter);
    void playerNameChanged(const QString& uuid, const QString& name);

public slots:
    void dispatchMessage(QByteArray msg, Channel* channel, ServerConnection* emitter);
};

#endif // MESSAGEDISPATCHER_H
