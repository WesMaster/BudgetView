#ifndef BudgetView_H
#define BudgetView_H

#include "Ui_BudgetView.h"
#include "Ui_AnalysisDlg.h"
#include "RelationsDlg.h"
#include "NewInvoiceDlg.h"
#include "DateRangeDlg.h"
#include "VATPeriodDlg.h"
#include "Model.h"
#include "ItemDelegate.h"
#include "MySortFilterProxyModel.h"
#include <QtGui>

class BudgetView : public QMainWindow, Ui_BudgetView{

	Q_OBJECT
	//									//										//
public:
										BudgetView								();
									   ~BudgetView								();

public slots:

private:
	void								editInvoice								(const QModelIndex &Index);
	QPair<QString, QString>				popUpDateRangeDlg						(const QString &WindowTitle, const QIcon &Icon, const bool &UseStartDate = true, const bool &UseEndDate = true);
	int									popUpYesNoQuestion						(const QString &Text, const QString &InformativeText);
	void								restoreState							();
	void								saveState								();
	void								setupApplication						();
	void								startTest								();

	void								closeEvent								(QCloseEvent *aEvent);

	// Application functions and variables
	Model							   *ModelObject;
	MySortFilterProxyModel			   *ProxyModel;
	QLabel							   *BalanceLabel;
	QLabel							   *CashLabel;
	QLabel							   *SaveLabel;

private slots:
	void								onActionExit							();
	void								onActionRelations						();
	void								onActionNewInvoice						();
	void								onActionAbout							();
	void								onActionBalance							();
	void								onActionCashFlow						();
	void								onActionVAT								();
	void								onActionSearch							();
	void								onActionClear_search					();
	void								onDoubleClick							(const QModelIndex &Index);
	void								onContextMenu							(const QPoint &Pos);
	void								onSelectionChanged						(const QItemSelection &Selected, const QItemSelection &Deselected);
	void								quit									();
	void								refresh									();
	void								search									(const QString &Text);
	//									//										//
};
#endif // BudgetView_H
