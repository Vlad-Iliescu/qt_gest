#include "furnizordelegate.h"

FurnizorDelegate::FurnizorDelegate(const QStringList &furnizori, QObject *parent) : QItemDelegate(parent) {
    this->furnizori = furnizori;
}

void FurnizorDelegate::setFurnizori(QStringList furnizori) {
    this->furnizori = furnizori;
}

QWidget *FurnizorDelegate::createEditor(QWidget *parent,
        const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    Q_UNUSED(option);
    Q_UNUSED(index);


    QComboBox *editor = new QComboBox(parent);
    editor->addItems(this->furnizori);
    return editor;
}

void FurnizorDelegate::setEditorData(QWidget *editor,
        const QModelIndex &index) const
{
    QString furn = index.data(Qt::DisplayRole).toString();

    QComboBox *myEditor = static_cast<QComboBox*>(editor);

    int idx = myEditor->findText(furn,Qt::MatchFixedString);
    idx = (idx == -1)?0:idx;

    myEditor->setCurrentIndex(idx);
}

void FurnizorDelegate::setModelData(QWidget *editor,
        QAbstractItemModel *model, const QModelIndex &index) const
{
    QComboBox *myEditor = static_cast<QComboBox*>(editor);
    QString furn = myEditor->currentText();

    QMap<int,QVariant> data;
    data.insert(Qt::DisplayRole,furn);
    model->setItemData(index,data);
}

void FurnizorDelegate::updateEditorGeometry(QWidget *editor,
        const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    Q_UNUSED(index);
    editor->setGeometry(option.rect);
}
