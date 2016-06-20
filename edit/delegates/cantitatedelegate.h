#ifndef CANTITATEDELEGATE_H
#define CANTITATEDELEGATE_H

#include <QItemDelegate>
#include <QSpinBox>

class CantitateDelegate : public QItemDelegate
{
    Q_OBJECT
public:
    explicit CantitateDelegate(QObject *parent = 0);
    QWidget *createEditor(QWidget *parent,
                    const QStyleOptionViewItem & option ,
                    const QModelIndex & index ) const;
    void setEditorData(QWidget *editor, const QModelIndex &index) const;
    void setModelData(QWidget *editor, QAbstractItemModel *model,
                    const QModelIndex &index) const;
    void updateEditorGeometry(QWidget *editor,
                    const QStyleOptionViewItem &option, const QModelIndex &index ) const;


signals:
    void cantitateChanged(int cantitate, int row) const;

public slots:

};

#endif // CANTITATEDELEGATE_H
