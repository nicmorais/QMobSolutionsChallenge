#ifndef NOTEDITABLEDELEGATE_H
#define NOTEDITABLEDELEGATE_H

#include <QItemDelegate>

class NotEditableDelegate : public QItemDelegate
{
        Q_OBJECT
    public:
        explicit NotEditableDelegate(QObject* parent = 0): QItemDelegate(parent) {};

    protected:
        bool editorEvent(QEvent* event, QAbstractItemModel* model, const QStyleOptionViewItem& option, const QModelIndex& index);
        QWidget* createEditor(QWidget*, const QStyleOptionViewItem&, const QModelIndex&) const;

};
#endif // NOTEDITABLEDELEGATE_H
