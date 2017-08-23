
INCLUDEPATH = $$PWD

include ($$PWD/../libQtQmlTricks/SuperMacros/QtSuperMacros.pri)
include ($$PWD/../libQtQmlTricks/SmartDataModels/QtQmlModels.pri)

HEADERS += \
    $$PWD/BibleBook.h \
    $$PWD/BibleChapter.h \
    $$PWD/BibleEngine.h \
    $$PWD/BibleText.h \
    $$PWD/BibleVerse.h \
    $$PWD/BibleWorker.h

SOURCES += \
    $$PWD/BibleBook.cpp \
    $$PWD/BibleChapter.cpp \
    $$PWD/BibleEngine.cpp \
    $$PWD/BibleText.cpp \
    $$PWD/BibleVerse.cpp \
    $$PWD/BibleWorker.cpp
