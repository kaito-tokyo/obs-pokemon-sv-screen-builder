#include <QtWidgets>

class UpdateDialog : public QDialog {
	Q_OBJECT
public:
	UpdateDialog(std::string version, std::string body, QWidget *parent = nullptr);

private:
	QVBoxLayout *layout;
};
