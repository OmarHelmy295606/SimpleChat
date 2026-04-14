#include "LoginWindow.h"

LoginWindow::LoginWindow(QWidget* parent)
: QWidget(parent){

    	titleLabel_     = new QLabel("SimpleChat", this);
	usernameLabel_  = new QLabel("Username:", this);
    	hostLabel_      = new QLabel("Server IP:", this);
    	usernameEdit_   = new QLineEdit(this);
    	hostEdit_       = new QLineEdit(this);
    	connectButton_  = new QPushButton("Connect", this);
	errorLabel_     = new QLabel("", this);

	QFont titleFont = titleLabel_->font();
	titleFont.setPointSize(24);
	titleFont.setBold(true);
	titleLabel_->setFont(titleFont);
	titleLabel_->setAlignment(Qt::AlignCenter);

	errorLabel_->setStyleSheet("color: red;");
	errorLabel_->setAlignment(Qt::AlignCenter);
	errorLabel_->hide();

	usernameEdit_->setPlaceholderText("Enter your username");
	hostEdit_->setPlaceholderText("e.g. 123.0.0.0");

	QHBoxLayout* usernameRow = new QHBoxLayout();
	usernameRow->addWidget(usernameLabel_);
	usernameRow->addWidget(usernameEdit_);

	QHBoxLayout* hostIPRow = new QHBoxLayout();
	hostIPRow->addWidget(hostLabel_);
	hostIPRow->addWidget(hostEdit_);

	QVBoxLayout* mainLayout = new QVBoxLayout();
	mainLayout->setSpacing(12);
	mainLayout->setContentsMargins(60,40,60,40);
	mainLayout->addStretch();
	mainLayout->addWidget(titleLabel_);
	mainLayout->addSpacing(20);
	mainLayout->addLayout(usernameRow);
	mainLayout->addLayout(hostIPRow);
	mainLayout->addSpacing(10);
	mainLayout->addWidget(connectButton_);
	mainLayout->addWidget(errorLabel_);
	mainLayout->addStretch();

	setLayout(mainLayout);
	setWindowTitle("SimpleChat - Login Page");
	setMinimumSize(400,300);

	connect(connectButton_, &QPushButton::clicked, this, &LoginWindow::onConnectClicked);
}



void LoginWindow::onConnectClicked(){
	QString username = usernameEdit_->text().trimmed();
	QString hostIP = hostEdit_->text().trimmed();
	if(username.isEmpty()){
		ErrorOccurred("Please Enter a username");
		return;
	}
	if(hostIP.isEmpty()){
		ErrorOccurred("Please Enter a server IP");
		return;
	}
	errorLabel_->hide();
	connectButton_->setEnabled(false);
	connectButton_->setText("Connecting ...");

	emit connectRequested(hostIP, 12345, username);
}

void LoginWindow::ErrorOccurred(const QString& errorMessage){
	errorLabel_->setText(errorMessage);
	errorLabel_->show();
	connectButton_->setEnabled(true);
	connectButton_->setText("Connect");
}

void LoginWindow::onConnected(){
	errorLabel_->hide();
}
