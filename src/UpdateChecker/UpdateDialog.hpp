#include <QDialog>
#include <QVBoxLayout>

#include <util/config-file.h>

class UpdateDialog : public QDialog {
	Q_OBJECT
public:
	UpdateDialog(std::string latestVersion, std::string latestChangelog,
		     config_t *config, QWidget *parent);

private:
	QVBoxLayout *layout;
};
