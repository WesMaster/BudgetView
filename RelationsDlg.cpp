#include "RelationsDlg.h"

RelationsDlg::RelationsDlg(Model *_ModelObject)
: ModelObject(_ModelObject)
{
	QLocale::setDefault(QLocale(QLocale::Dutch, QLocale::Netherlands));

	setupUi(this);

	ProxyModel = new MySortFilterProxyModel(this);
	ProxyModel->setFilterKeyColumn(2);

	NameEdit->setFocus();

	connect(SaveButton, SIGNAL(clicked()), this, SLOT(onSave()));
	connect(NameEdit, SIGNAL(returnPressed()), this, SLOT(onReturnPressed()));
	connect(NameEdit, SIGNAL(textChanged(const QString&)), this, SLOT(onTextChanged(const QString&)));
	connect(IBANEdit, SIGNAL(returnPressed()), this, SLOT(onReturnPressed()));
	connect(RelationsTW, SIGNAL(customContextMenuRequested(const QPoint&)), this, SLOT(onContextMenu(const QPoint&)));

	restoreState();

	refresh();
}

RelationsDlg::~RelationsDlg()
{

}

void RelationsDlg::closeEvent(QCloseEvent *Event)
{
	saveState();
	Event->accept();
}

void RelationsDlg::onContextMenu(const QPoint &Pos)
{
	QMenu Menu;

	QIcon EditIcon;
    EditIcon.addFile(QString::fromUtf8(":/Icons/document-edit.png"), QSize(), QIcon::Normal, QIcon::Off);
	QAction *EditRelation = new QAction(EditIcon, tr("Edit relation"), this);

	QIcon DeleteIcon;
    DeleteIcon.addFile(QString::fromUtf8(":/Icons/edit-delete.png"), QSize(), QIcon::Normal, QIcon::Off);
	QAction *DeleteRelation = new QAction(DeleteIcon, tr("Delete relation"), this);

	QModelIndex Index = RelationsTW->indexAt(Pos);
	if(Index.isValid())
	{
		Menu.addAction(EditRelation);
		Menu.addSeparator();
		Menu.addAction(DeleteRelation);
	}
	else
	{
		
	}

	QAction *Act = Menu.exec(RelationsTW->mapToGlobal(Pos));
	if(Act == EditRelation)
	{
		QModelIndexList List = RelationsTW->selectionModel()->selectedIndexes();
		IDEdit->setText(RelationsTW->model()->data(List.at(0)).toString());
		NameEdit->setText(RelationsTW->model()->data(List.at(2)).toString());
		IBANEdit->setText(RelationsTW->model()->data(List.at(3)).toString());				
	}
	else if(Act == DeleteRelation)
	{
		if(popUpYesNoQuestion(tr("Deleting relation"), tr("Are you sure you want to delete the selected relation?")) == QMessageBox::Yes)
		{
			QModelIndexList List = RelationsTW->selectionModel()->selectedIndexes();
			ModelObject->deleteRelation(RelationsTW->model()->data(List.at(0)).toInt());
			refresh();
		}
	}

	delete EditRelation;
	delete DeleteRelation;
}

void RelationsDlg::refresh()
{
	ProxyModel->setSourceModel(ModelObject->loadRelations());
    RelationsTW->setModel(ProxyModel);

	//RelationsTW->setModel(ModelObject->loadRelations());
}

void RelationsDlg::onSave()
{
	saveState();

	onReturnPressed();

	QList<Relation> NewRelationsList = getNewRelations();
	ModelObject->addNewRelations(NewRelationsList);

	NewRelationsTW->clear();
	refresh();
}

Relation RelationsDlg::getRelationById(const int &Id)
{
	Relation CurrRel;
	for(int i = 0; i < RelationList.size(); i++)
	{
		if(RelationList.at(i).Id == Id)
		{
			CurrRel = RelationList.at(i);
			break;
		}
	}
	return CurrRel;
}

void RelationsDlg::addRelations(const QList<Relation> &NewRelationList)
{
	QList<QTreeWidgetItem*> ItemList;
	for(int i = 0; i < NewRelationList.size(); i++)
	{
		QTreeWidgetItem *Item = new QTreeWidgetItem();
		Item->setText(0, QString::number(NewRelationList.at(i).Id));
		Item->setText(1, NewRelationList.at(i).Name);
		Item->setText(2, NewRelationList.at(i).IBAN);

		ItemList << Item;
	}
	NewRelationsTW->addTopLevelItems(ItemList);
}

bool RelationsDlg::onReturnPressed()
{
	QString Name = NameEdit->text();
	QString IBAN = IBANEdit->text();
	int Id = 0;
	if(!IDEdit->text().isEmpty())
	{
		Id = IDEdit->text().toInt();
	}

	if(Name.isEmpty())
	{
		return false;
	}
	else
	{
		Relation NewRel;
		NewRel.Id = Id;
		NewRel.Name = Name;
		NewRel.IBAN = IBAN;
		RelationsToAddList << NewRel;

		addRelations(RelationsToAddList);
		NameEdit->clear();
		IBANEdit->clear();
		RelationsToAddList.clear();
		NameEdit->setFocus();

		return true;
	}
}

void RelationsDlg::restoreState()
{
	QSettings LocalSettings("./MainSettings", QSettings::IniFormat);
	restoreGeometry(LocalSettings.value("RelationsDlgGeometry").toByteArray());

	QList<QSplitter *> AllSplitters = this->findChildren<QSplitter *>();
	for(int i = 0; i < AllSplitters.size(); i++)
	{
		QString State = LocalSettings.value("RelationsDlg_State_" + AllSplitters.at(i)->objectName()).toString();
		AllSplitters.at(i)->restoreState(QByteArray::fromBase64(State.toLatin1()));
	}

	RelationsTW->header()->restoreState(LocalSettings.value(QString("Headers/%1").arg(RelationsTW->objectName())).toByteArray());
	NewRelationsTW->header()->restoreState(LocalSettings.value(QString("Headers/%1").arg(NewRelationsTW->objectName())).toByteArray());
}

void RelationsDlg::saveState()
{
	QSettings LocalSettings("./MainSettings", QSettings::IniFormat);
	LocalSettings.setValue("RelationsDlgGeometry", this->saveGeometry());

	QList<QSplitter *> AllSplitters = this->findChildren<QSplitter *>();
	for(int i = 0; i < AllSplitters.size(); i++)
	{
		LocalSettings.setValue("RelationsDlg_State_" + AllSplitters.at(i)->objectName(), AllSplitters.at(i)->saveState().toBase64());
	}

	QHeaderView *HeaderView = RelationsTW->header();
	LocalSettings.setValue(QString("Headers/%1").arg(RelationsTW->objectName()), HeaderView->saveState());

	QHeaderView *NewHeaderView = NewRelationsTW->header();
	LocalSettings.setValue(QString("Headers/%1").arg(NewRelationsTW->objectName()), NewHeaderView->saveState());

	LocalSettings.sync();
}

QList<Relation> RelationsDlg::getNewRelations()
{
	QList<Relation> ReturnList;
	int CountNew = NewRelationsTW->topLevelItemCount();
	for(int i = 0; i < CountNew; i++)
	{
		QTreeWidgetItem *Item = NewRelationsTW->topLevelItem(i);
		if(Item)
		{
			Relation NewRel;
			NewRel.Id = Item->text(0).toInt();
			NewRel.Name = Item->text(1);
			NewRel.IBAN = Item->text(2);

			ReturnList << NewRel;
		}
	}

	return ReturnList;
}

int RelationsDlg::popUpYesNoQuestion(const QString &Text, const QString &InformativeText)
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

void RelationsDlg::onTextChanged(const QString &NewText)
{
	ProxyModel->setFilterRegExp(QRegExp(NewText, Qt::CaseInsensitive, QRegExp::FixedString));
	ProxyModel->setFilterKeyColumn(2);
}