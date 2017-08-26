#ifndef BIBLECHAPTER_H
#define BIBLECHAPTER_H

#include <QObject>

#include "QQmlVarPropertyHelpers.h"
#include "QQmlConstRefPropertyHelpers.h"

class BibleChapter : public QObject {
    Q_OBJECT
    QML_CONSTANT_CSTREF_PROPERTY (QString, chapterId)

public:
    explicit BibleChapter (const QString & chapterId = "",
                           QObject       * parent    = Q_NULLPTR);
};

#endif // BIBLECHAPTER_H
