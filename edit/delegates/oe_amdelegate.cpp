#include "oe_amdelegate.h"

Oe_amDelegate::Oe_amDelegate(QObject *parent) : QItemDelegate(parent) {}

QWidget *Oe_amDelegate::createEditor(QWidget *parent,
        const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    Q_UNUSED(option);
    Q_UNUSED(index);


    QComboBox *editor = new QComboBox(parent);
    editor->insertItem(0,"OE");
    editor->insertItem(1,"AM");
    return editor;
}

void Oe_amDelegate::setEditorData(QWidget *editor,
        const QModelIndex &index) const
{
    QString oe_am = index.data(Qt::DisplayRole).toString();
    int idx = (oe_am == "AM")?1:0;

    QComboBox *myEditor = static_cast<QComboBox*>(editor);
    myEditor->setCurrentIndex(idx);
}

void Oe_amDelegate::setModelData(QWidget *editor,
        QAbstractItemModel *model, const QModelIndex &index) const
{
    QComboBox *myEditor = static_cast<QComboBox*>(editor);
    int idx = myEditor->currentIndex();
    QString oe_am = (idx == 1)?"AM":"OE";

    QMap<int,QVariant> data;
    data.insert(Qt::DisplayRole,oe_am);
    data.insert(Qt::TextAlignmentRole,Qt::AlignHCenter);
    model->setItemData(index,data);
}

void Oe_amDelegate::updateEditorGeometry(QWidget *editor,
        const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    Q_UNUSED(index);
    editor->setGeometry(option.rect);
}


