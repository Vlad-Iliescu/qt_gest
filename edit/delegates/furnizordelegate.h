#ifndef FURNIZORDELEGATE_H
#define FURNIZORDELEGATE_H

#include <QItemDelegate>
#include <QComboBox>

class FurnizorDelegate : public QItemDelegate
{
    Q_OBJECT
public:
    explicit FurnizorDelegate(const QStringList &furnizori, QObject *parent = 0);
    QWidget *createEditor(QWidget *parent,
                    const QStyleOptionViewItem & option ,
                    const QModelIndex & index ) const;
    void setEditorData(QWidget *editor, const QModelIndex &index) const;
    void setModelData(QWidget *editor, QAbstractItemModel *model,
                    const QModelIndex &index) const;
    void updateEditorGeometry(QWidget *editor,
                    const QStyleOptionViewItem &option, const QModelIndex &index ) const;

    void setFurnizori(QStringList furnizori);
signals:

public slots:

private:
    QStringList furnizori;

};

#endif // FURNIZORDELEGATE_H
