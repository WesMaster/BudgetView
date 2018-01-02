#pragma once

#include <QStyledItemDelegate>
#include <QPainter>

class ItemDelegate : public QStyledItemDelegate{
public:
                                                ItemDelegate                               (QTreeView *TreeView){};
    //QSize                                       sizeHint                                   (const QStyleOptionViewItem &, const QModelIndex &) const{return QSize(20,20);};
    void                                        paint                                      (QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index ) const
    {
		QColor BarColour = QColor(144, 238, 144);
		if(index.column() == 5)
		{
			if(index.model()->index(index.row(), 14).data().toBool())
			{
				painter->fillRect(option.rect, BarColour);
			}
		}
        
	    QStyledItemDelegate::paint(painter, option, index);
    };
};