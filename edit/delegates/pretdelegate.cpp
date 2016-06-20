#include "pretdelegate.h"

PretDelegate::PretDelegate(QObject *parent) : QItemDelegate(parent) {}

QWidget *PretDelegate::createEditor(QWidget *parent,
        const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    Q_UNUSED(option);
    Q_UNUSED(index);


    QDoubleSpinBox *editor = new QDoubleSpinBox(parent);
    editor->setMinimum(0.00);
    editor->setMaximum(99999.99);
    editor->setAlignment(Qt::AlignRight);
    return editor;
}

void PretDelegate::setEditorData(QWidget *editor,
        const QModelIndex &index) const
{
    QString pret_s = index.data(Qt::DisplayRole).toString();

    QDoubleSpinBox *myEditor = static_cast<QDoubleSpinBox*>(editor);
    myEditor->setLocale(QLocale::C);

    bool ok;
    double pret = pret_s.toDouble(&ok);
    pret = (ok)?pret:0.00;

    myEditor->setValue(pret);
}

void PretDelegate::setModelData(QWidget *editor,
        QAbstractItemModel *model, const QModelIndex &index) const
{
    QDoubleSpinBox *myEditor = static_cast<QDoubleSpinBox*>(editor);
    QString pret = myEditor->text();


    QMap<int,QVariant> data;
    data.insert(Qt::DisplayRole,pret);
    data.insert(Qt::TextAlignmentRole,Qt::AlignRight);
    model->setItemData(index,data);

    emit pretChanged(myEditor->value(), index.row());
}

void PretDelegate::updateEditorGeometry(QWidget *editor,
        const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    Q_UNUSED(index);
    editor->setGeometry(option.rect);
}

