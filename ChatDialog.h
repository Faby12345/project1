#ifndef CHATDIALOG_H
#define CHATDIALOG_H

#include <QDialog>
#include <QTextBrowser>
#include <QLineEdit>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QtNetwork/QNetworkAccessManager>
#include <QtNetwork/QNetworkRequest>
#include <QtNetwork/QNetworkReply>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QUrl>
#include <QDebug>

#include "api.h"

class ChatDialog : public QDialog {
    Q_OBJECT

public:
    explicit ChatDialog(QWidget *parent = nullptr)
        : QDialog(parent),
        messageView(new QTextBrowser(this)),
        inputLine(new QLineEdit(this)),
        sendButton(new QPushButton("Send", this)),
        networkManager(new QNetworkAccessManager(this))
    {

        setWindowTitle("Chat");
        resize(400, 300);

        auto *layout = new QVBoxLayout(this);
        layout->addWidget(messageView);

        auto *inputLayout = new QHBoxLayout;
        inputLayout->addWidget(inputLine);
        inputLayout->addWidget(sendButton);
        layout->addLayout(inputLayout);

        connect(sendButton, &QPushButton::clicked, this, &ChatDialog::onSend);
        connect(inputLine, &QLineEdit::returnPressed, this, &ChatDialog::onSend);
        connect(networkManager, &QNetworkAccessManager::finished,
                this, &ChatDialog::onNetworkReply);
    }

private slots:
    void onSend() {
        API API_TOKEN;
        QString text = inputLine->text().trimmed();
        if (text.isEmpty()) return;

        messageView->append("<b>You:</b> " + text);
        QJsonObject userMsg;
        userMsg["role"] = "user";
        userMsg["content"] = text;
        conversation.append(userMsg);
        inputLine->clear();

        QNetworkRequest request(QUrl("https://openrouter.ai/api/v1/chat/completions"));
        request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
        request.setRawHeader("Authorization", QByteArray("Bearer ") + QByteArray(API_TOKEN.getToken()));
        request.setRawHeader("HTTP-Referer", QByteArray("my-qt-chat-app"));
        request.setRawHeader("X-Title", QByteArray("MyQtDesktopChat"));

        QJsonObject body;
        body["model"] = "deepseek/deepseek-chat:free";
        body["messages"] = conversation;

        networkManager->post(request, QJsonDocument(body).toJson());
    }

    void onNetworkReply(QNetworkReply* reply) {
        int status = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
        QByteArray resp = reply->readAll();
        //qDebug() << "HTTP status:" << status << "\nBody:" << resp;

        if (reply->error() != QNetworkReply::NoError) {
            messageView->append("<b>Bot:</b> Error fetching reply");
        } else {
            QJsonDocument doc = QJsonDocument::fromJson(resp);
            QString botText;
            if (doc.isObject()) {
                auto choices = doc.object()["choices"].toArray();
                if (!choices.isEmpty()) {
                    botText = choices[0].toObject()["message"].toObject()["content"].toString();
                }
            }
            if (botText.isEmpty()) botText = QString::fromUtf8(resp);
            messageView->append("<b>Bot:</b> " + botText);
            QJsonObject botMsg;
            botMsg["role"] = "assistant";
            botMsg["content"] = botText;
            conversation.append(botMsg);
        }
        reply->deleteLater();
    }

private:
    QTextBrowser             *messageView;
    QLineEdit                *inputLine;
    QPushButton              *sendButton;
    QNetworkAccessManager    *networkManager;
    QJsonArray                conversation;
};

#endif // CHATDIALOG_H
