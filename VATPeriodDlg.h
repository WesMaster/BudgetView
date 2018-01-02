#ifndef VATPeriodDlg_H
#define VATPeriodDlg_H

#include "Ui_VATPeriodDlg.h"
#include <QSettings>

class VATPeriodDlg : public QDialog, Ui_VATPeriodDlg{

	Q_OBJECT
	//									//										//
public:
										VATPeriodDlg							(const QString &WindowTitle, const QIcon &WindowIcon);
									   ~VATPeriodDlg							();

   int									getYear									();
   QString								getPeriod								();

public slots:

private:
	void								restoreState							();
	void								saveState								();

	int 								VATYear;
	QString								VATPeriod;

private slots:
	void								onOk									();
	//									//										//
};
#endif