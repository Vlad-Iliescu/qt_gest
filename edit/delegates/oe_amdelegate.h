#ifndef OE_AMDELEGATE_H
#define OE_AMDELEGATE_H

#include <QItemDelegate>
#include <QComboBox>

class Oe_amDelegate : public QItemDelegate
{
    Q_OBJECT
public:
    explicit Oe_amDelegate(QObject *parent = 0);
    QWidget *createEditor(QWidget *parent,
                    const QStyleOptionViewItem & option ,
                    const QModelIndex & index ) const;
    void setEditorData(QWidget *editor, const QModelIndex &index) const;
    void setModelData(QWidget *editor, QAbstractItemModel *model,
                    const QModelIndex &index) const;
    void updateEditorGeometry(QWidget *editor,
                    const QStyleOptionViewItem &option, const QModelIndex &index ) const;

signals:

public slots:

};

#endif // OE_AMDELEGATE_H
