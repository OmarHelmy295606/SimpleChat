#include "mainwindow.h"
#include <QSplitter>
#include <QScrollBar>
#include <QDateTime>
#include <QFrame>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    setWindowTitle("Chat");
    setMinimumSize(800, 560);
    setupUI();
}


void MainWindow::setupUI() {
    QWidget *root = new QWidget(this);
    setCentralWidget(root);

    QHBoxLayout *rootLayout = new QHBoxLayout(root);
    rootLayout->setContentsMargins(0, 0, 0, 0);
    rootLayout->setSpacing(0);

    //left sidebar
    QWidget *sidebar = new QWidget;
    sidebar->setFixedWidth(260);
    sidebar->setObjectName("sidebar");
    setupSidebar(sidebar);

    //line that divides between sidebar and chat
    QFrame *divider = new QFrame;
    divider->setFrameShape(QFrame::VLine);
    divider->setObjectName("divider");

    //right chat panel
    QWidget *chatPanel = new QWidget;
    chatPanel->setObjectName("chatPanel");
    setupChatPanel(chatPanel);

    rootLayout->addWidget(sidebar);
    rootLayout->addWidget(divider);
    rootLayout->addWidget(chatPanel, 1); // stretch = 1 means it fills remaining space
setStyleSheet(R"(
        QMainWindow, QWidget { background: #ffffff; }

        #sidebar { background: #ffffff; }
        #sidebarTop { background: #ffffff; border-bottom: 1px solid #e8e6e0; }
        #sidebarTitle { font-size: 15px; font-weight: 600; color: #2C2C2A; }

        #searchBox {
            background: #F1EFE8;
            border: 1px solid #D3D1C7;
            border-radius: 8px;
            padding: 0 10px;
            font-size: 13px;
            color: #2C2C2A;
        }

        #contactList {
            background: #ffffff;
            border: none;
            font-size: 13px;
        }
        #contactList::item {
            padding: 10px 14px;
            border-bottom: 1px solid #F1EFE8;
            color: #2C2C2A;
        }
        #contactList::item:selected {
            background: #E6F1FB;
            color: #185FA5;
            border-left: 3px solid #378ADD;
        }
        #contactList::item:hover:!selected { background: #F8F7F4; }

        #accountBar { border-top: 1px solid #e8e6e0; }
        #accountName { font-size: 13px; font-weight: 500; color: #2C2C2A; }

        #divider { color: #E8E6E0; }

        #chatHeader { background: #ffffff; border-bottom: 1px solid #E8E6E0; }
        #headerName { font-size: 14px; font-weight: 600; color: #2C2C2A; }
        #headerStatus { font-size: 11px; color: #639922; }

        #chatScroll { border: none; background: #FAFAF8; }
        #chatWidget { background: #FAFAF8; }

        #inputBar { background: #ffffff; border-top: 1px solid #E8E6E0; }
        #inputBox {
            background: #F1EFE8;
            border: 1px solid #D3D1C7;
            border-radius: 18px;
            padding: 0 14px;
            font-size: 13px;
            color: #2C2C2A;
        }
        #inputBox:focus { border-color: #378ADD; }

        #sendBtn {
            background: #378ADD;
            color: white;
            border: none;
            border-radius: 18px;
            font-size: 13px;
            font-weight: 500;
        }
        #sendBtn:hover { background: #185FA5; }
        #sendBtn:pressed { background: #0C447C; }

        #sentBubble {
            background: #378ADD;
            color: white;
            border-radius: 12px;
            border-bottom-right-radius: 2px;
            font-size: 13px;
        }
    )");
}

void MainWindow::setupSidebar(QWidget *parent) {
    QVBoxLayout *layout = new QVBoxLayout(parent);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(0);

    // top bar that contains title and search
    QWidget *topBar = new QWidget;
    topBar->setObjectName("sidebarTop");
    QVBoxLayout *topLayout = new QVBoxLayout(topBar);
    topLayout->setContentsMargins(14, 14, 14, 10);
    topLayout->setSpacing(10);

    QLabel *title = new QLabel("Messages");
    title->setObjectName("sidebarTitle");

    m_searchBox = new QLineEdit;
    m_searchBox->setPlaceholderText("Search...");
    m_searchBox->setObjectName("searchBox");
    m_searchBox->setFixedHeight(32);

    topLayout->addWidget(title);
    topLayout->addWidget(m_searchBox);

    // contacts
    m_contactList = new QListWidget;
    m_contactList->setObjectName("contactList");
    m_contactList->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    //dummy data: remove when real data is usedddd
    m_contactList->addItem("Jerome");
    m_contactList->addItem("Jerome");
    m_contactList->addItem("Jerome");
    m_contactList->addItem("Jerome");

    // account bar at the bottom
    QWidget *accountBar = new QWidget;
    accountBar->setObjectName("accountBar");
    QHBoxLayout *accLayout = new QHBoxLayout(accountBar);
    accLayout->setContentsMargins(14, 10, 14, 10);

    QLabel *accountName = new QLabel("Jerome");
    accountName->setObjectName("accountName");

    accLayout->addWidget(accountName);
    accLayout->addStretch();

    layout->addWidget(topBar);
    layout->addWidget(m_contactList, 1);
    layout->addWidget(accountBar);
}

void MainWindow::setupChatPanel(QWidget *parent) {
    QVBoxLayout *layout = new QVBoxLayout(parent);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(0);

    // header of chat panel
    QWidget *header = new QWidget;
    header->setObjectName("chatHeader");
    header->setFixedHeight(56);
    QHBoxLayout *headerLayout = new QHBoxLayout(header);
    headerLayout->setContentsMargins(16, 0, 16, 0);
    headerLayout->setSpacing(10);

    QWidget *headerInfo = new QWidget;
    QVBoxLayout *infoLayout = new QVBoxLayout(headerInfo);
    infoLayout->setContentsMargins(0, 0, 0, 0);
    infoLayout->setSpacing(1);
    //dummy data
    m_headerName = new QLabel("Jerome");
    m_headerName->setObjectName("headerName");

    m_headerStatus = new QLabel("Online");
    m_headerStatus->setObjectName("headerStatus");

    infoLayout->addWidget(m_headerName);
    infoLayout->addWidget(m_headerStatus);

    headerLayout->addWidget(headerInfo, 1);

    //scroll area in chats
    m_scrollArea = new QScrollArea;
    m_scrollArea->setObjectName("chatScroll");
    m_scrollArea->setWidgetResizable(true);
    m_scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    QWidget *chatWidget = new QWidget;
    chatWidget->setObjectName("chatWidget");
    m_chatLayout = new QVBoxLayout(chatWidget);
    m_chatLayout->setContentsMargins(16, 16, 16, 16);
    m_chatLayout->setSpacing(8);
    m_chatLayout->addStretch(); // pushes messages to the bottom

    m_scrollArea->setWidget(chatWidget);

    // input bar
    QWidget *inputBar = new QWidget;
    inputBar->setObjectName("inputBar");
    inputBar->setFixedHeight(56);
    QHBoxLayout *inputLayout = new QHBoxLayout(inputBar);
    inputLayout->setContentsMargins(14, 0, 14, 0);
    inputLayout->setSpacing(8);

    m_inputBox = new QLineEdit;
    m_inputBox->setPlaceholderText("Write a message...");
    m_inputBox->setObjectName("inputBox");
    m_inputBox->setFixedHeight(36);

    m_sendBtn = new QPushButton("Send");
    m_sendBtn->setObjectName("sendBtn");
    m_sendBtn->setFixedSize(70, 36);

    connect(m_sendBtn, &QPushButton::clicked, this, &MainWindow::onSendMessage);
    connect(m_inputBox, &QLineEdit::returnPressed, this, &MainWindow::onSendMessage);

    inputLayout->addWidget(m_inputBox, 1);
    inputLayout->addWidget(m_sendBtn);

    layout->addWidget(header);
    layout->addWidget(m_scrollArea, 1);
    layout->addWidget(inputBar);
}

void MainWindow::onSendMessage() {
    QString text = m_inputBox->text().trimmed();
    if (text.isEmpty()) return;

    //bubble widget
    QWidget *row = new QWidget;
    QHBoxLayout *rowLayout = new QHBoxLayout(row);
    rowLayout->setContentsMargins(0, 0, 0, 0);

    QLabel *bubble = new QLabel(text);
    bubble->setObjectName("sentBubble");
    bubble->setWordWrap(true);
    bubble->setMaximumWidth(340);
    bubble->setContentsMargins(12, 8, 12, 8);

    rowLayout->addStretch();       // pushes bubble to the right
    rowLayout->addWidget(bubble);

    m_chatLayout->insertWidget(m_chatLayout->count() - 1, row);

    m_inputBox->clear();

    QScrollBar *sb = m_scrollArea->verticalScrollBar();
    sb->setValue(sb->maximum());
}
