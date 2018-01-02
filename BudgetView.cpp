#include "BudgetView.h"

BudgetView::BudgetView()
{
	ModelObject = new Model();
	ProxyModel = new MySortFilterProxyModel(this);

	//startTest();
	setupApplication();
}   // BudgetView()

BudgetView::~BudgetView()
{
	delete ModelObject;
	delete ProxyModel;
}   // ~BudgetView()

void BudgetView::startTest()
{
	QString V1 = "156,65";
	QString V2 = "21";

	QString VATAmount = getVATAmount(V1, V2);
	QString NettAmount = getNettAmount(V1, VATAmount);

	QString Result = "189,55";
	if(NettAmount != Result)
	{
		QMessageBox::warning(NULL, "Test failed", Result + " != " + NettAmount);
	}

	V1 = "13156,65";
	V2 = "21";

	VATAmount = getVATAmount(V1, V2);
	NettAmount = getNettAmount(V1, VATAmount);

	Result = "15919,55";
	if(NettAmount != Result)
	{
		QMessageBox::warning(NULL, "Test failed", Result + " != " + NettAmount);
	}
}

void BudgetView::closeEvent(QCloseEvent *aEvent)
{
	saveState();

	aEvent->accept();
}	// void closeEvent()

void BudgetView::quit()
{
	close();
}	// void quit()

void BudgetView::setupApplication()
{
	QLocale::setDefault(QLocale(QLocale::Dutch, QLocale::Netherlands));
	
	setupUi(this);

	connect(actionExit, SIGNAL(triggered()), this, SLOT(onActionExit()));
	connect(actionRelations, SIGNAL(triggered()), this, SLOT(onActionRelations()));
	connect(actionNew_invoice, SIGNAL(triggered()), this, SLOT(onActionNewInvoice()));
	connect(actionAbout_BudgetView, SIGNAL(triggered()), this, SLOT(onActionAbout()));
	connect(actionBalance, SIGNAL(triggered()), this, SLOT(onActionBalance()));
	connect(actionCashflow, SIGNAL(triggered()), this, SLOT(onActionCashFlow()));
	connect(actionVAT, SIGNAL(triggered()), this, SLOT(onActionVAT()));
	connect(invoicesView, SIGNAL(customContextMenuRequested(const QPoint&)), this, SLOT(onContextMenu(const QPoint&)));
	connect(actionSearch, SIGNAL(triggered()), this, SLOT(onActionSearch()));
	connect(actionClear_search, SIGNAL(triggered()), this, SLOT(onActionClear_search()));
	connect(invoicesView, SIGNAL(doubleClicked(const QModelIndex&)), this, SLOT(onDoubleClick(const QModelIndex&)));

	BalanceLabel = new QLabel();
	CashLabel = new QLabel();
	SaveLabel = new QLabel();

	restoreState();
	refresh();

	ItemDelegate *MyDelegate = new ItemDelegate(invoicesView);
	invoicesView->setItemDelegate(MyDelegate);

	show();
}   // void setupApplication()

void BudgetView::onActionExit()
{
	quit();
}

void BudgetView::onDoubleClick(const QModelIndex &Index)
{
	QModelIndexList List = invoicesView->selectionModel()->selectedIndexes();
	editInvoice(List.at(0));
}

void BudgetView::onActionClear_search()
{
	QHeaderView *Header = invoicesView->header();
	int SortColumn = Header->sortIndicatorSection();
	Qt::SortOrder SortOrder = Header->sortIndicatorOrder();

	ProxyModel->clearFilter();

	invoicesView->sortByColumn(SortColumn, SortOrder);
}

void BudgetView::onActionSearch()
{
	QModelIndex CurrentIndex = invoicesView->currentIndex();
	ProxyModel->setFilterKeyColumn(CurrentIndex.column());

	QString WindowTitle = tr("Search");
	QIcon Icon;
	Icon.addFile(QString::fromUtf8(":/Icons/view-financial-list.png"), QSize(), QIcon::Normal, QIcon::Off);
		

	QList<int> DateColumns = ProxyModel->getDateColumns();
	if(DateColumns.contains(CurrentIndex.column()))
	{
		QPair<QString, QString> Dates = popUpDateRangeDlg(WindowTitle, Icon, true, true);

		ProxyModel->setFilterMinimumDate(QDate::fromString(Dates.first, "yyyy-MM-dd"));
		ProxyModel->setFilterMaximumDate(QDate::fromString(Dates.second, "yyyy-MM-dd"));
	}
	else
	{
		QInputDialog InputDialog;

		bool val = QObject::connect(&InputDialog,SIGNAL(textValueChanged(const QString &)),this,SLOT(search(const QString &)));
		InputDialog.setLabelText(tr("Search:"));
		InputDialog.setTextValue(CurrentIndex.data().toString());
		InputDialog.exec();
	}
}

void BudgetView::search(const QString &Text)
{
	QModelIndex CurrentIndex = invoicesView->currentIndex();
	ProxyModel->setFilterRegExp(QRegExp(Text, Qt::CaseInsensitive, QRegExp::FixedString));
	ProxyModel->setFilterKeyColumn(CurrentIndex.column());
}

void BudgetView::onActionRelations()
{
	RelationsDlg Dialog(ModelObject);
	if(Dialog.exec() == QDialog::Accepted)
	{
		
	}
}

void BudgetView::onActionNewInvoice()
{
	NewInvoiceDlg Dialog(ModelObject);
	if(Dialog.exec() == QDialog::Accepted)
	{
		refresh();
	}
}

void BudgetView::onActionAbout()
{
	QMessageBox::about(this, tr("About"), windowTitle() + tr("\n\u00A9 Wesley Jonker 2015"));
}

void BudgetView::onActionBalance()
{
	QString WindowTitle = tr("Balance analysis");
	QIcon Icon;
    Icon.addFile(QString::fromUtf8(":/Icons/view-expenses-categories.png"), QSize(), QIcon::Normal, QIcon::Off);
	QPair<QString, QString> Dates = popUpDateRangeDlg(WindowTitle, Icon, false, true);

	QString EndDate = Dates.second;
	if(EndDate.isEmpty())
	{
		return;
	}

	Ui::AnalysisDlg DlgUi;
	QDialog Dialog;
	DlgUi.setupUi(&Dialog);
	DlgUi.RangeLabel->setText(tr("Balance on: " + EndDate.toUtf8()));
	Dialog.setWindowTitle(WindowTitle);
	Dialog.setWindowIcon(Icon);

	DlgUi.DataWidget->clear();
	DlgUi.DataWidget->setHeaderLabels(QStringList() << "" << "");
	
	QSettings LocalSettings("./MainSettings", QSettings::IniFormat);
	Dialog.restoreGeometry(LocalSettings.value("BalanceAnalysisGeometry").toByteArray());
	DlgUi.DataWidget->header()->restoreState(LocalSettings.value(QString("Headers/BalanceAnalysis")).toByteArray());

	QList<QTreeWidgetItem*> DetailedList;

	QStringList DataList = ModelObject->getBalance(EndDate);
	QTreeWidgetItem *RevItem = new QTreeWidgetItem();
	RevItem->setText(0, tr("Revenues:"));
	RevItem->setText(1, DataList.at(1));

	QTreeWidgetItem *ExpItem = new QTreeWidgetItem();
	ExpItem->setText(0, tr("Expenditures:"));
	ExpItem->setText(1, DataList.at(2));

	QTreeWidgetItem *BalItem = new QTreeWidgetItem();
	BalItem->setText(0, tr("Balance:"));
	BalItem->setText(1, DataList.at(0));

	DetailedList << RevItem << ExpItem << BalItem;

	DlgUi.DataWidget->addTopLevelItems(DetailedList);
	if(Dialog.exec() == QDialog::Accepted)
	{
		LocalSettings.setValue("BalanceAnalysisGeometry", Dialog.saveGeometry());
		QHeaderView *HeaderView = DlgUi.DataWidget->header();
		LocalSettings.setValue(QString("Headers/BalanceAnalysis"), HeaderView->saveState());
		LocalSettings.sync();
	}
}

void BudgetView::onActionCashFlow()
{
	QString WindowTitle = tr("Cashflow analysis");
	QIcon Icon;
    Icon.addFile(QString::fromUtf8(":/Icons/view-financial-list.png"), QSize(), QIcon::Normal, QIcon::Off);
	QPair<QString, QString> Dates = popUpDateRangeDlg(WindowTitle, Icon, true, true);

	QString StartDate = Dates.first;
	QString EndDate = Dates.second;
	QMap<QString, QPair<QString, QString>> CashflowMap = ModelObject->getCashflow(StartDate, EndDate);

	Ui::AnalysisDlg DlgUi;
	QDialog Dialog;
	DlgUi.setupUi(&Dialog);
	DlgUi.RangeLabel->setText(tr("Daterange: " + StartDate.toUtf8() + " - " + EndDate.toUtf8()));
	Dialog.setWindowTitle(WindowTitle);
	Dialog.setWindowIcon(Icon);

	DlgUi.DataWidget->clear();
	DlgUi.DataWidget->setHeaderLabels(QStringList() << "" << "");

	QSettings LocalSettings("./MainSettings", QSettings::IniFormat);
	Dialog.restoreGeometry(LocalSettings.value("CashflowAnalysisGeometry").toByteArray());
	DlgUi.DataWidget->header()->restoreState(LocalSettings.value(QString("Headers/CashflowAnalysis")).toByteArray());

	QList<QTreeWidgetItem*> DetailedList;

	QMapIterator<QString, QPair<QString, QString>> i(CashflowMap);
	DlgUi.DataWidget->setHeaderLabels(QStringList() << "" << tr("Revenues") << tr("Expenditures"));
	while(i.hasNext())
	{
		i.next();
		QTreeWidgetItem *Item = new QTreeWidgetItem();
		Item->setText(0, i.key());
		Item->setText(1, i.value().first);
		Item->setText(2, i.value().second);

		DetailedList << Item;
	}

	DlgUi.DataWidget->addTopLevelItems(DetailedList);
	if(Dialog.exec() == QDialog::Accepted)
	{
		LocalSettings.setValue("CashflowAnalysisGeometry", Dialog.saveGeometry());
		QHeaderView *HeaderView = DlgUi.DataWidget->header();
		LocalSettings.setValue(QString("Headers/CashflowAnalysis"), HeaderView->saveState());
		LocalSettings.sync();
	}
}

void BudgetView::onActionVAT()
{
	QString WindowTitle = tr("VAT analysis");
	QIcon Icon;
    Icon.addFile(QString::fromUtf8(":/Icons/view-income-categories.png"), QSize(), QIcon::Normal, QIcon::Off);

	VATPeriodDlg VATDialog(WindowTitle, Icon);
	QString Year = "";
	QString Period = "";
	if(VATDialog.exec() == QDialog::Accepted)
	{
		Year = QString::number(VATDialog.getYear());
		Period = VATDialog.getPeriod();
	}

	QPair<QString, QMap<QString, VAT>> VATCalculation = ModelObject->getVAT(Year, Period);

	Ui::AnalysisDlg DlgUi;
	QDialog Dialog;
	DlgUi.setupUi(&Dialog);
	DlgUi.RangeLabel->setText(tr("VAT range: " + Year.toUtf8() + " - " + Period.toUtf8()));
	Dialog.setWindowTitle(WindowTitle);
	Dialog.setWindowIcon(Icon);

	DlgUi.DataWidget->clear();
	DlgUi.DataWidget->setHeaderLabels(QStringList() << "" << "");

	QSettings LocalSettings("./MainSettings", QSettings::IniFormat);
	Dialog.restoreGeometry(LocalSettings.value("VATAnalysisGeometry").toByteArray());
	DlgUi.DataWidget->header()->restoreState(LocalSettings.value(QString("Headers/VATAnalysis")).toByteArray());

	QList<QTreeWidgetItem*> DetailedList;

	QTreeWidgetItem *Item = new QTreeWidgetItem();
	Item->setText(0, tr("Total VAT:"));
	Item->setText(1, VATCalculation.first);

	QTreeWidgetItem *EmptyItem = new QTreeWidgetItem();

	QTreeWidgetItem *RevItem = new QTreeWidgetItem();
	RevItem->setText(0, tr("Revenues"));
	DetailedList << Item << EmptyItem << RevItem;

	QMap<QString, VAT> VATMap = VATCalculation.second;
	QMapIterator<QString, VAT> i(VATMap);
	while(i.hasNext())
	{
		i.next();
		QTreeWidgetItem *GrossItem = new QTreeWidgetItem();
		GrossItem->setText(0, tr("Grossamount with ") + i.key() + tr("% VAT:	"));
		GrossItem->setText(1, i.value().SaleGrossamount);

		QTreeWidgetItem *VATItem = new QTreeWidgetItem();
		VATItem->setText(0, tr("VAT ") + i.key() + "%:");
		VATItem->setText(1, i.value().SaleVATamount);

		DetailedList << GrossItem << VATItem;

		if(i.hasNext())
		{
			QTreeWidgetItem *Item = new QTreeWidgetItem();
			DetailedList << Item;
		}
	}
	i.toFront();
	QTreeWidgetItem *Empty2Item = new QTreeWidgetItem();

	QTreeWidgetItem *ExpItem = new QTreeWidgetItem();
	ExpItem->setText(0, tr("Expenditures"));

	DetailedList << Empty2Item << ExpItem;

	while(i.hasNext())
	{
		i.next();
		QTreeWidgetItem *GrossItem = new QTreeWidgetItem();
		GrossItem->setText(0, tr("Grossamount with ") + i.key() + tr("% VAT:	"));
		GrossItem->setText(1, i.value().PurchaseGrossamount);

		QTreeWidgetItem *VATItem = new QTreeWidgetItem();
		VATItem->setText(0, tr("VAT ") + i.key() + "%:");
		VATItem->setText(1, i.value().PurchaseVATamount);

		DetailedList << GrossItem << VATItem;

		if(i.hasNext())
		{
			QTreeWidgetItem *Item = new QTreeWidgetItem();
			DetailedList << Item;
		}
	}

	DlgUi.DataWidget->addTopLevelItems(DetailedList);
	if(Dialog.exec() == QDialog::Accepted)
	{
		LocalSettings.setValue("VATAnalysisGeometry", Dialog.saveGeometry());
		QHeaderView *HeaderView = DlgUi.DataWidget->header();
		LocalSettings.setValue(QString("Headers/VATAnalysis"), HeaderView->saveState());
		LocalSettings.sync();
	}
}

QPair<QString, QString> BudgetView::popUpDateRangeDlg(const QString &WindowTitle, const QIcon &Icon, const bool &UseStartDate, const bool &UseEndDate)
{
	DateRangeDlg Dialog(WindowTitle, Icon, UseStartDate, UseEndDate);

	if(Dialog.exec() == QDialog::Accepted)
	{
		QString StartDate = Dialog.getStartDate().toString("yyyy-MM-dd");
		QString EndDate = Dialog.getEndDate().toString("yyyy-MM-dd");

		return qMakePair(StartDate, EndDate);
	}

	return qMakePair(QString(""), QString(""));
}

void BudgetView::saveState()
{
	QSettings LocalSettings("./MainSettings", QSettings::IniFormat);
	LocalSettings.setValue("MainGeometry", this->saveGeometry());

	QList<QSplitter *> AllSplitters = this->findChildren<QSplitter *>();
	for(int i = 0; i < AllSplitters.size(); i++)
	{
		LocalSettings.setValue("Main_State_" + AllSplitters.at(i)->objectName(), AllSplitters.at(i)->saveState().toBase64());
	}

	QHeaderView *HeaderView = invoicesView->header();
	LocalSettings.setValue(QString("Headers/%1").arg(invoicesView->objectName()), HeaderView->saveState());

	LocalSettings.sync();
}

void BudgetView::restoreState()
{
	QSettings LocalSettings("./MainSettings", QSettings::IniFormat);
	restoreGeometry(LocalSettings.value("MainGeometry").toByteArray());

	QList<QSplitter *> AllSplitters = this->findChildren<QSplitter *>();
	for(int i = 0; i < AllSplitters.size(); i++)
	{
		QString State = LocalSettings.value("Main_State_" + AllSplitters.at(i)->objectName()).toString();
		AllSplitters.at(i)->restoreState(QByteArray::fromBase64(State.toLatin1()));
	}

	invoicesView->header()->restoreState(LocalSettings.value(QString("Headers/%1").arg(invoicesView->objectName())).toByteArray());
}

void BudgetView::onSelectionChanged(const QItemSelection &Selected, const QItemSelection &Deselected)
{
	QModelIndexList IndexList = Selected.indexes();
	if(IndexList.size() == 0)
	{
		return;
	}
	QString Data = invoicesView->model()->data(IndexList.at(11)).toString();
	
	commentsBox->setText(Data);

	detailedWidget->clear();
	QStringList DetailedList;
	QString RelationNumber = invoicesView->model()->data(IndexList.at(2)).toString();
	QString RelationName = invoicesView->model()->data(IndexList.at(3)).toString();
	
	QString IBAN = ModelObject->getRelationById(invoicesView->model()->data(IndexList.at(2)).toInt()).IBAN;
	QString Grossamount = tr("Grossamount:		") + invoicesView->model()->data(IndexList.at(7)).toString();
	QString VAT			= tr("VAT:		") + invoicesView->model()->data(IndexList.at(12)).toString();
	QString Nettamount  = tr("Nettamount:		") + invoicesView->model()->data(IndexList.at(13)).toString();
	QString ExtraCost   = tr("Extra costs:		") + invoicesView->model()->data(IndexList.at(9)).toString();

	
	int ExtraCostInt = stringToInt(invoicesView->model()->data(IndexList.at(9)).toString());
	int NettInt = stringToInt(invoicesView->model()->data(IndexList.at(13)).toString());
	int TotalInt = NettInt + ExtraCostInt;

	QString Total		= tr("Total:		") + intToString(TotalInt); 

	DetailedList << RelationNumber << RelationName << IBAN << "" << Grossamount << VAT << Nettamount << ExtraCost << Total;
	detailedWidget->addItems(DetailedList);
}

void BudgetView::refresh()
{
	ProxyModel->setSourceModel(ModelObject->loadInvoices());
    invoicesView->setModel(ProxyModel);
	//invoicesView->setModel(ModelObject->loadInvoices());
	connect(invoicesView->selectionModel(), SIGNAL(selectionChanged(const QItemSelection&, const QItemSelection&)), this, SLOT(onSelectionChanged(const QItemSelection&, const QItemSelection&)));

	BalanceLabel->setText(tr("Balance: ") + ModelObject->getBalance(QDate::currentDate().toString("yyyy-MM-dd")).at(0));
	Statusbar->addWidget(BalanceLabel, Statusbar->width());

	SaveLabel->setText(tr("Save: ") + ModelObject->getSave(QDate::currentDate().toString("yyyy-MM-dd")).at(0));
	Statusbar->addWidget(SaveLabel, Statusbar->width());

	CashLabel->setText(tr("Cash: ") + ModelObject->getCash(QDate::currentDate().toString("yyyy-MM-dd")).at(0));
	Statusbar->addWidget(CashLabel, Statusbar->width());
}

void BudgetView::onContextMenu(const QPoint &Pos)
{
	QMenu Menu;

	QIcon NewIcon;
    NewIcon.addFile(QString::fromUtf8(":/Icons/document-new.png"), QSize(), QIcon::Normal, QIcon::Off);
	QAction *NewInvoice = new QAction(NewIcon, tr("New invoice"), this);

	QIcon EditIcon;
    EditIcon.addFile(QString::fromUtf8(":/Icons/document-edit.png"), QSize(), QIcon::Normal, QIcon::Off);
	QAction *EditInvoice = new QAction(EditIcon, tr("Edit invoice"), this);

	QIcon DeleteIcon;
    DeleteIcon.addFile(QString::fromUtf8(":/Icons/edit-delete.png"), QSize(), QIcon::Normal, QIcon::Off);
	QAction *DeleteInvoice = new QAction(DeleteIcon, tr("Delete invoice"), this);

	QIcon RefreshIcon;
    RefreshIcon.addFile(QString::fromUtf8(":/Icons/view-refresh.png"), QSize(), QIcon::Normal, QIcon::Off);
	QAction *Refresh = new QAction(RefreshIcon, tr("Refresh"), this);

	QModelIndex Index = invoicesView->indexAt(Pos);

	QAction *Check = new QAction(tr("Set checked"), this);
	Check->setCheckable(true);
	QModelIndexList List = invoicesView->selectionModel()->selectedIndexes();
	if(Index.isValid())
	{
		QString ActionText = Check->text();
		if(invoicesView->model()->data(List.at(14)).toBool())
		{
			ActionText = tr("Set unchecked");
			Check->setChecked(true);
		}
		
		Check->setText(ActionText);
	}
	
	if(Index.isValid())
	{
		Menu.addAction(NewInvoice);
		Menu.addAction(EditInvoice);
		Menu.addSeparator();
		Menu.addAction(Check);
		Menu.addSeparator();
		Menu.addAction(DeleteInvoice);
		Menu.addSeparator();
		Menu.addAction(Refresh);
	}
	else
	{
		Menu.addAction(NewInvoice);
		Menu.addSeparator();
		Menu.addAction(Refresh);
	}

	QAction *Act = Menu.exec(invoicesView->mapToGlobal(Pos));
	if(Act == NewInvoice)
	{
		onActionNewInvoice();
	}
	else if(Act == EditInvoice)
	{
		editInvoice(List.at(0));
	}
	else if(Act == DeleteInvoice)
	{
		if(popUpYesNoQuestion(tr("Deleting invoice"), tr("Are you sure you want to delete the selected invoice?")) == QMessageBox::Yes)
		{
			ModelObject->deleteInvoice(invoicesView->model()->data(List.at(0)).toInt());
			refresh();
		}
	}
	else if(Act == Refresh)
	{
		refresh();
	}
	else if(Act == Check)
	{
		ModelObject->setChecked(invoicesView->model()->data(List.at(0)).toInt(), Check->isChecked());
		refresh();
	}

	delete NewInvoice;
	delete EditInvoice;
	delete DeleteInvoice;
	delete Refresh;
	delete Check;
}

int BudgetView::popUpYesNoQuestion(const QString &Text, const QString &InformativeText)
{
	QMessageBox MsgBox;
	MsgBox.setText(Text);
	MsgBox.setInformativeText(InformativeText);
	QIcon Icon;
    Icon.addFile(QString::fromUtf8(":/Icons/view-financial-transfer.png"), QSize(), QIcon::Normal, QIcon::Off);
	MsgBox.setWindowIcon(Icon);
	MsgBox.setIcon(QMessageBox::Question);
	MsgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
	MsgBox.setDefaultButton(QMessageBox::No);
	return MsgBox.exec();
}

void BudgetView::editInvoice(const QModelIndex &Index)
{
	NewInvoiceDlg Dialog(ModelObject);
	Invoice InvoiceData = ModelObject->getInvoiceById(invoicesView->model()->data(Index).toInt());

	Dialog.setInvoiceData(InvoiceData);
	if(Dialog.exec() == QDialog::Accepted)
	{
		refresh();
	}
}