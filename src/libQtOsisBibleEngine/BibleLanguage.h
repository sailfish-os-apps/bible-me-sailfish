#ifndef BIBLELANGUAGE_H
#define BIBLELANGUAGE_H

#include <QObject>
#include <QVariant>
#include <QString>

#include "QQmlVarPropertyHelpers.h"
#include "QQmlConstRefPropertyHelpers.h"
#include "QQmlObjectListModel.h"

class BibleText;

class BibleLanguage : public QObject {
    Q_OBJECT
    QML_READONLY_VAR_PROPERTY    (bool,      hasLocal)
    QML_CONSTANT_CSTREF_PROPERTY (QString,   languageId)
    QML_CONSTANT_CSTREF_PROPERTY (QString,   languageTitle)
    QML_OBJMODEL_PROPERTY        (BibleText, textsModel)

public:
    explicit BibleLanguage (const QString & languageId    = "",
                            const QString & languageTitle = "",
                            QObject       * parent        = Q_NULLPTR);
};

#endif // BIBLELANGUAGE_H
