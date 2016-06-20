#ifndef RAPORT_H
#define RAPORT_H

#include <QDialog>
#include <QTableWidgetItem>
#include <QVariant>
#include <QDebug>
#include <QClipboard>
#include <QTableWidgetSelectionRange>

namespace Ui {
    class Raport;
}

class Raport : public QDialog
{
    Q_OBJECT

public:
    explicit Raport(QWidget *parent = 0);
    ~Raport();
    void populate(QVariantMap data);

private:
    Ui::Raport *ui;

private slots:
    void copySelection();
};

#endif // RAPORT_H
