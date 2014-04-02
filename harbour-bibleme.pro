
################## CONFIG ########################

TARGET       = harbour-bibleme
TEMPLATE     = app
QT          += core network sql gui qml quick
INCLUDEPATH += \
    src/libQtQmlTricks \
    /usr/include/sailfishapp

############### SOURCES & CONTENT #####################

SOURCES     += \
    src/harbour-bibleme.cpp \
    src/libQtQmlTricks/qqmlvariantlistmodel.cpp \
    src/libQtQmlTricks/qqmlobjectlistmodel.cpp \
    src/libQtQmlTricks/qqmlhelpers.cpp \
    src/BibleEngine.cpp

HEADERS     += \
    src/libQtQmlTricks/qqmlvariantlistmodel_p.h \
    src/libQtQmlTricks/qqmlvariantlistmodel.h \
    src/libQtQmlTricks/qqmlobjectlistmodel_p.h \
    src/libQtQmlTricks/qqmlobjectlistmodel.h \
    src/libQtQmlTricks/qqmlmodels.h \
    src/libQtQmlTricks/qqmlhelpers.h \
    src/BibleEngine.h

OTHER_FILES += \
    harbour-bibleme.desktop \
    harbour-bibleme.png \
    harbour-bibleme.svg \
    rpm/harbour-bibleme.yaml \
    qml/harbour-bibleme.qml \
    qml/Ajax.js \
    qml/cover/* \
    qml/components/* \
    qml/img/* \
    qml/pages/BookmarksPage.qml \
    qml/pages/SearchPage.qml \
    qml/pages/SelectPage.qml \
    qml/pages/FilesPage.qml \
    qml/pages/ViewPage.qml \
    qml/pages/SettingsPage.qml

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
