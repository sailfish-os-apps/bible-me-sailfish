#ifndef BIBLETEXT_H
#define BIBLETEXT_H

#include <QObject>

#include "QQmlVarPropertyHelpers.h"
#include "QQmlConstRefPropertyHelpers.h"

class BibleText : public QObject {
    Q_OBJECT
    QML_CONSTANT_CSTREF_PROPERTY (QString, textKey)
    QML_CONSTANT_CSTREF_PROPERTY (QString, bibleId)
    QML_CONSTANT_CSTREF_PROPERTY (QString, bibleTitle)
    QML_READONLY_VAR_PROPERTY    (bool,    hasLocal)
    QML_READONLY_VAR_PROPERTY    (bool,    isLoading)
    QML_READONLY_VAR_PROPERTY    (int,     percent)

public:
    explicit BibleText (const QString & textKey    = "",
                        const QString & bibleId    = "",
                        const QString & bibleTitle = "",
                        const bool      hasLocal   = false,
                        QObject       * parent     = Q_NULLPTR);
};

#endif // BIBLETEXT_H
