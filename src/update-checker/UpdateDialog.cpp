#include <QCheckBox>
#include <QLabel>
#include <QPushButton>
#include <QScrollArea>
#include <QString>
#include <QVBoxLayout>

#include <util/config-file.h>

#include "UpdateDialog.hpp"

static QString dialogContent =
	"<h1>ポケモンSVスクリーンビルダー - {version}! 🚀</h1>"
	"<p>新バージョンが利用可能です！以下のURLをクリックするとダウンロードページが表示されます！</o>"
	"<p><a href=\"https://github.com/royshil/obs-backgroundremoval/releases\">https://github.com/royshil/obs-backgroundremoval/releases</a></p>"
	"<h2>更新履歴</h2>";

UpdateDialog::UpdateDialog(const char *_pluginName, const char *_pluginVersion,
			   const char *latestVersion,
			   const char *latestChangelog, config_t *_config,
			   QWidget *parent = nullptr)
	: QDialog(parent),
	  layout(new QVBoxLayout),
	  config(_config),
	  pluginName(_pluginName),
	  pluginVersion(_pluginVersion)
{
	setWindowTitle("ポケモンSVスクリーンビルダー - 更新が利用可能！");
	setLayout(layout);
	QLabel *label = new QLabel(
		dialogContent.replace(QString("{version}"), latestVersion));
	label->setOpenExternalLinks(true);
	label->setTextInteractionFlags(Qt::TextBrowserInteraction);
	label->setTextFormat(Qt::RichText);
	label->setWordWrap(true);
	layout->addWidget(label);

	QScrollArea *scrollArea = new QScrollArea;
	QLabel *scrollAreaLabel = new QLabel(latestChangelog);
	scrollAreaLabel->setOpenExternalLinks(true);
	scrollAreaLabel->setTextInteractionFlags(Qt::TextBrowserInteraction);
	scrollAreaLabel->setTextFormat(Qt::MarkdownText);
	scrollAreaLabel->setWordWrap(true);
	scrollArea->setWidget(scrollAreaLabel);
	scrollArea->setWidgetResizable(true);
	layout->addWidget(scrollArea);

	// Add a checkbox to disable update checks
	QCheckBox *disableCheckbox = new QCheckBox("更新通知をオフにする");
	layout->addWidget(disableCheckbox);
	connect(disableCheckbox, &QCheckBox::stateChanged, [this](int state) {
		if (state == Qt::Unchecked) {
			return;
		}
		config_set_bool(config, pluginName.c_str(), "check_update_skip",
				true);
		config_set_string(config, pluginName.c_str(),
				  "check_update_skip_version",
				  pluginVersion.c_str());
	});

	// Add a button to close the dialog
	QPushButton *closeButton = new QPushButton("Close");
	layout->addWidget(closeButton);
	connect(closeButton, &QPushButton::clicked, this, &QDialog::close);
}
