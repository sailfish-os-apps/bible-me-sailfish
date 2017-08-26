
INCLUDEPATH = $$PWD

include ($$PWD/../libQtQmlTricks/SuperMacros/QtSuperMacros.pri)
include ($$PWD/../libQtQmlTricks/SmartDataModels/QtQmlModels.pri)

HEADERS += \
    $$PWD/BibleBook.h \
    $$PWD/BibleChapter.h \
    $$PWD/BibleText.h \
    $$PWD/BibleVerse.h \
    $$PWD/BibleEngine.h \
    $$PWD/BibleWorker.h \
    $$PWD/BibleLanguage.h

SOURCES += \
    $$PWD/BibleBook.cpp \
    $$PWD/BibleChapter.cpp \
    $$PWD/BibleText.cpp \
    $$PWD/BibleVerse.cpp \
    $$PWD/BibleEngine.cpp \
    $$PWD/BibleWorker.cpp \
    $$PWD/BibleLanguage.cpp
