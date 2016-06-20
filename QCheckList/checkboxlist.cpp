#include "checkboxlist.h"

CheckBoxList::CheckBoxList(QWidget *parent) :
    QComboBox(parent), m_DisplayText("")
{
    // set delegate items view
     view()->setItemDelegate(new CheckBoxListDelegate(this));

     // Enable editing on items view
     view()->setEditTriggers(QAbstractItemView::CurrentChanged);

     // set "CheckBoxList::eventFilter" as event filter for items view
     view()->viewport()->installEventFilter(this);

     // it just cool to have it as defualt
     view()->setAlternatingRowColors(true);

     this->setDisplayText("Selectati coloanele");
}

CheckBoxList::~CheckBoxList() {

}

bool CheckBoxList::eventFilter(QObject *object, QEvent *event) {
    // don’t close items view after we release the mouse button
    // by simple eating MouseButtonRelease in viewport of items view
    if(event->type() == QEvent::MouseButtonRelease && object==view()->viewport())
    {
        return true;
    }
    return QComboBox::eventFilter(object,event);
}

void CheckBoxList::paintEvent(QPaintEvent *event) {
    Q_UNUSED(event);

    QStylePainter painter(this);
    painter.setPen(palette().color(QPalette::Text));

    // draw the combobox frame, focusrect and selected etc.
    QStyleOptionComboBox opt;
    initStyleOption(&opt);

    // if no display text been set , use "…" as default
    if(m_DisplayText.isNull())
        opt.currentText = "…";
    else
        opt.currentText = m_DisplayText;
    painter.drawComplexControl(QStyle::CC_ComboBox, opt);

    // draw the icon and text
    painter.drawControl(QStyle::CE_ComboBoxLabel, opt);
}

void CheckBoxList::setDisplayText(QString text) {
    m_DisplayText = text;
}

QString CheckBoxList::getDisplayText() const {
    return m_DisplayText;
}

bool CheckBoxList::itemCheckState(int index) {
    return this->itemData(index).toBool();
}

void CheckBoxList::setItemCheckState(int index, bool state) {
    this->setItemData(index,state);
}

void CheckBoxList::uncheckAll() {
    if (this->model())
    {
        QModelIndex index = this->model()->index(0, this->modelColumn(), this->rootModelIndex());
        QModelIndexList indexes = this->model()->match(index, Qt::UserRole, true, -1, Qt::MatchExactly);
        foreach(const QModelIndex& index, indexes) {
            this->setItemCheckState(index.row(),false);
        }
    }
}

QStringList CheckBoxList::checkedItems() const
{
    QStringList items;
    if (this->model())
    {
        QModelIndex index = this->model()->index(0, this->modelColumn(), this->rootModelIndex());
        QModelIndexList indexes = this->model()->match(index, Qt::UserRole, true, -1, Qt::MatchExactly);
        foreach(const QModelIndex& index, indexes) {
            items += index.data().toString();
        }
    }
    return items;
}

QStringList CheckBoxList::checkedIndexes() const {
    QStringList items;
    if (this->model())
    {
        QModelIndex index = this->model()->index(0, this->modelColumn(), this->rootModelIndex());
        QModelIndexList indexes = this->model()->match(index, Qt::UserRole, true, -1, Qt::MatchExactly);
        foreach(const QModelIndex& index, indexes) {
            items << QString("%1").arg(index.row());
        }
    }
    return items;
}
