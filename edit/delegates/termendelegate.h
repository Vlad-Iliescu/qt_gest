#ifndef TERMENDELEGATE_H
#define TERMENDELEGATE_H

#include <QItemDelegate>
#include <QDate>
#include <QSpinBox>

class TermenDelegate : public QItemDelegate
{
    Q_OBJECT
public:
    explicit TermenDelegate(QObject *parent = 0);
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

private:

};

#endif // TERMENDELEGATE_H
