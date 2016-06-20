#ifndef ASIGURATORDELEGATE_H
#define ASIGURATORDELEGATE_H

#include <QItemDelegate>
#include <QComboBox>

class AsiguratorDelegate : public QItemDelegate
{
    Q_OBJECT
public:
    explicit AsiguratorDelegate(const QMap<QString, int> &asiguratori, QObject *parent = 0);
    QWidget *createEditor(QWidget *parent,
                    const QStyleOptionViewItem & option ,
                    const QModelIndex & index ) const;
    void setEditorData(QWidget *editor, const QModelIndex &index) const;
    void setModelData(QWidget *editor, QAbstractItemModel *model,
                    const QModelIndex &index) const;
    void updateEditorGeometry(QWidget *editor,
                    const QStyleOptionViewItem &option, const QModelIndex &index ) const;
    void setAsiguratori(QMap<QString, int> asiguratori);

signals:
    void asiguratorChanged(int tip, int row) const;

public slots:

private:
    QMap<QString, int> asiguratori;

};

#endif // ASIGURATORDELEGATE_H
