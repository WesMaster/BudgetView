#include "DateRangeDlg.h"

DateRangeDlg::DateRangeDlg(const QString &WindowTitle, const QIcon &WindowIcon, const bool &UseStartDate, const bool &UseEndDate)
{
	QLocale::setDefault(QLocale(QLocale::Dutch, QLocale::Netherlands));

	setupUi(this);
	StartDateEdit->setDate(QDate::currentDate());
	EndDateEdit->setDate(QDate::currentDate());

	StartDateBox->setHidden(!UseStartDate);
	EndDateBox->setHidden(!UseEndDate);
	StartDateEdit->setHidden(!UseStartDate);
	EndDateEdit->setHidden(!UseEndDate);

	setWindowTitle(WindowTitle);
	setWindowIcon(WindowIcon);

	connect(OkButton, SIGNAL(clicked()), this, SLOT(onOk()));
}

DateRangeDlg::~DateRangeDlg()
{

}

void DateRangeDlg::restoreState()
{
	QSettings LocalSettings("./MainSettings", QSettings::IniFormat);
	restoreGeometry(LocalSettings.value("DateRangeDlgGeometry").toByteArray());
}

void DateRangeDlg::saveState()
{
	QSettings LocalSettings("./MainSettings", QSettings::IniFormat);
	LocalSettings.setValue("DateRangeDlgGeometry", this->saveGeometry());

	LocalSettings.sync();
}

void DateRangeDlg::onOk()
{
	if(StartDateBox->checkState() == Qt::Checked)
	{
		StartDate = StartDateEdit->date();
	}

	if(EndDateBox->checkState() == Qt::Checked)
	{
		EndDate = EndDateEdit->date();
	}

	accept();
}

QDate DateRangeDlg::getStartDate()
{
	return StartDate;
}

QDate DateRangeDlg::getEndDate()
{
	return EndDate;
}