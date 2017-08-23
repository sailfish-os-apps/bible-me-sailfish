
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

SOURCES += $$PWD/src/harbour-bibleme.cpp
HEADERS +=

OTHER_FILES += \
    $$PWD/rpm/harbour-bibleme.yaml \
    $$PWD/harbour-bibleme.desktop \
    $$PWD/icons/harbour-bibleme.svg \
    $$PWD/icons/86x86/harbour-bibleme.png \
    $$PWD/icons/108x108/harbour-bibleme.png \
    $$PWD/icons/128x128/harbour-bibleme.png \
    $$PWD/icons/256x256/harbour-bibleme.png

RESOURCES += $$PWD/data.qrc

################ LOCALIZATION ########################

TRANSLATIONS += \
    lang/en.ts \
    lang/fr.ts \
    lang/fi.ts \
    lang/de.ts

#lupdate_only { ### uncomment this block before running lupdate on project !
#    SOURCES += qml/*.qml qml/pages/*.qml qml/cover/*.qml
#}

################## PACKAGING ########################

CONFIG       += link_pkgconfig
PKGCONFIG    += sailfishapp

target.files  = $${TARGET}
target.path   = /usr/bin
desktop.files = $$PWD/$${TARGET}.desktop
desktop.path  = /usr/share/applications
icon86.files  = $$PWD/icons/86x86/$${TARGET}.png
icon86.path   = /usr/share/icons/hicolor/86x86/apps
icon108.files = $$PWD/icons/108x108/$${TARGET}.png
icon108.path  = /usr/share/icons/hicolor/108x108/apps
icon128.files = $$PWD/icons/128x128/$${TARGET}.png
icon128.path  = /usr/share/icons/hicolor/128x128/apps
icon256.files = $$PWD/icons/256x256/$${TARGET}.png
icon256.path  = /usr/share/icons/hicolor/256x256/apps
INSTALLS     += target desktop icon86 icon108 icon128 icon256
