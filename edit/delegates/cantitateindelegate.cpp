#include "cantitateindelegate.h"

CantitateInDelegate::CantitateInDelegate(QObject *parent) : QItemDelegate(parent) {}

QWidget *CantitateInDelegate::createEditor(QWidget *parent,
        const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    Q_UNUSED(option);
    Q_UNUSED(index);


    QSpinBox *editor = new QSpinBox(parent);
    editor->setMinimum(0);
    editor->setMaximum(999);
    editor->setAlignment(Qt::AlignRight);
    return editor;
}

void CantitateInDelegate::setEditorData(QWidget *editor,
        const QModelIndex &index) const
{
    QString cant = index.data(Qt::DisplayRole).toString();

    QSpinBox *myEditor = static_cast<QSpinBox*>(editor);

    bool ok;
    int cantitate = cant.toInt(&ok);
    cantitate =(ok)?cantitate:1;

    myEditor->setValue(cantitate);
}

void CantitateInDelegate::setModelData(QWidget *editor,
        QAbstractItemModel *model, const QModelIndex &index) const
{
    QSpinBox *myEditor = static_cast<QSpinBox*>(editor);
    int cant = myEditor->value();

    QMap<int,QVariant> data;
    data.insert(Qt::DisplayRole,cant);
    data.insert(Qt::TextAlignmentRole,Qt::AlignHCenter);
    model->setItemData(index,data);
}

void CantitateInDelegate::updateEditorGeometry(QWidget *editor,
        const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    Q_UNUSED(index);
    editor->setGeometry(option.rect);
}
