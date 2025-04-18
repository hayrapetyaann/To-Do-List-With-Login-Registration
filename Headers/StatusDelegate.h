#ifndef STATUSDELEGATE_H
#define STATUSDELEGATE_H

#include <QStyledItemDelegate>
#include <QObject>

class StatusDelegate : public QStyledItemDelegate {
    Q_OBJECT
public:
    explicit StatusDelegate(QObject* parent = nullptr);

    QWidget* createEditor(QWidget* parent, const QStyleOptionViewItem& option,
                          const QModelIndex& index) const override;

    void setEditorData(QWidget* editor, const QModelIndex& index) const override;

    void setModelData(QWidget* editor, QAbstractItemModel* model,
                      const QModelIndex& index) const override;

    void updateEditorGeometry(QWidget* editor, const QStyleOptionViewItem& option,
                              const QModelIndex& index) const override;
};

#endif // STATUSDELEGATE_H
