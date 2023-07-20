#include <QLabel>
#include <QString>
#include <QPushButton>
#include <QDialogButtonBox>

#include <obs-module.h>

#include "DefaultLayoutCreatedDialog.hpp"

DefaultLayoutCreatedDialog::DefaultLayoutCreatedDialog(QWidget *parent)
	: QDialog(parent)
{
	layout = new QVBoxLayout;
	setLayout(layout);

	QLabel *label = new QLabel(obs_module_text("DefaultLayoutCreatedDialogText"));
	layout->addWidget(label);

	buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok);
	layout->addWidget(buttonBox);
    connect(buttonBox, &QDialogButtonBox::accepted, this, &QDialog::accept);
}
