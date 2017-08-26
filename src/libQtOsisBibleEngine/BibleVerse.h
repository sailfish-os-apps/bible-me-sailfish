#ifndef BIBLEVERSE_H
#define BIBLEVERSE_H

#include <QObject>

#include "QQmlVarPropertyHelpers.h"
#include "QQmlConstRefPropertyHelpers.h"

class BibleVerse : public QObject {
    Q_OBJECT
    QML_CONSTANT_CSTREF_PROPERTY (QString, verseId)
    QML_CONSTANT_CSTREF_PROPERTY (QString, textContent)
    QML_READONLY_VAR_PROPERTY    (bool,    marked)

public:
    explicit BibleVerse (const QString & verseId     = "",
                         const QString & textContent = "",
                         const bool      marked      = false,
                         QObject       * parent      = Q_NULLPTR);
};

#endif // BIBLEVERSE_H
