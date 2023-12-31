#include <QCoreApplication>
#include <QTcpServer>
#include <QTcpSocket>

class SimpleHttpServer : public QTcpServer {
public:
    SimpleHttpServer(QObject *parent = nullptr) : QTcpServer(parent) {
        if (!this->listen(QHostAddress::Any, 8080)) {
            qFatal("Unable to start the server: %s", qPrintable(this->errorString()));
        } else {
            qDebug("Server started!");
        }
    }

    void handleClient(QTcpSocket* socket) {
        if (socket->canReadLine()) {
            // خواندن درخواست از سوکت
            QByteArray requestLine = socket->readLine();
            QString requestStr(requestLine);

            // مثال: تجزیه درخواست و استخراج مسیر از آن
            QStringList requestParts = requestStr.split(' ');
            if (requestParts.count() >= 2) {
                QString method = requestParts[0]; // مثلا "GET"
                QString path = requestParts[1]; // مسیر درخواستی

                if(method == "GET"){
                    // ارسال پاسخ بر اساس مسیر
                    if (path == "/") {
                        // مسیر اصلی یا روت
                        socket->write("HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n<h1>Welcome to the root page!</h1>");
                    } else if (path == "/hello") {
                        // مسیر Hello
                        socket->write("HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n<h1>Hello, user!</h1>");
                    } else {
                        // مسیر نامعلوم
                        socket->write("HTTP/1.1 404 Not Found\r\nContent-Type: text/html\r\n\r\n<h1>404 Page Not Found</h1>");
                    }
                }else if(method=="POST"){

                    if (path == "/submit") {  // فرض بر اینکه مسیر "/submit" است
                        QByteArray content;
                        while (socket->canReadLine()) {
                            QString line = socket->readLine();
                            content.append(line);
                        }
                        // اکنون شما فرض می‌کنید محتویات دریافت شده:
                        // برای سادگی فرمت ثابت "key=value" است.
                        QStringList contentList = QString(content).split('&');
                        QString response;

                        // مثال: "key1=value1&key2=value2"
                        for (const QString &param : contentList) {
                            // تجزیه و استخراج key-value از محتوی
                            response.append("Received: " + param + "\n");
                        }
                        // ارسال پاسخ
                        socket->write("HTTP/1.1 200 OK\r\nContent-Type: text/plain\r\n\r\n");
                        socket->write(response.toUtf8());
                    }
                }

            }

            socket->disconnectFromHost();
        }
    }


protected:
    void incomingConnection(qintptr socketDescriptor) override {
        QTcpSocket *socket = new QTcpSocket(this);

        if (socket->setSocketDescriptor(socketDescriptor)) {
            connect(socket, &QTcpSocket::readyRead, [this, socket]() { handleClient(socket); });
            connect(socket, &QTcpSocket::disconnected, socket, &QTcpSocket::deleteLater);
        } else {
            qWarning("Failed to set socket descriptor!");
        }
    }
};

int main(int argc, char *argv[]) {
    QCoreApplication a(argc, argv);

    SimpleHttpServer server;

    return a.exec();
}


//for test =========>  127.0.0.1:8080/+path   in browser or postman or ...
