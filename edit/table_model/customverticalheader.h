#ifndef CUSTOMVERTICALHEADER_H
#define CUSTOMVERTICALHEADER_H

#include <QHeaderView>
#include <QPainter>
#include <QMouseEvent>
#include <QDebug>


class CustomVerticalHeader : public QHeaderView
{
    Q_OBJECT
public:
    explicit CustomVerticalHeader(QWidget *parent = 0);

private:
    void paintSection(QPainter *painter, const QRect &rect, int logicalIndex) const;
    void mousePressEvent(QMouseEvent *event);
};

#endif // CUSTOMVERTICALHEADER
