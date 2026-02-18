#ifndef CLIENTHANDLER_H
#define CLIENTHANDLER_H

#include <QObject>

class QTcpSocket;
class IGamePacket;

class ClientHandler : public QObject
{
    Q_OBJECT

public:
    explicit ClientHandler(QTcpSocket *socket, QObject *parent = nullptr);
    ~ClientHandler() override;

    QTcpSocket* socket() const;
    QString username() const;

    // ارسال پکت به کلاینت
    void sendPacket(const IGamePacket &packet);

signals:
    void userLoggedIn(const QString &username, ClientHandler *client);
    void packetReceived(ClientHandler *handler, IGamePacket *packet);

private slots:
    void onReadyRead();

private:
    QTcpSocket *m_socket   = nullptr;
    QString     m_username;
};

#endif // CLIENTHANDLER_H
