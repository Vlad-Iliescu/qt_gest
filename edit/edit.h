#ifndef EDIT_H
#define EDIT_H

#include <QDialog>
#include <QTableWidgetItem>
#include <QVariant>

#include "global.h"
#include "log.h"

#include "Delegates/comandadelegate.h"
#include "Delegates/oe_amdelegate.h"
#include "Delegates/asiguratordelegate.h"
#include "Delegates/cantitatedelegate.h"
#include "Delegates/furnizordelegate.h"
#include "Delegates/termendelegate.h"
#include "Delegates/pretdelegate.h"
#include "Delegates/cantitateindelegate.h"

//#include "edit/table_model/vericalheader.h"

#include "global.h"
#include "log.h"
#include "math.h"

namespace Ui {
    class Edit;
}

class Edit : public QDialog
{
    Q_OBJECT

signals:
    void kill();

public:
    explicit Edit(QWidget *parent = 0);
    ~Edit();

public slots:
    void getDetaliiContract(int contract_id);
    void resetPrivileges();
    int getContractId();

private:
    Ui::Edit *ui;
    enum Privilege {
        Mandatory = 1,
        Optional = 2,
        NotAllowed = 3
    };
    enum PrivilegeRoles{
        ItemPrivilege = Qt::UserRole + 1
    };

    QHash<int, Privilege> privileges;

    int contract_id;
    int nr_linii;
    QMovie *movie;

    double TVA;
    QMenu *menu;
    QAction *duplicate;
    QAction *remove;
    QAction *add;
    QAction *copy;
    QAction *total;
    QAction *comanda;
    QVariantList toDelete;

    QMap<QString, int> asiguratori;
    QMap<QString, int> consilieri;
    QStringList furnizori;

    void createActions();
    inline double round(double n, unsigned d = 2) {
          return floor(n * pow(10., d) + .5) / pow(10., d);
    }


private slots:
    void loadingStop();
    void loadingStart();
    void handleNetworkError(QNetworkReply *reply);

    void parseContract(QByteArray data);

    void calcPretVanzareTVA(double pret, int row);
    void calcTotal(int cantitate, int row);
    void calcTotal(double pretTVA, int row);

    void calcTotals();

    void showContextMenu(const QPoint point);

    void changedAsig(int tip, int row);

    void addNewRow();
    void duplicateRow(int row);
    void removeRow(int row);
    void comandaMass();

    QVariant getCellData(int row, int col, Qt::ItemDataRole role, bool *empty);
    void saveContract();
    void parseSaveContract(QByteArray data);

    void colorCells();
    void colorRow(int row);
    void colorCell(int row, int col);
    QPixmap stareIcon(int stare);

    Edit::Privilege getPrivCol(int tip, int col);
    void beforeClose();

    void copySelection();
    void totalSelection();

};

#endif // EDIT_H
