#include "AboutWindow.h"

AboutWindow::AboutWindow(QWidget* parent) : QWidget(parent) {
	auto* titleLabel  = new QLabel("SimpleChat", this);
	auto* descLabel   = new QLabel("A lightweight local network\nchat application.", this);
	auto* techLabel   = new QLabel("Built with:\nC++17  •  Qt6  •  Boost.Asio", this);
	auto* closeButton = new QPushButton("Close", this);

	QFont titleFont = titleLabel->font();
	titleFont.setPointSize(20);
	titleFont.setBold(true);
	titleLabel->setFont(titleFont);
	titleLabel->setAlignment(Qt::AlignCenter);
	descLabel->setAlignment(Qt::AlignCenter);
	techLabel->setAlignment(Qt::AlignCenter);

	auto* layout = new QVBoxLayout(this);
	layout->setSpacing(16);
    	layout->setContentsMargins(40, 40, 40, 40);
    	layout->addStretch();
    	layout->addWidget(titleLabel);
    	layout->addWidget(descLabel);
    	layout->addWidget(techLabel);
    	layout->addSpacing(10);
    	layout->addWidget(closeButton);
    	layout->addStretch();

	setWindowTitle("About SimpleChat");
	setFixedSize(320, 280);
	connect(closeButton, &QPushButton::clicked, this, &AboutWindow::closeRequested);
}
