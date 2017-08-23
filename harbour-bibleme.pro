
################## CONFIG ########################

TARGET       = harbour-bibleme
TEMPLATE     = app

QT          += core network gui qml quick

MOC_DIR      = _moc
OBJECTS_DIR  = _obj
RCC_DIR      = _rcc

INCLUDEPATH += /usr/include/sailfishapp

############### SOURCES & CONTENT #####################

include ($$PWD/src/libQtOsisBibleEngine/QtOsisBibleEngine.pri)

SOURCES     += src/harbour-bibleme.cpp
HEADERS     +=

OTHER_FILES += \
    $$PWD/harbour-bibleme.desktop \
    $$PWD/harbour-bibleme.png \
    $$PWD/harbour-bibleme.svg \
    $$PWD/rpm/harbour-bibleme.yaml \
    $$PWD/qml/harbour-bibleme.qml \
    $$PWD/qml/pages/BookmarksPage.qml \
    $$PWD/qml/pages/SearchPage.qml \
    $$PWD/qml/pages/SelectPage.qml \
    $$PWD/qml/pages/FilesPage.qml \
    $$PWD/qml/pages/ViewPage.qml \
    $$PWD/qml/pages/SettingsPage.qml \
    $$PWD/qml/cover/CoverPage.qml \
    $$PWD/lang/*.ts \
    $$PWD/lang/de.ts

RESOURCES   += \
    $$PWD/data.qrc

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

