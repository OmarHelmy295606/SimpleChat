#pragma once
#include <QMainWindow>
#include <QListWidget>
#include <QListWidgetItem>
#include <QLineEdit>
#include <QPushButton>
#include <QLabel>
#include <QScrollArea>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QWidget>
#include <QMap>
#include "message.h"

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit MainWindow(const QString& username, QWidget *parent = nullptr);
public slots:
	void onMessageReceived(const Message& message);
	void onUserListUpdated(const QStringList& users);
	void onDisconnected();

signals:
	void messageSent(const QString& recipient, const QString& content);
	void aboutRequested();

private slots:
    	void onSendMessage();
	void onUserSelected(QListWidgetItem* item);

private:
    void setupUI();
    void setupSidebar(QWidget *parent);
    void setupChatPanel(QWidget *parent);
    void addSentBubble(const QString& text);
    void addReceivedBubble(const QString& sender, const QString& text);


    QString username_;
    QString activeRecipient_;
    QMap<QString, QString> conversations_;


    // Sidebar
    QLineEdit    *m_searchBox;
    QListWidget  *m_contactList;
    QLabel*      m_accountName;

    // Chat header
    QLabel       *m_headerName;
    QLabel       *m_headerStatus;

    // Chat area
    QScrollArea  *m_scrollArea;
    QVBoxLayout  *m_chatLayout;

    // Input bar
    QLineEdit    *m_inputBox;
    QPushButton  *m_sendBtn;
};
