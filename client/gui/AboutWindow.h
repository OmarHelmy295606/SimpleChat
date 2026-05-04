#pragma once
#include <QWidget>
#include <QLabel>
#include <QPushButton>
#include <QVBoxLayout>

class AboutWindow : public QWidget {
    Q_OBJECT
public:
    	explicit AboutWindow(QWidget* parent = nullptr);
signals:
	void closeRequested();
};
