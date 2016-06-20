#ifndef CHECKBOXLIST_H
#define CHECKBOXLIST_H

#include <QComboBox>
#include <QStylePainter>
#include <QAbstractItemView>

#include "checkboxlistdelegate.h"

class CheckBoxList : public QComboBox
{
    Q_OBJECT

signals:
    void checkedItemsChanged(QStringList items);

public:
    CheckBoxList(QWidget *parent = 0);
    virtual ~CheckBoxList();

    bool eventFilter(QObject *object, QEvent *event);
    virtual void paintEvent(QPaintEvent *event);

    void setDisplayText(QString text);
    QString getDisplayText() const;

    bool itemCheckState(int index);
    void setItemCheckState(int index, bool state);
    void uncheckAll();

    QStringList checkedItems() const;
    QStringList checkedIndexes() const;

private:
    QString m_DisplayText;

};

#endif // CHECKBOXLIST_H
