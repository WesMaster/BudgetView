#include "Model.h"

Model::Model()
{
	QLocale::setDefault(QLocale(QLocale::Dutch, QLocale::Netherlands));

	QSqlDatabase db = QSqlDatabase::addDatabase("QODBC");
	QSettings Settings("./BudgetView.ini", QSettings::IniFormat);

	db.setHostName(Settings.value("DB/HostName", "").toString());
	db.setDatabaseName(Settings.value("DB/Database", "").toString());
	db.setUserName(Settings.value("DB/UserName", "").toString());
	db.setPassword(Settings.value("DB/Password", "").toString());
	bool ok = db.open();

	if(!ok)
	{
		QMessageBox::information(NULL, Q_FUNC_INFO, db.lastError().text());
	}
	
	InvoiceModel = new QSqlQueryModel();
	RelationModel = new QSqlQueryModel();
}

Model::~Model()
{
	if(InvoiceModel != NULL)
	{
		delete InvoiceModel;
	}

	if(RelationModel != NULL)
	{
		delete RelationModel;
	}
}

QSqlQueryModel* Model::loadInvoices(const QString &WhereClause)
{
	// TODO try to get rid of this new everytime invoices are loaded
	if(InvoiceModel == NULL)
	{
		InvoiceModel = new QSqlQueryModel();
	}

	InvoiceModel->setQuery("SELECT invoices.ID, invoices.TimeChanged, RelationNumber, RelationName, InvoiceType, InvoiceNumber, InvoiceDate, GrossAmount, VAT, ExtraCost, PayedAt, Comment, VATAmount, NettAmount, Checked, VATYear, VATPeriod FROM invoices LEFT JOIN relations on RelationNumber = relations.Id " + WhereClause);
	InvoiceModel->setHeaderData(0, Qt::Horizontal, QObject::tr("ID"));
	InvoiceModel->setHeaderData(1, Qt::Horizontal, QObject::tr("TimeChanged"));
	InvoiceModel->setHeaderData(2, Qt::Horizontal, QObject::tr("RelationNumber"));
	InvoiceModel->setHeaderData(3, Qt::Horizontal, QObject::tr("RelationName"));
	InvoiceModel->setHeaderData(4, Qt::Horizontal, QObject::tr("InvoiceType"));
	InvoiceModel->setHeaderData(5, Qt::Horizontal, QObject::tr("InvoiceNumber"));
	InvoiceModel->setHeaderData(6, Qt::Horizontal, QObject::tr("InvoiceDate"));
	InvoiceModel->setHeaderData(7, Qt::Horizontal, QObject::tr("GrossAmount"));
	InvoiceModel->setHeaderData(8, Qt::Horizontal, QObject::tr("VAT"));
	InvoiceModel->setHeaderData(9, Qt::Horizontal, QObject::tr("ExtraCost"));
	InvoiceModel->setHeaderData(10, Qt::Horizontal, QObject::tr("PayedAt"));
	InvoiceModel->setHeaderData(11, Qt::Horizontal, QObject::tr("Comment"));
	InvoiceModel->setHeaderData(12, Qt::Horizontal, QObject::tr("VATAmount"));
	InvoiceModel->setHeaderData(13, Qt::Horizontal, QObject::tr("NettAmount"));
	InvoiceModel->setHeaderData(14, Qt::Horizontal, QObject::tr("Checked"));
	InvoiceModel->setHeaderData(15, Qt::Horizontal, QObject::tr("VATYear"));
	InvoiceModel->setHeaderData(16, Qt::Horizontal, QObject::tr("VATPeriod"));

	return InvoiceModel;
}

QSqlQueryModel* Model::loadRelations()
{
	if(RelationModel == NULL)
	{
		RelationModel = new QSqlQueryModel();
	}

	RelationModel->setQuery("SELECT ID, TimeChanged, RelationName, IBAN FROM relations");
	RelationModel->setHeaderData(0, Qt::Horizontal, QObject::tr("ID"));
	RelationModel->setHeaderData(1, Qt::Horizontal, QObject::tr("TimeChanged"));
	RelationModel->setHeaderData(2, Qt::Horizontal, QObject::tr("RelationName"));
	RelationModel->setHeaderData(3, Qt::Horizontal, QObject::tr("IBAN"));

	return RelationModel;
}

void Model::addNewRelations(QList<Relation> RelationsToAdd)
{
	for(int i = 0; i < RelationsToAdd.size(); i++)
	{
		QSqlQuery Query;
		if(RelationsToAdd.at(i).Id == 0)
		{
			Query.prepare("INSERT INTO relations (TimeChanged, RelationName, IBAN) "
					   "VALUES (GETDATE(), ?, ?)");
		}
		else
		{
			Query.prepare("UPDATE relations SET TimeChanged = GETDATE(), RelationName = ?, IBAN = ? "
						"WHERE ID = ?");
			Query.bindValue(2, RelationsToAdd.at(i).Id);
		}
		Query.bindValue(0, RelationsToAdd.at(i).Name);
		Query.bindValue(1, RelationsToAdd.at(i).IBAN);
		if(!Query.exec())
		{
			QMessageBox::warning(NULL, Q_FUNC_INFO, Query.lastError().text());
		}
	}
}

void Model::deleteRelation(const int &Id)
{
	QSqlQuery Query;
	Query.prepare("DELETE FROM relations WHERE ID = ?");
	Query.bindValue(0, Id);
	if(!Query.exec())
	{
		QMessageBox::warning(NULL, Q_FUNC_INFO, Query.lastError().text());
	}
}

QList<Relation> Model::getRelations()
{
	QList<Relation> RelationList;
	RelationModel = loadRelations();
	for(int i = 0; i < RelationModel->rowCount(); i++)
	{
		Relation Rel;
		Rel.Id = RelationModel->record(i).value("Id").toInt();
		Rel.Name = RelationModel->record(i).value("RelationName").toString();
		Rel.IBAN = RelationModel->record(i).value("IBAN").toString();

		RelationList << Rel;
	}
	return RelationList;
}

Relation Model::getRelationById(const int &Id)
{
	Relation ReturnRelation;
	RelationModel = loadRelations();
	for(int i = 0; i < RelationModel->rowCount(); i++)
	{
		if(RelationModel->record(i).value("Id").toInt() == Id)
		{
			ReturnRelation.Id = RelationModel->record(i).value("Id").toInt();
			ReturnRelation.Name = RelationModel->record(i).value("RelationName").toString();
			ReturnRelation.IBAN = RelationModel->record(i).value("IBAN").toString();
			break;
		}
	}
	return ReturnRelation;
}

bool Model::saveInvoice(Invoice InvoiceData)
{
	if(InvoiceData.RelationId != 0)
	{
		QSqlQuery Query;
		if(InvoiceData.Id == 0)
		{
			Query.prepare("INSERT INTO invoices (TimeChanged, RelationNumber, InvoiceType, "
				"InvoiceNumber, InvoiceDate, GrossAmount, VAT, ExtraCost, PayedAt, Comment, VATAmount, NettAmount, VATYear, VATPeriod) "
				"VALUES (GETDATE(), ?, ?, ?, ?, ? ,?, ?, ?, ?, ?, ?, ?, ?)");
		}
		else
		{
			Query.prepare("UPDATE invoices SET TimeChanged = GETDATE(), RelationNumber = ?, InvoiceType = ?, "
				"InvoiceNumber = ?, InvoiceDate = ?, GrossAmount = ?, VAT = ?, ExtraCost = ?, PayedAt =?, "
				"Comment = ?, VATAmount = ?, NettAmount = ?, VATYear = ?, VATPeriod = ? WHERE ID = ?");
			Query.bindValue(13, InvoiceData.Id);
		}
		Query.bindValue(0, InvoiceData.RelationId);
		Query.bindValue(1, InvoiceData.Type);
		Query.bindValue(2, InvoiceData.InvoiceNumber);
		Query.bindValue(3, InvoiceData.InvoiceDate.toString("yyyy-MM-dd"));
		Query.bindValue(4, InvoiceData.Grossamount);
		Query.bindValue(5, InvoiceData.VAT);
		Query.bindValue(6, InvoiceData.ExtraCosts);
		Query.bindValue(7, InvoiceData.PayedAt.toString("yyyy-MM-dd"));
		Query.bindValue(8, InvoiceData.Comment);
		Query.bindValue(9, InvoiceData.VATamount);
		Query.bindValue(10, InvoiceData.Nettamount);
		Query.bindValue(11, InvoiceData.VATYear);
		Query.bindValue(12, InvoiceData.VATPeriod);
		if(!Query.exec())
		{
			QMessageBox::warning(NULL, Q_FUNC_INFO, Query.lastError().text());
		}

		return true;
	}

	return false;
}

void Model::deleteInvoice(const int &Id)
{
	QSqlQuery Query;
	Query.prepare("DELETE FROM invoices WHERE ID = ?");
	Query.bindValue(0, Id);
	if(!Query.exec())
	{
		QMessageBox::warning(NULL, Q_FUNC_INFO, Query.lastError().text());
	}
}

Invoice Model::getInvoiceById(const int &Id)
{
	Invoice InvoiceData;
	InvoiceModel = loadInvoices();
	for(int i = 0; i < InvoiceModel->rowCount(); i++)
	{
		if(InvoiceModel->record(i).value("Id").toInt() == Id)
		{
			InvoiceData.Id = InvoiceModel->record(i).value("Id").toInt();
			InvoiceData.RelationId = InvoiceModel->record(i).value("RelationNumber").toInt();
			InvoiceData.Type = InvoiceModel->record(i).value("InvoiceType").toString();
			InvoiceData.InvoiceDate = InvoiceModel->record(i).value("InvoiceDate").toDate();
			InvoiceData.InvoiceNumber = InvoiceModel->record(i).value("InvoiceNumber").toString();
			InvoiceData.Grossamount = InvoiceModel->record(i).value("GrossAmount").toString();
			InvoiceData.VAT = InvoiceModel->record(i).value("VAT").toInt();
			InvoiceData.ExtraCosts = InvoiceModel->record(i).value("ExtraCost").toString();
			InvoiceData.PayedAt = InvoiceModel->record(i).value("PayedAt").toDate();
			InvoiceData.Comment = InvoiceModel->record(i).value("Comment").toString();
			InvoiceData.VATamount = InvoiceModel->record(i).value("VATAmount").toString();
			InvoiceData.Nettamount = InvoiceModel->record(i).value("NettAmount").toString();
			InvoiceData.VATYear = InvoiceModel->record(i).value("VATYear").toInt();
			InvoiceData.VATPeriod = InvoiceModel->record(i).value("VATPeriod").toString();
		}
	}
	return InvoiceData;
}

QPair<QString, QMap<QString, VAT>> Model::getVAT(const QString &StartDate, const QString &EndDate)
{
	QString WhereString = "WHERE ";
	QStringList WhereExtension;
	if(!StartDate.isEmpty())
	{
		//WhereExtension << "InvoiceDate >= '" + StartDate + "'";
		WhereExtension << "VATYear = " + StartDate;
	}
	if(!EndDate.isEmpty())
	{
		//WhereExtension << "InvoiceDate <= '" + EndDate + "'";
		WhereExtension << "VATPeriod = '" + EndDate + "'";
	}
	WhereString += WhereExtension.join(" AND ");
	InvoiceModel = loadInvoices(WhereString);
	//InvoiceModel = loadInvoices("WHERE InvoiceDate >= '" + StartDate + "' AND InvoiceDate <= '" + EndDate + "'");
	QString TotalVAT = "";
	QMap<QString, VAT> VATMap;
	for(int i = 0; i < InvoiceModel->rowCount(); i++)
	{
		QString Type = InvoiceModel->record(i).value("InvoiceType").toString();
		QString GrossAmount = InvoiceModel->record(i).value("GrossAmount").toString();
		QString VATString = InvoiceModel->record(i).value("VATAmount").toString();
		QString VATPerc = InvoiceModel->record(i).value("VAT").toString();

		VAT VATObject = VATMap[VATPerc];
		if(Type == QObject::tr("Sale"))
		{
			VATObject.SaleGrossamount = intToString(stringToInt(VATObject.SaleGrossamount) + stringToInt(GrossAmount));
			VATObject.SaleVATamount = intToString(stringToInt(VATObject.SaleVATamount) - stringToInt(VATString));
			
			TotalVAT = intToString(stringToInt(TotalVAT) - stringToInt(VATString));
		}
		else if(Type == QObject::tr("Purchase"))
		{
			VATObject.PurchaseGrossamount = intToString(stringToInt(VATObject.PurchaseGrossamount) + stringToInt(GrossAmount));
			VATObject.PurchaseVATamount = intToString(stringToInt(VATObject.PurchaseVATamount) + stringToInt(VATString));

			TotalVAT = intToString(stringToInt(TotalVAT) + stringToInt(VATString));
		}
		VATMap[VATPerc] = VATObject;
	}
	return qMakePair(TotalVAT, VATMap);
}

QStringList Model::getBalance(const QString &EndDate)
{
	InvoiceModel = loadInvoices("WHERE PayedAt <= '" + EndDate + "'");
	QString Balance = "";
	QString NettSale = "";
	QString NettPurchase = "";
	for(int i = 0; i < InvoiceModel->rowCount(); i++)
	{
		QString Type = InvoiceModel->record(i).value("InvoiceType").toString();

		int NettInt = stringToInt(InvoiceModel->record(i).value("NettAmount").toString());
		int ExtraCostInt = stringToInt(InvoiceModel->record(i).value("ExtraCost").toString());

		if(Type == QObject::tr("Sale"))
		{
			Balance = intToString(stringToInt(Balance) + NettInt + ExtraCostInt);
			NettSale = intToString(stringToInt(NettSale) + NettInt + ExtraCostInt);
		}
		else if(Type == QObject::tr("Purchase"))
		{
			Balance = intToString(stringToInt(Balance) - NettInt - ExtraCostInt);
			NettPurchase = intToString(stringToInt(NettPurchase) + NettInt + ExtraCostInt);
		}
		else if(Type == QObject::tr("Cash deposit"))
		{
			Balance = intToString(stringToInt(Balance) + NettInt + ExtraCostInt);
		}
		else if(Type == QObject::tr("Cash withdrawal"))
		{
			Balance = intToString(stringToInt(Balance) - NettInt - ExtraCostInt);
		}
	}

	QStringList SaveList = getSave(EndDate);

	Balance = intToString(stringToInt(Balance) - stringToInt(SaveList.at(0)));

	QStringList ReturnList = QStringList() << Balance << NettSale << NettPurchase;
	return ReturnList;
}

QMap<QString, QPair<QString, QString>> Model::getCashflow(const QString &StartDate, const QString &EndDate)
{
	QMap<QString, QPair<QString, QString>> ReturnMap;
	InvoiceModel = loadInvoices("WHERE PayedAt >= '" + StartDate + "' AND PayedAt <= '" + EndDate + "'");
	for(int i = 0; i < InvoiceModel->rowCount(); i++)
	{
		QPair<QString, QString> RelationData = ReturnMap[InvoiceModel->record(i).value("RelationName").toString()];
		QString Type = InvoiceModel->record(i).value("InvoiceType").toString();

		int NettInt = stringToInt(InvoiceModel->record(i).value("NettAmount").toString());
		int ExtraCostInt = stringToInt(InvoiceModel->record(i).value("ExtraCost").toString());

		if(Type == QObject::tr("Sale"))
		{
			RelationData.first = intToString(stringToInt(RelationData.first) + NettInt + ExtraCostInt);
		}
		else if(Type == QObject::tr("Purchase"))
		{
			RelationData.second = intToString(stringToInt(RelationData.second) + NettInt + ExtraCostInt);
		}
		ReturnMap[InvoiceModel->record(i).value("RelationName").toString()] = RelationData;
	}
	
	return ReturnMap;
}

void Model::setChecked(const int &Id, const bool &State)
{
	QSqlQuery Query;
	Query.prepare("UPDATE invoices SET Checked = ? WHERE ID = ?");
	Query.bindValue(0, State);
	Query.bindValue(1, Id);
	if(!Query.exec())
	{
		QMessageBox::warning(NULL, Q_FUNC_INFO, Query.lastError().text());
	}
}

QStringList Model::getCash(const QString &EndDate)
{
	InvoiceModel = loadInvoices("WHERE PayedAt <= '" + EndDate + "'");
	QString Balance = "";
	QString Purchase = "";
	QString Sale = "";
	for(int i = 0; i < InvoiceModel->rowCount(); i++)
	{
		QString Type = InvoiceModel->record(i).value("InvoiceType").toString();

		int NettInt = stringToInt(InvoiceModel->record(i).value("NettAmount").toString());
		int ExtraCostInt = stringToInt(InvoiceModel->record(i).value("ExtraCost").toString());

		if(Type == QObject::tr("Cash deposit") || Type == QObject::tr("Cash purchase"))
		{
			Balance = intToString(stringToInt(Balance) - NettInt - ExtraCostInt);
		}
		else if(Type == QObject::tr("Cash withdrawal") || Type == QObject::tr("Cash sale"))
		{
			Balance = intToString(stringToInt(Balance) + NettInt + ExtraCostInt);
		}
	}

	QStringList ReturnList = QStringList() << Balance << Purchase << Sale;
	return ReturnList;
}

QStringList Model::getSave(const QString &EndDate)
{
	InvoiceModel = loadInvoices("WHERE PayedAt <= '" + EndDate + "'");
	QString Balance = "";
	QString NettDeposit = "";
	QString NettWithdrawal = "";
	for(int i = 0; i < InvoiceModel->rowCount(); i++)
	{
		QString Type = InvoiceModel->record(i).value("InvoiceType").toString();

		int NettInt = stringToInt(InvoiceModel->record(i).value("NettAmount").toString());
		int ExtraCostInt = stringToInt(InvoiceModel->record(i).value("ExtraCost").toString());

		if(Type == QObject::tr("Save deposit"))
		{
			Balance = intToString(stringToInt(Balance) + NettInt + ExtraCostInt);
			NettDeposit = intToString(stringToInt(NettDeposit) + NettInt + ExtraCostInt);
		}
		else if(Type == QObject::tr("Save withdrawal"))
		{
			Balance = intToString(stringToInt(Balance) - NettInt - ExtraCostInt);
			NettWithdrawal = intToString(stringToInt(NettWithdrawal) + NettInt + ExtraCostInt);
		}
	}

	QStringList ReturnList = QStringList() << Balance << NettDeposit << NettWithdrawal;
	return ReturnList;
}