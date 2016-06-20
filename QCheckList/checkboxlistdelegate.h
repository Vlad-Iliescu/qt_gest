#ifndef CHECKBOXLISTDELEGATE_H
#define CHECKBOXLISTDELEGATE_H

#include <QItemDelegate>
#include <QCheckBox>
#include <QApplication>
#include <QDebug>

class CheckBoxListDelegate : public QItemDelegate
{
public:
    CheckBoxListDelegate(QObject *parent);
    void paint(QPainter *painter, const QStyleOptionViewItem &option,
                 const QModelIndex &index) const;
    QWidget *createEditor(QWidget *parent,
                 const QStyleOptionViewItem & option ,
                 const QModelIndex & index ) const;
    void setEditorData(QWidget *editor, const QModelIndex &index) const;
    void setModelData(QWidget *editor, QAbstractItemModel *model,
                 const QModelIndex &index) const;
    void updateEditorGeometry(QWidget *editor,
                 const QStyleOptionViewItem &option, const QModelIndex &index ) const;
};

#endif // CHECKBOXLISTDELEGATE_H
