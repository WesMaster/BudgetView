#pragma once

#include <QDate>
#include <QSortFilterProxyModel>

class MySortFilterProxyModel : public QSortFilterProxyModel
{
    Q_OBJECT

public:
    MySortFilterProxyModel(QObject *parent = 0);

	QVariant data(const QModelIndex & index, int role = Qt::DisplayRole) const {
    if (role == Qt::TextAlignmentRole && RightAlignColumns.contains(index.column()))
		return Qt::AlignRight; //maybe different result depending on column/row
	else
      return QSortFilterProxyModel::data(index, role);
  }

    QDate filterMinimumDate() const { return minDate; }
    void setFilterMinimumDate(const QDate &date);

    QDate filterMaximumDate() const { return maxDate; }
    void setFilterMaximumDate(const QDate &date);
	QList<int> getDateColumns();

	public slots:
			void clearFilter();

protected:
    bool filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const;
    bool lessThan(const QModelIndex &left, const QModelIndex &right) const;

private:
    bool dateInRange(const QDate &date) const;
	QList<int>							DateColumns;
	QList<int>							RightAlignColumns;
	QList<int>							SortNumberColumns;
    QDate minDate;
    QDate maxDate;
};