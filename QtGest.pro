#-------------------------------------------------
#
# Project created by QtCreator 2011-08-02T13:00:16
#
#-------------------------------------------------

QT       += core gui network widgets

TARGET = QtGest
TEMPLATE = app


SOURCES += main.cpp\
        qtgest.cpp \
    error_logger/logger.cpp \
    network/networkaccess.cpp \
    login/login.cpp \
    log.cpp \
    global.cpp \
    add/addcontract.cpp \
    mdis/mainmdi.cpp \
    mdis/raport.cpp \
    edit/edit.cpp \
    edit/table_model/contractedelegate.cpp \
    edit/table_model/customverticalheader.cpp \
    edit/delegates/comandadelegate.cpp \
    edit/delegates/oe_amdelegate.cpp \
    edit/delegates/asiguratordelegate.cpp \
    edit/delegates/cantitatedelegate.cpp \
    edit/delegates/furnizordelegate.cpp \
    edit/delegates/termendelegate.cpp \
    edit/delegates/pretdelegate.cpp \
    edit/delegates/cantitateindelegate.cpp \
    todo/todo.cpp \
    furnizor/furnizori.cpp \
    furnizor/addeditfrunizor.cpp \
    user/users.cpp \
    user/edituser.cpp \
    user/adduser.cpp \
    rapoarte/rapoarte.cpp \
    rapoarte/addraport.cpp \
    QCheckList/checkboxlistdelegate.cpp \
    QCheckList/checkboxlist.cpp \
    accelerate/kuhn_munkres.cpp \
    accelerate/alt_key.cpp \
    find/search.cpp \
    mdis/finalizate.cpp \
    asigurator/asiguratori.cpp \
    asigurator/addeditasigurator.cpp \
    parser/parser.cpp \
    serializer/serializer.cpp

HEADERS  += qtgest.h \
    global.h \
    error_logger/logger.h \
    log.h \
    network/networkaccess.h \
    login/login.h \
    add/addcontract.h \
    mdis/mainmdi.h \
    mdis/raport.h \
    edit/edit.h \
    edit/table_model/contractedelegate.h \
    edit/table_model/customverticalheader.h \
    edit/delegates/comandadelegate.h \
    edit/delegates/oe_amdelegate.h \
    edit/delegates/asiguratordelegate.h \
    edit/delegates/cantitatedelegate.h \
    edit/delegates/furnizordelegate.h \
    edit/delegates/termendelegate.h \
    edit/delegates/pretdelegate.h \
    edit/delegates/cantitateindelegate.h \
    todo/todo.h \
    furnizor/furnizori.h \
    furnizor/addeditfrunizor.h \
    user/users.h \
    user/edituser.h \
    user/adduser.h \
    rapoarte/rapoarte.h \
    rapoarte/addraport.h \
    QCheckList/checkboxlistdelegate.h \
    QCheckList/checkboxlist.h \
    accelerate/kuhn_munkres.hpp \
    accelerate/alt_key.hpp \
    find/search.h \
    mdis/finalizate.h \
    asigurator/asiguratori.h \
    asigurator/addeditasigurator.h \
    parser/parser.h \
    serializer/serializer.h

FORMS    += qtgest.ui \
    login/login.ui \
    add/addcontract.ui \
    mdis/mainmdi.ui \
    edit/edit.ui \
    todo/todo.ui \
    furnizor/furnizori.ui \
    furnizor/addeditfrunizor.ui \
    user/users.ui \
    user/edituser.ui \
    user/adduser.ui \
    mdis/raport.ui \
    rapoarte/rapoarte.ui \
    rapoarte/addraport.ui \
    find/search.ui \
    mdis/finalizate.ui \
    asigurator/asiguratori.ui \
    asigurator/addeditasigurator.ui

OTHER_FILES = resource.rc

DESTDIR = debug/target/

OBJECTS_DIR = debug/obj/

MOC_DIR = debug/moc/

RCC_DIR = debug/rcc/

RESOURCES += \
    resources.qrc

RC_FILE = resource.rc

LIBS += -lqjson0

#INCLUDEPATH += network/src

#QT += widgets

DISTFILES += \
    application.ini \
    README


















