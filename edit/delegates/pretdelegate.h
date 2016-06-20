#ifndef PRETDELEGATE_H
#define PRETDELEGATE_H

#include <QItemDelegate>
#include <QDoubleSpinBox>

class PretDelegate : public QItemDelegate
{
    Q_OBJECT
public:
    explicit PretDelegate(QObject *parent = 0);
    QWidget *createEditor(QWidget *parent,
                    const QStyleOptionViewItem & option ,
                    const QModelIndex & index ) const;
    void setEditorData(QWidget *editor, const QModelIndex &index) const;
    void setModelData(QWidget *editor, QAbstractItemModel *model,
                    const QModelIndex &index) const;
    void updateEditorGeometry(QWidget *editor,
                    const QStyleOptionViewItem &option, const QModelIndex &index ) const;

signals:
    void pretChanged(double pret, int row) const;

public slots:

};

#endif // PRETDELEGATE_H
