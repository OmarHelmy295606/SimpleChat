#include "MainWindow.h"
#include <QSplitter>
#include <QScrollBar>
#include <QDateTime>
#include <QFrame>
#include <QMessageBox>
#include <QListWidgetItem>

MainWindow::MainWindow(const QString& username, QWidget *parent)
    : QMainWindow(parent) , username_(username)
{
    setWindowTitle("SimpleChat");
    setMinimumSize(800, 560);
    setupUI();

	if(m_accountName)
		m_accountName->setText(username_);
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

   connect(m_searchBox, &QLineEdit::textChanged, this, [=](const QString& text){
    for(int i = 0; i < m_contactList->count(); i++){
        QListWidgetItem* item = m_contactList->item(i);
        bool match = item->text().contains(text, Qt::CaseInsensitive);
        item->setHidden(!match);
    }
});

    // contacts
    m_contactList = new QListWidget;
    m_contactList->setObjectName("contactList");
    m_contactList->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    connect(m_contactList, &QListWidget::itemClicked,
            this, &MainWindow::onUserSelected);

    // account bar at the bottom
   QWidget *accountBar = new QWidget;
   accountBar->setObjectName("accountBar");
   QHBoxLayout *accLayout = new QHBoxLayout(accountBar);
    accLayout->setContentsMargins(14, 10, 14, 10);

    m_accountName = new QLabel(username_);
    m_accountName->setObjectName("accountName");

    QPushButton* aboutButton_ = new QPushButton("About");
    aboutButton_->setFixedHeight(28);

     accLayout->addWidget(m_accountName);
     accLayout->addStretch();
     accLayout->addWidget(aboutButton_);

     connect(aboutButton_, &QPushButton::clicked, this, &MainWindow::aboutRequested);

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


    m_headerName = new QLabel("Select a contact");
    m_headerName->setObjectName("headerName");

    m_headerStatus = new QLabel("");
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

	m_inputBox->setEnabled(false);
    m_sendBtn = new QPushButton("Send");
    m_sendBtn->setObjectName("sendBtn");
    m_sendBtn->setFixedSize(70, 36);

    m_sendBtn->setEnabled(false);

    connect(m_sendBtn, &QPushButton::clicked, this, &MainWindow::onSendMessage);
    connect(m_inputBox, &QLineEdit::returnPressed, this, &MainWindow::onSendMessage);

    inputLayout->addWidget(m_inputBox, 1);
    inputLayout->addWidget(m_sendBtn);

    layout->addWidget(header);
    layout->addWidget(m_scrollArea, 1);
    layout->addWidget(inputBar);
}

void MainWindow::onUserSelected(QListWidgetItem* item){
	activeRecipient_ = item->text();

	m_headerName->setText(activeRecipient_);
	m_headerStatus->setText("Online");

	QLayoutItem* child;
	while((child = m_chatLayout->takeAt(0)) != nullptr) {
		if(child->widget())
			child->widget()->deleteLater();
		delete child;
	}
	m_chatLayout->addStretch();
	QString history = conversations_.value(activeRecipient_, "");
	if(!history.isEmpty()){
		QStringList lines = history.split('\n', Qt::SkipEmptyParts);
		for (const QString& line : lines) {
			if(line.startsWith("SENT:"))
				addSentBubble(line.mid(5));
			else if(line.startsWith("RECV:")){
				int secondColon = line.indexOf(':',5);
				QString sender = line.mid(5, secondColon -5);
				QString text = line.mid(secondColon + 1);
				addReceivedBubble(sender, text);
			}
		}
	}

	m_inputBox->setEnabled(true);
	m_sendBtn->setEnabled(true);
	m_inputBox->setFocus();

	QScrollBar* sb = m_scrollArea->verticalScrollBar();
	sb->setValue(sb->maximum());
}

void MainWindow::onSendMessage() {
    if(activeRecipient_.isEmpty()) return;

    QString text = m_inputBox->text().trimmed();
    if (text.isEmpty()) return;

    m_inputBox->clear();

    conversations_[activeRecipient_] += "SENT:" + username_ + ":" + text + "\n";
    addSentBubble(text);

    emit messageSent(activeRecipient_, text);
}

void MainWindow::onMessageReceived(const Message& message){
	QString sender = QString::fromStdString(message.sender);
	QString content = QString::fromStdString(message.content);

	if(sender == username_) return;

	QString key = sender;

	conversations_[key] += "RECV:" + sender + ":" + content + "\n";
	if (key == activeRecipient_) {
		addReceivedBubble(sender, content);
	}


}

void MainWindow::onUserListUpdated(const QStringList& users){
	m_contactList->clear();
	for (const QString& user : users) {
		if(user == username_) continue;
		m_contactList->addItem(user);
	}

}

void MainWindow::onDisconnected(){
	QMessageBox::warning(this, "Disconnected", "Lost connection to the server.");
	m_inputBox->setEnabled(false);
	m_sendBtn->setEnabled(false);
	m_headerStatus->setText("Disconnected");
}

void MainWindow::addSentBubble(const QString& text){
	QWidget* row = new QWidget;
	QHBoxLayout* rowLayout = new QHBoxLayout(row);
	rowLayout->setContentsMargins(0, 0, 0, 0);

	QLabel* bubble = new QLabel(text);
	bubble->setObjectName("sentBubble");
	bubble->setWordWrap(true);
	bubble->setMaximumWidth(340);
	bubble->setContentsMargins(12, 8, 12, 8);

	rowLayout->addStretch();
	rowLayout->addWidget(bubble);
	m_chatLayout->insertWidget(m_chatLayout->count() - 1, row);
	QScrollBar* sb = m_scrollArea->verticalScrollBar();
	sb->setValue(sb->maximum());

}

void MainWindow::addReceivedBubble(const QString& sender, const QString& text){
	QWidget* row = new QWidget;
	QHBoxLayout* rowLayout = new QHBoxLayout(row);
	rowLayout->setContentsMargins(0, 0, 0, 0);
	QLabel* bubble = new QLabel("<b>" + sender + "</b><br>" + text);
	bubble->setObjectName("receivedBubble");
        bubble->setWordWrap(true);
        bubble->setMaximumWidth(340);
        bubble->setContentsMargins(12, 8, 12, 8);
	bubble->setTextFormat(Qt::RichText);
	rowLayout->addWidget(bubble);
	rowLayout->addStretch();

	m_chatLayout->insertWidget(m_chatLayout->count() - 1, row);
	QScrollBar* sb = m_scrollArea->verticalScrollBar();
	sb->setValue(sb->maximum());
}
