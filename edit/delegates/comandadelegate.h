#ifndef COMANDADELEGATE_H
#define COMANDADELEGATE_H

#include <QItemDelegate>
#include <QComboBox>

class ComandaDelegate : public QItemDelegate
{
    Q_OBJECT
public:
    explicit ComandaDelegate(QObject *parent = 0);
    QWidget *createEditor(QWidget *parent,
                    const QStyleOptionViewItem & option ,
                    const QModelIndex & index ) const;
    void setEditorData(QWidget *editor, const QModelIndex &index) const;
    void setModelData(QWidget *editor, QAbstractItemModel *model,
                    const QModelIndex &index) const;
    void updateEditorGeometry(QWidget *editor,
                    const QStyleOptionViewItem &option, const QModelIndex &index ) const;

signals:
    void comandaChange() const;

public slots:

};

#endif // COMANDADELEGATE_H
