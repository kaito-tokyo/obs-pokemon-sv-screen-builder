#include <QDialog>
#include <QVBoxLayout>

#include <util/config-file.h>

class UpdateDialog : public QDialog {
	Q_OBJECT
public:
	UpdateDialog(const char *pluginName, const char *latestVersion,
		     const char *latestChangelog, config_t *config,
		     QWidget *parent);

private:
	QVBoxLayout *layout;
};
