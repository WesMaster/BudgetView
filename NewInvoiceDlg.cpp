#include "NewInvoiceDlg.h"

NewInvoiceDlg::NewInvoiceDlg(Model *_ModelObject)
: ModelObject(_ModelObject)
{
	QLocale::setDefault(QLocale(QLocale::Dutch, QLocale::Netherlands));

	setupUi(this);

	connect(SaveButton, SIGNAL(clicked()), this, SLOT(onSave()));
	connect(VATCheck, SIGNAL(stateChanged(int)), this, SLOT(onVATChanged(int)));
	connect(NettCheck, SIGNAL(stateChanged(int)), this, SLOT(onNettChanged(int)));
	connect(GrossamountSpin, SIGNAL(valueChanged(const QString&)), this, SLOT(onAmountChanged(const QString&)));
	connect(VATBox, SIGNAL(currentIndexChanged(const QString&)), this, SLOT(onAmountChanged(const QString&)));
	connect(VATamountSpin, SIGNAL(valueChanged(const QString&)), this, SLOT(onAmountChanged(const QString&)));
	connect(InvoiceDateEdit, SIGNAL(dateChanged(const QDate&)), this, SLOT(onInvoiceDateChanged(const QDate&)));

	InvoiceDateEdit->setDate(QDate::currentDate());
	PayedDateEdit->setDate(QDate::currentDate());

	RelationList = ModelObject->getRelations();
	QStringList RelationsForCompleter;
	for(int i = 0; i < RelationList.size(); i++)
	{
		RelationsForCompleter << RelationList.at(i).Name;
	}

/*	QStringListModel *StringListModel;
    StringListModel = (QStringListModel*)(Completer->model());
    if(StringListModel == NULL)
	{
        StringListModel = new QStringListModel();
	}
    StringListModel->setStringList(RelationsForCompleter);*/

	Completer = new QCompleter(RelationsForCompleter, this);
	//Completer->setModel(StringListModel);
	Completer->setCaseSensitivity(Qt::CaseInsensitive);
	RelationBox->addItems(RelationsForCompleter);
	RelationBox->setCompleter(Completer);

	restoreState();
}

NewInvoiceDlg::~NewInvoiceDlg()
{
	if(Completer != NULL)
	{
		delete Completer;
	}
}

void NewInvoiceDlg::onSave()
{
	Invoice NewInvoiceData;
	NewInvoiceData.Id = IDEdit->text().toInt();
	NewInvoiceData.Type = TypeBox->currentText();
	NewInvoiceData.RelationId = getRelationId(RelationBox->currentText());
	NewInvoiceData.InvoiceDate = InvoiceDateEdit->date();
	NewInvoiceData.InvoiceNumber = InvoiceNumberEdit->text();
	NewInvoiceData.Grossamount = GrossamountSpin->cleanText();
	NewInvoiceData.VAT = VATBox->currentText().toInt();
	NewInvoiceData.VATamount = VATamountSpin->cleanText();
	NewInvoiceData.Nettamount = NettamountSpin->cleanText();
	NewInvoiceData.ExtraCosts = ExtraCostSpin->cleanText();
	NewInvoiceData.PayedAt = PayedDateEdit->date();
	NewInvoiceData.VATYear = VATYearEdit->date().year();
	NewInvoiceData.VATPeriod = VATPeriodBox->currentText();
	NewInvoiceData.Comment = CommentEdit->toPlainText();

	saveState();
	if(ModelObject->saveInvoice(NewInvoiceData))
	{
		accept();
	}
	else
	{
		reject();	
	}
}

int NewInvoiceDlg::getRelationId(const QString &RelationName)
{
	int ReturnId = 0;
	for(int i = 0; i < RelationList.size(); i++)
	{
		if(RelationList.at(i).Name == RelationName)
		{
			ReturnId = RelationList.at(i).Id;
			break;
		}
	}

	return ReturnId;
}

QString NewInvoiceDlg::getRelationName(const int &RelationId)
{
	QString ReturnName = "";
	for(int i = 0; i < RelationList.size(); i++)
	{
		if(RelationList.at(i).Id == RelationId)
		{
			ReturnName = RelationList.at(i).Name;
			break;
		}
	}

	return ReturnName;
}

void NewInvoiceDlg::setInvoiceData(Invoice InvoiceData)
{
	if(InvoiceData.VATamount != getVATAmount(InvoiceData.Grossamount, QString::number(InvoiceData.VAT)))
	{
		VATCheck->setChecked(true);
	}
	if(InvoiceData.Nettamount != getNettAmount(InvoiceData.Grossamount, InvoiceData.VATamount))
	{
		NettCheck->setChecked(true);
	}

	IDEdit->setText(QString::number(InvoiceData.Id));
	TypeBox->setCurrentIndex(TypeBox->findText(InvoiceData.Type));
	RelationBox->setCurrentIndex(RelationBox->findText(getRelationName(InvoiceData.RelationId)));
	InvoiceDateEdit->setDate(InvoiceData.InvoiceDate);
	InvoiceNumberEdit->setText(InvoiceData.InvoiceNumber);
	GrossamountSpin->setValue(GrossamountSpin->valueFromText(InvoiceData.Grossamount));
	VATBox->setCurrentIndex(VATBox->findText(QString::number(InvoiceData.VAT)));
	VATamountSpin->setValue(VATamountSpin->valueFromText(InvoiceData.VATamount));
	NettamountSpin->setValue(NettamountSpin->valueFromText(InvoiceData.Nettamount));
	ExtraCostSpin->setValue(ExtraCostSpin->valueFromText(InvoiceData.ExtraCosts));
	PayedDateEdit->setDate(InvoiceData.PayedAt);
	VATYearEdit->setDate(QDate(InvoiceData.VATYear, 1, 1));
	VATPeriodBox->setCurrentIndex(VATPeriodBox->findText(InvoiceData.VATPeriod));
	CommentEdit->setText(InvoiceData.Comment);

	setWindowTitle(tr("Edit invoice"));
	QIcon EditIcon;
    EditIcon.addFile(QString::fromUtf8(":/Icons/document-edit.png"), QSize(), QIcon::Normal, QIcon::Off);
	setWindowIcon(EditIcon);
}

void NewInvoiceDlg::restoreState()
{
	QSettings LocalSettings("./MainSettings", QSettings::IniFormat);
	restoreGeometry(LocalSettings.value("NewInvoiceDlgGeometry").toByteArray());

	QList<QSplitter *> AllSplitters = this->findChildren<QSplitter *>();
	for(int i = 0; i < AllSplitters.size(); i++)
	{
		QString State = LocalSettings.value("NewInvoiceDlg_State_" + AllSplitters.at(i)->objectName()).toString();
		AllSplitters.at(i)->restoreState(QByteArray::fromBase64(State.toLatin1()));
	}
}

void NewInvoiceDlg::saveState()
{
	QSettings LocalSettings("./MainSettings", QSettings::IniFormat);
	LocalSettings.setValue("NewInvoiceDlgGeometry", this->saveGeometry());

	QList<QSplitter *> AllSplitters = this->findChildren<QSplitter *>();
	for(int i = 0; i < AllSplitters.size(); i++)
	{
		LocalSettings.setValue("NewInvoiceDlg_State_" + AllSplitters.at(i)->objectName(), AllSplitters.at(i)->saveState().toBase64());
	}
	LocalSettings.sync();
}

void NewInvoiceDlg::onVATChanged(int State)
{
	VATamountSpin->setEnabled(State);

	QString Value = "";
	onAmountChanged(Value);
}

void NewInvoiceDlg::onNettChanged(int State)
{
	NettamountSpin->setEnabled(State);

	QString Value = "";
	onAmountChanged(Value);
}

void NewInvoiceDlg::onAmountChanged(const QString &Value)
{
	QString GrossAmount = GrossamountSpin->cleanText();

	if(!VATCheck->isChecked())
	{
		QString VAT = VATBox->currentText();
		QString VATAmount = getVATAmount(GrossAmount, VAT);

		VATamountSpin->setValue(VATamountSpin->valueFromText(VATAmount));
	}
	
	if(!NettCheck->isChecked())
	{
		int GrossInt = stringToInt(GrossAmount);
		int NewVAT = stringToInt(VATamountSpin->cleanText());
		QString NettAmount = intToString(GrossInt + NewVAT);
		NettamountSpin->setValue(NettamountSpin->valueFromText(NettAmount));
	}
}

void NewInvoiceDlg::onInvoiceDateChanged(const QDate &Date)
{
	double Month = Date.month();
	QString Period = "Q" + QString::number(ceil(Month / 3));

	VATYearEdit->setDate(Date);
	VATPeriodBox->setCurrentIndex(VATPeriodBox->findText(Period));
}