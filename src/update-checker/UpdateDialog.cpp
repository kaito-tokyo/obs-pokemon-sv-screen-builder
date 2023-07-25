#include <QCheckBox>
#include <QLabel>
#include <QPushButton>
#include <QScrollArea>
#include <QString>

#include "UpdateDialog.hpp"

static QString dialogContent =
	"<h1>ポケモンSVスクリーンビルダー - {version}! 🚀</h1>"
	"<p>新バージョンが利用可能です！以下のURLをクリックするとダウンロードページが表示されます！</o>"
	"<p><a href=\"https://github.com/umireon/obs-pokemon-sv-screen-builder/releases\">https://github.com/umireon/obs-pokemon-sv-screen-builder/releases</a></p>"
	"<h2>更新履歴</h2>";

UpdateDialog::UpdateDialog(std::string latestVersion,
			   std::string latestChangelog, config_t *config,
			   QWidget *parent = nullptr)
	: QDialog(parent), layout(new QVBoxLayout)
{
	setWindowTitle("ポケモンSVスクリーンビルダー - 更新が利用可能！");
	setLayout(layout);
	QLabel *label = new QLabel(dialogContent.replace(
		QString("{version}"), latestVersion.c_str()));
	label->setOpenExternalLinks(true);
	label->setTextInteractionFlags(Qt::TextBrowserInteraction);
	label->setTextFormat(Qt::RichText);
	label->setWordWrap(true);
	layout->addWidget(label);

	QScrollArea *scrollArea = new QScrollArea;
	QLabel *scrollAreaLabel = new QLabel(latestChangelog.c_str());
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
	connect(disableCheckbox, &QCheckBox::stateChanged,
		[this, latestVersion, config](int state) {
			config_set_bool(config, "check-update", "skip",
					state != Qt::Unchecked);
			config_set_string(config, "check-update",
					  "skip-version",
					  latestVersion.c_str());
			config_save_safe(config, "tmp", nullptr);
		});

	// Add a button to close the dialog
	QPushButton *closeButton = new QPushButton("Close");
	layout->addWidget(closeButton);
	connect(closeButton, &QPushButton::clicked, this, &QDialog::close);
}
