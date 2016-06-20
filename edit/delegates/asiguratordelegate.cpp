#include "asiguratordelegate.h"
#include <QDebug>

AsiguratorDelegate::AsiguratorDelegate(const QMap<QString, int> &asiguratori, QObject *parent) : QItemDelegate(parent) {
    this->asiguratori = asiguratori;
}

void AsiguratorDelegate::setAsiguratori(QMap<QString, int> asiguratori) {
    this->asiguratori = asiguratori;
}

QWidget *AsiguratorDelegate::createEditor(QWidget *parent,
        const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    Q_UNUSED(option);
    Q_UNUSED(index);


    QComboBox *editor = new QComboBox(parent);
//    editor->setSizeAdjustPolicy(QComboBox::AdjustToContents);

    editor->addItem("--Fara--",0);

    QMapIterator<QString, int> i(this->asiguratori);
    while (i.hasNext()) {
        i.next();
        editor->addItem(i.key(),i.value());
    }

    return editor;
}

void AsiguratorDelegate::setEditorData(QWidget *editor,
        const QModelIndex &index) const
{
    QString asig = index.data(Qt::DisplayRole).toString();

    QComboBox *myEditor = static_cast<QComboBox*>(editor);

    int idx = myEditor->findText(asig,Qt::MatchFixedString);
    idx = (idx == -1)?0:idx;

    myEditor->setCurrentIndex(idx);
}

void AsiguratorDelegate::setModelData(QWidget *editor,
        QAbstractItemModel *model, const QModelIndex &index) const
{
    QComboBox *myEditor = static_cast<QComboBox*>(editor);
    QString asig = myEditor->currentText();
    int tip = myEditor->itemData(myEditor->currentIndex()).toInt();


    QMap<int,QVariant> data;
    data.insert(Qt::DisplayRole,asig);
    data.insert(Qt::UserRole,tip);
    model->setItemData(index,data);

    emit asiguratorChanged(tip,index.row());
}

void AsiguratorDelegate::updateEditorGeometry(QWidget *editor,
        const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    Q_UNUSED(index);
    editor->setGeometry(option.rect);
}
