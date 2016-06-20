#include "comandadelegate.h"

ComandaDelegate::ComandaDelegate(QObject *parent) : QItemDelegate(parent) {
}

QWidget *ComandaDelegate::createEditor(QWidget *parent,
        const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    Q_UNUSED(option);
    Q_UNUSED(index);


    QComboBox *editor = new QComboBox(parent);
    editor->insertItem(0,"Oferta");
    editor->insertItem(1,"Comanda");
    return editor;
}

void ComandaDelegate::setEditorData(QWidget *editor,
        const QModelIndex &index) const
{
    QString comanda = index.data(Qt::DisplayRole).toString();
    int comanda_idx = (comanda == "Comanda")?1:0;

    QComboBox *myEditor = static_cast<QComboBox*>(editor);
    myEditor->setCurrentIndex(comanda_idx);
}

void ComandaDelegate::setModelData(QWidget *editor,
        QAbstractItemModel *model, const QModelIndex &index) const
{
    QComboBox *myEditor = static_cast<QComboBox*>(editor);
    int idx = myEditor->currentIndex();
    QString comanda = (idx == 1)?"Comanda":"Oferta";
    bool com = (comanda == "Comanda")?true:false;


    QMap<int,QVariant> data;
    data.insert(Qt::DisplayRole,comanda);
    data.insert(Qt::UserRole,com);
    model->setItemData(index,data);

    emit comandaChange();
}

void ComandaDelegate::updateEditorGeometry(QWidget *editor,
        const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    Q_UNUSED(index);
    editor->setGeometry(option.rect);
}

