#include "customverticalheader.h"

CustomVerticalHeader::CustomVerticalHeader(QWidget *parent) : QHeaderView(Qt::Vertical,parent) {
    this->setDefaultSectionSize(18);
    this->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Maximum);
}

void CustomVerticalHeader::paintSection(QPainter *painter, const QRect &rect, int logicalIndex) const {
    bool On = this->model()->headerData(logicalIndex,Qt::Vertical,Qt::UserRole).toBool();


    painter->save();
    QHeaderView::paintSection(painter, rect, logicalIndex);
    painter->restore();

    QStyleOptionButton option;
    option.rect = rect;

    if (On) {
        option.state = QStyle::State_On;
    } else {
        option.state = QStyle::State_Off;
    }

    option.state |= QStyle::State_Enabled;

    painter->setBackground(QBrush(Qt::green));
    this->style()->drawControl(QStyle::CE_CheckBox,&option,painter);

    }

void CustomVerticalHeader::mousePressEvent(QMouseEvent *event) {
    int index = this->logicalIndexAt(event->pos());
    bool on = !this->model()->headerData(index,Qt::Vertical,Qt::UserRole).toBool();

    this->model()->setHeaderData(index,Qt::Vertical,on,Qt::UserRole);
    this->updateSection(index);
    QHeaderView::mousePressEvent(event);
}
