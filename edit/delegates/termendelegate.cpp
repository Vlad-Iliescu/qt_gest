#include "termendelegate.h"
#include <QDebug>

TermenDelegate::TermenDelegate(QObject *parent) : QItemDelegate(parent) {}

QWidget *TermenDelegate::createEditor(QWidget *parent,
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

void TermenDelegate::setEditorData(QWidget *editor,
        const QModelIndex &index) const
{
    QString term = index.data(Qt::UserRole).toString();

    QSpinBox *myEditor = static_cast<QSpinBox*>(editor);

    bool ok;
    int termen = term.toInt(&ok);
    termen =(ok)?termen:1;

    myEditor->setValue(termen);
}

void TermenDelegate::setModelData(QWidget *editor,
        QAbstractItemModel *model, const QModelIndex &index) const
{
    QSpinBox *myEditor = static_cast<QSpinBox*>(editor);
    int termen = myEditor->value();

    QMap<int,QVariant> data;
    data.insert(Qt::UserRole,termen);

    QDateTime arrive = (model->data(index,Qt::WhatsThisRole).canConvert(QVariant::DateTime))
                            ?(model->data(index,Qt::WhatsThisRole).toDateTime())
                                :QDateTime::currentDateTime();
//    qDebug() << arrive;
    data.insert(Qt::WhatsThisRole,arrive);

    while (termen) {
        arrive = arrive.addDays(1);
        if ((arrive.date().dayOfWeek() == Qt::Saturday) || (arrive.date().dayOfWeek() == Qt::Sunday)) {
            arrive = arrive.addDays(1);
        } else {
            --termen;
        }
    }

    data.insert(Qt::DisplayRole,arrive.toString("dd-MM-yyyy"));
    data.insert(Qt::TextAlignmentRole,Qt::AlignRight);
    model->setItemData(index,data);
}

void TermenDelegate::updateEditorGeometry(QWidget *editor,
        const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    Q_UNUSED(index);
    editor->setGeometry(option.rect);
}

