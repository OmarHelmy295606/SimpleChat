#pragma once
#include <QWidget>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>

class LoginWindow : public QWidget{
	Q_OBJECT
public:
explicit LoginWindow(QWidget* parent = nullptr);

signals:
void connectRequested(const QString& host, int port, const QString& username);

public slots:
void ErrorOccurred(const QString& errorMessage);
void onConnected();

private:
QLabel* titleLabel_;
QLabel* usernameLabel_;
QLabel* hostLabel_;
QLineEdit* usernameEdit_;
QLineEdit* hostEdit_;
QPushButton* connectButton_;
QLabel* errorLabel_;

private slots:
void onConnectClicked();

};
