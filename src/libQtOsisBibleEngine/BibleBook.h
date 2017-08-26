#ifndef BIBLEBOOK_H
#define BIBLEBOOK_H

#include <QObject>
#include <QByteArray>

#include "QQmlVarPropertyHelpers.h"
#include "QQmlConstRefPropertyHelpers.h"

class BibleBook : public QObject {
    Q_OBJECT
    QML_CONSTANT_CSTREF_PROPERTY (QString, bookId)

public:
    explicit BibleBook (const QString & bookId = "",
                        QObject       * parent = Q_NULLPTR);
};

#endif // BIBLEBOOK_H
