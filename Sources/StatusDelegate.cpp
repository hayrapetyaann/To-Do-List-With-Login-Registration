#include "StatusDelegate.h"
#include <QComboBox>

StatusDelegate::StatusDelegate(QObject *parent)
    : QStyledItemDelegate(parent) {}

QWidget *StatusDelegate::createEditor(QWidget *parent, const QStyleOptionViewItem &,
                                      const QModelIndex &) const {
    QComboBox *combo = new QComboBox(parent);
    combo->addItems({"To Do", "In Progress", "Ready"});
    return combo;
}

void StatusDelegate::setEditorData(QWidget *editor, const QModelIndex &index) const {
    QString currentText = index.model()->data(index, Qt::EditRole).toString();
    QComboBox *combo = qobject_cast<QComboBox *>(editor);
    int idx = combo->findText(currentText);
    if (idx >= 0) {
        combo->setCurrentIndex(idx);
    }
}

void StatusDelegate::setModelData(QWidget *editor, QAbstractItemModel *model,
                                   const QModelIndex &index) const {
    QComboBox *combo = qobject_cast<QComboBox *>(editor);
    QString newValue = combo->currentText();
    model->setData(index, newValue);
}

void StatusDelegate::updateEditorGeometry(QWidget *editor,
                                          const QStyleOptionViewItem &option,
                                          const QModelIndex &) const {
    editor->setGeometry(option.rect);
}
