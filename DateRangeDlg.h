#ifndef DateRangeDlg_H
#define DateRangeDlg_H

#include "Ui_DateRangeDlg.h"
#include <QSettings>

class DateRangeDlg : public QDialog, Ui_DateRangeDlg{

	Q_OBJECT
	//									//										//
public:
										DateRangeDlg							(const QString &WindowTitle, const QIcon &WindowIcon, const bool &UseStartDate, const bool &UseEndDate);
									   ~DateRangeDlg							();

   QDate								getStartDate							();
   QDate								getEndDate								();

public slots:

private:
	void								restoreState							();
	void								saveState								();

	QDate								StartDate;
	QDate								EndDate;

private slots:
	void								onOk									();
	//									//										//
};
#endif