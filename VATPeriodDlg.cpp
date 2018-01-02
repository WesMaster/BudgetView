#include "VATPeriodDlg.h"

VATPeriodDlg::VATPeriodDlg(const QString &WindowTitle, const QIcon &WindowIcon)
{
	QLocale::setDefault(QLocale(QLocale::Dutch, QLocale::Netherlands));

	setupUi(this);

	VATYearEdit->setDate(QDate::currentDate());

	setWindowTitle(WindowTitle);
	setWindowIcon(WindowIcon);

	connect(OkButton, SIGNAL(clicked()), this, SLOT(onOk()));
}

VATPeriodDlg::~VATPeriodDlg()
{

}

void VATPeriodDlg::restoreState()
{
	QSettings LocalSettings("./MainSettings", QSettings::IniFormat);
	restoreGeometry(LocalSettings.value("DateRangeDlgGeometry").toByteArray());
}

void VATPeriodDlg::saveState()
{
	QSettings LocalSettings("./MainSettings", QSettings::IniFormat);
	LocalSettings.setValue("DateRangeDlgGeometry", this->saveGeometry());

	LocalSettings.sync();
}

void VATPeriodDlg::onOk()
{
	VATYear = VATYearEdit->date().toString("yyyy").toInt();
	VATPeriod = VATPeriodBox->currentText();

	accept();
}

int VATPeriodDlg::getYear()
{
	return VATYear;
}

QString VATPeriodDlg::getPeriod()
{
	return VATPeriod;
}