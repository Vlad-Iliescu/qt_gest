#include "cantitatedelegate.h"

CantitateDelegate::CantitateDelegate(QObject *parent) : QItemDelegate(parent) {}

QWidget *CantitateDelegate::createEditor(QWidget *parent,
        const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    Q_UNUSED(option);
    Q_UNUSED(index);


    QSpinBox *editor = new QSpinBox(parent);
    editor->setMinimum(1);
    editor->setMaximum(999);
    editor->setAlignment(Qt::AlignRight);
    return editor;
}

void CantitateDelegate::setEditorData(QWidget *editor,
        const QModelIndex &index) const
{
    QString cant = index.data(Qt::DisplayRole).toString();

    QSpinBox *myEditor = static_cast<QSpinBox*>(editor);

    bool ok;
    int cantitate = cant.toInt(&ok);
    cantitate =(ok)?cantitate:1;

    myEditor->setValue(cantitate);
}

void CantitateDelegate::setModelData(QWidget *editor,
        QAbstractItemModel *model, const QModelIndex &index) const
{
    QSpinBox *myEditor = static_cast<QSpinBox*>(editor);
    int cant = myEditor->value();
    int row = index.row();


    QMap<int,QVariant> data;
    data.insert(Qt::DisplayRole,cant);
    data.insert(Qt::TextAlignmentRole,Qt::AlignHCenter);
    model->setItemData(index,data);

    emit cantitateChanged(cant, row);
}

void CantitateDelegate::updateEditorGeometry(QWidget *editor,
        const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    Q_UNUSED(index);
    editor->setGeometry(option.rect);
}
