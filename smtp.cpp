#include "smtp.h"

Smtp::Smtp(const QString &user, const QString &pass, const QString &host, int port, int timeout)
    : user(user), pass(pass), host(host), port(port), timeout(timeout)
{
    socket = new QSslSocket(this);
    t = new QTextStream(socket);
    state = Init;

    connect(socket, &QSslSocket::stateChanged, this, &Smtp::stateChanged);
    connect(socket, &QSslSocket::readyRead, this, &Smtp::readyRead);
    connect(socket, &QSslSocket::disconnected, this, &Smtp::disconnected);
    connect(socket, QOverload<QAbstractSocket::SocketError>::of(&QSslSocket::errorOccurred),
            this, &Smtp::errorReceived);
}

Smtp::~Smtp()
{
    delete t;
    delete socket;
}

bool Smtp::sendMail(const QString &from, const QString &to, const QString &subject, const QString &body)
{
    this->from = from;
    this->rcpt = to;

    message = "To: " + to + "\n";
    message.append("From: " + from + "\n");
    message.append("Subject: " + subject + "\n");
    message.append("\n" + body + "\n");

    socket->connectToHostEncrypted(host, port);

    if (!socket->waitForConnected(timeout)) {
        emit status("Connexion au serveur impossible.");
        return false;
    }

    state = Init;
    return true;
}

void Smtp::readyRead()
{
    response += socket->readAll();

    if (state == Init && response.startsWith("220")) {
        *t << "EHLO " << host << "\r\n";
        t->flush();
        state = HandShake;
    } else if (state == HandShake && response.contains("250")) {
        *t << "AUTH LOGIN\r\n";
        t->flush();
        state = Auth;
    } else if (state == Auth && response.contains("334")) {
        *t << QByteArray(user.toUtf8()).toBase64() << "\r\n";
        t->flush();
        state = User;
    } else if (state == User && response.contains("334")) {
        *t << QByteArray(pass.toUtf8()).toBase64() << "\r\n";
        t->flush();
        state = Pass;
    } else if (state == Pass && response.contains("235")) {
        *t << "MAIL FROM: <" << from << ">\r\n";
        t->flush();
        state = Mail;
    } else if (state == Mail && response.contains("250")) {
        *t << "RCPT TO: <" << rcpt << ">\r\n";
        t->flush();
        state = Rcpt;
    } else if (state == Rcpt && response.contains("250")) {
        *t << "DATA\r\n";
        t->flush();
        state = Data;
    } else if (state == Data && response.startsWith("354")) {
        *t << message << "\r\n.\r\n";
        t->flush();
        state = Body;
    } else if (state == Body && response.startsWith("250")) {
        *t << "QUIT\r\n";
        t->flush();
        state = Quit;
    } else if (state == Quit && response.startsWith("221")) {
        socket->disconnectFromHost();
        emit status("Message envoyé avec succès !");
        state = Close;
    }
}

void Smtp::stateChanged(QAbstractSocket::SocketState socketState)
{
    qDebug() << "État du socket changé : " << socketState;
}

void Smtp::errorReceived(QAbstractSocket::SocketError socketError)
{
    qDebug() << "Erreur du socket : " << socketError;
    emit status("Erreur lors de l'envoi de l'email : " + socket->errorString());
}

void Smtp::disconnected()
{
    qDebug() << "Déconnexion du serveur SMTP.";
}
