#ifndef CANTITATEINDELEGATE_H
#define CANTITATEINDELEGATE_H

#include <QItemDelegate>
#include <QSpinBox>

class CantitateInDelegate : public QItemDelegate
{
    Q_OBJECT
public:
    explicit CantitateInDelegate(QObject *parent = 0);
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

#endif // CANTITATEINDELEGATE_H
