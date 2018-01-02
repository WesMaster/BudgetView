#ifndef Model_H
#define Model_H

#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlQuery>
#include <QSqlQueryModel>
#include <QSqlRecord>
#include <QDate>
#include <QMessageBox>
#include <QObject>
#include <QSettings>
#include <QLocale>

#include "Calculations.h"

struct Relation
{
	int		Id;
	QString Name;
	QString IBAN;
};

struct Invoice
{
	int		Id;
	QString Type;
	int		RelationId;
	QDate	InvoiceDate;
	QString InvoiceNumber;
	QString Grossamount;
	int		VAT;
	QString VATamount;
	QString Nettamount;
	QString ExtraCosts;
	QDate	PayedAt;
	int	    VATYear;
	QString VATPeriod;
	QString Comment;
};

struct VAT
{
	QString PurchaseGrossamount;
	QString PurchaseVATamount;
	QString SaleGrossamount;
	QString SaleVATamount;
};

class Model{
	//									//										//
public:
										Model									();
									   ~Model									();

	void								addNewRelations							(QList<Relation> RelationsToAdd);
	void								deleteInvoice							(const int &Id);
	void								deleteRelation							(const int &Id);
	QStringList							getBalance								(const QString &EndDate);
	QStringList							getCash									(const QString &EndDate);
	QStringList							getSave 								(const QString &EndDate);
 QMap<QString, QPair<QString, QString>>	getCashflow								(const QString &StartDate, const QString &EndDate);
	Invoice								getInvoiceById							(const int &Id);
	Relation							getRelationById							(const int &Id);
	QList<Relation>						getRelations							();
	QPair<QString, QMap<QString, VAT>>	getVAT									(const QString &StartDate, const QString &EndDate);

	QSqlQueryModel*						loadInvoices							(const QString &WhereClause = "");
	QSqlQueryModel*						loadRelations							();
	bool								saveInvoice								(Invoice InvoiceData);
	void								setChecked								(const int &Id, const bool &State);
private:
	QSqlQueryModel					   *InvoiceModel;
	QSqlQueryModel                     *RelationModel;
	//									//										//
};
#endif
