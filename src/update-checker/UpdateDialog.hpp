#include <QDialog>
#include <QVBoxLayout>

#include <util/config-file.h>

class UpdateDialog : public QDialog {
	Q_OBJECT
public:
	UpdateDialog(const char *_pluginName, const char *_pluginVersion,
		     const char *latestVersion, const char *latestChangelog,
		     config_t *_config, QWidget *parent);

private:
	QVBoxLayout *layout;
	config_t *config;
	std::string pluginName;
	std::string pluginVersion;
};
