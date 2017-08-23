#ifndef BIBLETEXT_H
#define BIBLETEXT_H

#include <QObject>
#include <QVariant>
#include <QString>
#include <QByteArray>

#include "QQmlVarPropertyHelpers.h"
#include "QQmlConstRefPropertyHelpers.h"

class BibleText : public QObject {
    Q_OBJECT
    QML_READONLY_VAR_PROPERTY (bool,       hasLocal)
    QML_READONLY_VAR_PROPERTY (bool,       isLoading)
    QML_READONLY_VAR_PROPERTY (int,        percent)
    QML_READONLY_CSTREF_PROPERTY (QString,    languageTitle)
    QML_READONLY_CSTREF_PROPERTY (QString,    bibleTitle)
    QML_READONLY_CSTREF_PROPERTY (QByteArray, languageID)
    QML_READONLY_CSTREF_PROPERTY (QByteArray, bibleID)
    QML_READONLY_CSTREF_PROPERTY (QByteArray, textKey)

public:
    explicit BibleText (QObject * parent = NULL);

    static BibleText * fromQtVariant (QVariantMap variant, QObject * parent = NULL);
    QVariantMap toQtVariant () const;
    void updateWithQtVariant(QVariantMap variant);
};

#endif // BIBLETEXT_H
