QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

# The following define makes your compiler emit warnings if you use
# any Qt feature that has been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    bookmarksitemmodel.cpp \
    djvu/djvucontentitemmodel.cpp \
    djvu/djvudocument.cpp \
    djvu/djvupagegraphicsitem.cpp \
    document.cpp \
    main.cpp \
    mainwindow.cpp \
    pdf/pdfcontentitemmodel.cpp \
    pdf/pdfdocument.cpp \
    pdf/pdfpagegraphicsitem.cpp \
    recentfileslist.cpp \
    settings.cpp

HEADERS += \
    contentsitemmodel.h \
    Tree.h \
    bookmark.h \
    bookmarksitemmodel.h \
    djvu/djvucontentitemmodel.h \
    djvu/djvudocument.h \
    djvu/djvupagegraphicsitem.h \
    document.h \
    mainwindow.h \
    pdf/pagegraphicsitem.h \
    pdf/pagegraphicsitem.h \
    pdf/pdfcontentitemmodel.h \
    pdf/pdfdocument.h \
    pdf/pdfpagegraphicsitem.h \
    recentfileslist.h \
    settings.h

FORMS += \
    mainwindow.ui

TRANSLATIONS += \
    RReader_ru_RU.ts

INCLUDEPATH += pdf/ djvu/

LIBS += -lpoppler-qt5 -ldjvulibre

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    resoures.qrc
