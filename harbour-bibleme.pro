
################## CONFIG ########################

TARGET       = harbour-bibleme
TEMPLATE     = app

QT          += core network gui qml quick

MOC_DIR      = _moc
OBJECTS_DIR  = _obj
RCC_DIR      = _rcc

INCLUDEPATH += \
    $$PWD/src/libQtOsisBibleEngine \
    $$PWD/src/libQtQmlTricks \
    /usr/include/sailfishapp

############### SOURCES & CONTENT #####################

SOURCES     += \
    src/harbour-bibleme.cpp \
    src/libQtQmlTricks/qqmlvariantlistmodel.cpp \
    src/libQtQmlTricks/qqmlobjectlistmodel.cpp \
    src/libQtQmlTricks/qqmlhelpers.cpp \
    src/libQtOsisBibleEngine/BibleEngine.cpp \
    src/libQtOsisBibleEngine/BibleText.cpp \
    src/libQtOsisBibleEngine/BibleBook.cpp \
    src/libQtOsisBibleEngine/BibleChapter.cpp \
    src/libQtOsisBibleEngine/BibleVerse.cpp \
    src/libQtOsisBibleEngine/BibleWorker.cpp

HEADERS     += \
    src/libQtQmlTricks/qqmlvariantlistmodel_p.h \
    src/libQtQmlTricks/qqmlvariantlistmodel.h \
    src/libQtQmlTricks/qqmlobjectlistmodel_p.h \
    src/libQtQmlTricks/qqmlobjectlistmodel.h \
    src/libQtQmlTricks/qqmlmodels.h \
    src/libQtQmlTricks/qqmlhelpers.h \
    src/libQtOsisBibleEngine/BibleEngine.h \
    src/libQtOsisBibleEngine/BibleText.h \
    src/libQtOsisBibleEngine/BibleBook.h \
    src/libQtOsisBibleEngine/BibleChapter.h \
    src/libQtOsisBibleEngine/BibleVerse.h \
    src/libQtOsisBibleEngine/BibleWorker.h

OTHER_FILES += \
    harbour-bibleme.desktop \
    harbour-bibleme.png \
    harbour-bibleme.svg \
    rpm/harbour-bibleme.yaml \
    qml/harbour-bibleme.qml \
    qml/pages/BookmarksPage.qml \
    qml/pages/SearchPage.qml \
    qml/pages/SelectPage.qml \
    qml/pages/FilesPage.qml \
    qml/pages/ViewPage.qml \
    qml/pages/SettingsPage.qml \
    qml/cover/CoverPage.qml \
    lang/*.ts \
    lang/de.ts

RESOURCES   += \
    data.qrc

################## PACKAGING ########################

CONFIG       += link_pkgconfig
PKGCONFIG    += sailfishapp

target.files  = $${TARGET}
target.path   = /usr/bin
desktop.files = $${TARGET}.desktop
desktop.path  = /usr/share/applications
icon.files    = $${TARGET}.png
icon.path     = /usr/share/icons/hicolor/86x86/apps
INSTALLS     += target desktop icon

################ LOCALIZATION ########################

TRANSLATIONS += \
    lang/en.ts \
    lang/fr.ts \
    lang/fi.ts \
    lang/de.ts

qt_linguist_only {
SOURCES += \
    qml/harbour-bibleme.qml \
    qml/pages/BookmarksPage.qml \
    qml/pages/SearchPage.qml \
    qml/pages/SelectPage.qml \
    qml/pages/FilesPage.qml \
    qml/pages/ViewPage.qml \
    qml/pages/SettingsPage.qml \
    qml/cover/CoverPage.qml
}

