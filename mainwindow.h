#pragma once
#include <QMainWindow>
#include <QListWidget>
#include <QLineEdit>
#include <QPushButton>
#include <QLabel>
#include <QScrollArea>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QWidget>

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);

private slots:
    void onSendMessage();

private:
    void setupUI();
    void setupSidebar(QWidget *parent);
    void setupChatPanel(QWidget *parent);

    // Sidebar
    QLineEdit    *m_searchBox;
    QListWidget  *m_contactList;

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
