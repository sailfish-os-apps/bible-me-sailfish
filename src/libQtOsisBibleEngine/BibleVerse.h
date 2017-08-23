#ifndef BIBLEVERSE_H
#define BIBLEVERSE_H

#include <QObject>
#include <QVariant>
#include <QString>
#include <QByteArray>

#include "QQmlVarPropertyHelpers.h"
#include "QQmlConstRefPropertyHelpers.h"

class BibleVerse : public QObject {
    Q_OBJECT
    QML_READONLY_VAR_PROPERTY (bool,       marked)
    QML_READONLY_CSTREF_PROPERTY (QByteArray, verseId)
    QML_READONLY_CSTREF_PROPERTY (QString,    textContent)

public:
    explicit BibleVerse (QObject * parent = NULL);

    static BibleVerse * fromQtVariant (QVariantMap variant, QObject * parent = NULL);
    QVariantMap toQtVariant () const;
    void updateWithQtVariant(QVariantMap variant);
};

#endif // BIBLEVERSE_H
