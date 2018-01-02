#ifndef NewInvoiceDlg_H
#define NewInvoiceDlg_H

#include "Ui_NewInvoiceDlg.h"
#include "RelationsDlg.h"
#include "Model.h"
#include "Calculations.h"
#include <QSqlRecord>
#include <QCompleter>
#include <math.h>

class NewInvoiceDlg : public QDialog, Ui_NewInvoiceDlg{

	Q_OBJECT
	//									//										//
public:
										NewInvoiceDlg							(Model *_ModelObject);
									   ~NewInvoiceDlg							();

	void								setInvoiceData							(Invoice InvoiceData);

public slots:

private:
//	QString								getVATAmount							(const QString &GrossAmount, const QString &VAT);
//	QString								getNettAmount							(const QString &GrossAmount, const QString &VAT);
	int									getRelationId							(const QString &RelationName);
	QString								getRelationName							(const int &RelationId);
	void								loadRelations							();
	void								restoreState							();
	void								saveState								();

	Model							   *ModelObject;
	QList<Relation>						RelationList;
	QCompleter						   *Completer;
private slots:
	void								onSave									();
	void								onVATChanged							(int State);
	void								onNettChanged							(int State);
	void								onAmountChanged							(const QString &Value);
	void								onInvoiceDateChanged					(const QDate &Date);
	//									//										//
};
#endif