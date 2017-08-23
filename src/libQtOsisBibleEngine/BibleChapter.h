#ifndef BIBLECHAPTER_H
#define BIBLECHAPTER_H

#include <QObject>
#include <QVariant>
#include <QString>
#include <QByteArray>

#include "QQmlVarPropertyHelpers.h"
#include "QQmlConstRefPropertyHelpers.h"

class BibleChapter : public QObject {
    Q_OBJECT
    QML_READONLY_CSTREF_PROPERTY (QByteArray, chapterId)
    QML_READONLY_VAR_PROPERTY (int,        firstVerseIdx)
    QML_READONLY_VAR_PROPERTY (int,        lastVerseIdx)

public:
    explicit BibleChapter (QObject * parent = NULL);

    static BibleChapter * fromQtVariant (QVariantMap variant, QObject * parent = NULL);
    QVariantMap toQtVariant () const;
    void updateWithQtVariant(QVariantMap variant);
};

#endif // BIBLECHAPTER_H
