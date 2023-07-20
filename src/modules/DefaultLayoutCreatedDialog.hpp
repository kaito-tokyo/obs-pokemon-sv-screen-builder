#pragma once

#include <vector>

#include <QDialog>
#include <QVBoxLayout>
#include <QDialogButtonBox>

#include <util/config-file.h>

class DefaultLayoutCreatedDialog : public QDialog {
	Q_OBJECT
public:
	DefaultLayoutCreatedDialog(QWidget *parent = nullptr);
	void updateText(std::vector<const char *> sourceNames);

private:
	QVBoxLayout *layout;
	QDialogButtonBox *buttonBox;
};
