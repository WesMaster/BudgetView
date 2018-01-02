#include "MySortFilterProxyModel.h"

MySortFilterProxyModel::MySortFilterProxyModel(QObject *parent)
    : QSortFilterProxyModel(parent)
{
	DateColumns << 6 << 10;
	SortNumberColumns << 0 << 2 << 7 << 8 << 9 << 12 << 13 << 15;
	RightAlignColumns << 7 << 8 << 9 << 12 << 13;
}

void MySortFilterProxyModel::setFilterMinimumDate(const QDate &date)
{
    minDate = date;
    invalidateFilter();
}

void MySortFilterProxyModel::setFilterMaximumDate(const QDate &date)
{
    maxDate = date;
    invalidateFilter();
}

bool MySortFilterProxyModel::filterAcceptsRow(int sourceRow,
        const QModelIndex &sourceParent) const
{
	int Column = filterKeyColumn();
	QModelIndex Index = sourceModel()->index(sourceRow, filterKeyColumn(), sourceParent);
	QVariant Data = sourceModel()->data(Index);

	if(DateColumns.contains(Column))
	{
		return dateInRange(Data.toDate());
	}
	else
	{
		return QSortFilterProxyModel::filterAcceptsRow(sourceRow, sourceParent);
	}
}

bool MySortFilterProxyModel::lessThan(const QModelIndex &left,
                                      const QModelIndex &right) const
{
	QVariant leftData = sourceModel()->data(left);
    QVariant rightData = sourceModel()->data(right);

	if(SortNumberColumns.contains(left.column()))
	{
		return leftData.toDouble() < rightData.toDouble();
	}

    switch (leftData.type()) {
        case QVariant::Int:
            return leftData.toInt() < rightData.toInt();
        case QVariant::String:
            return leftData.toString() < rightData.toString();
        case QVariant::DateTime:
            return leftData.toDateTime() < rightData.toDateTime();
		case QVariant::Date:
            return leftData.toDate() < rightData.toDate();
        default:
            return false;
    }
}
 
bool MySortFilterProxyModel::dateInRange(const QDate &date) const
{
    return (!minDate.isValid() || date > minDate)
           && (!maxDate.isValid() || date < maxDate);
}


void MySortFilterProxyModel::clearFilter()
{
	setFilterRegExp(QRegExp("", Qt::CaseInsensitive, QRegExp::FixedString));
	minDate = QDate();
	maxDate = QDate();
	invalidateFilter();
}

QList<int> MySortFilterProxyModel::getDateColumns()
{
	return DateColumns;
}