#include "noteditabledelegate.h"

bool NotEditableDelegate::editorEvent(QEvent* event, QAbstractItemModel* model, const QStyleOptionViewItem& option, const QModelIndex& index)
{
    return false;
}

QWidget* NotEditableDelegate::createEditor(QWidget*, const QStyleOptionViewItem&, const QModelIndex&) const
{
    return Q_NULLPTR;
}
