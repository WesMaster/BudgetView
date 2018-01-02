#ifndef RelationsDlg_H
#define RelationsDlg_H

#include "Ui_RelationsDlg.h"
#include "Model.h"
#include <QSettings>
#include <QMenu>
#include <QCloseEvent>
#include "MySortFilterProxyModel.h"

class RelationsDlg : public QDialog, Ui_RelationsDlg{

	Q_OBJECT
	//									//										//
public:
										RelationsDlg							(Model *_ModelObject);
									   ~RelationsDlg							();

	Relation							getRelationById							(const int &Id);
public slots:

private:
	void								addRelations							(const QList<Relation> &RelationList);
	void								closeEvent								(QCloseEvent *Event);
	QList<Relation>						getNewRelations							();
	int									popUpYesNoQuestion						(const QString &Text, const QString &InformativeText);
	void								refresh									();
	void								restoreState							();
	void								saveState								();

	Model							   *ModelObject;
	MySortFilterProxyModel			   *ProxyModel;
	QList<Relation>						RelationList;
	QList<Relation>						RelationsToAddList;

private slots:
	void								onContextMenu							(const QPoint &Pos);
	bool								onReturnPressed							();
	void								onSave									();
	void								onTextChanged							(const QString &NewText);
	//									//										//
};
#endif