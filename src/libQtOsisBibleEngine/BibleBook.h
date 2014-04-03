#ifndef BIBLEBOOK_H
#define BIBLEBOOK_H

#include <QObject>
#include <QQmlHelpers>
#include <QVariant>
#include <QString>
#include <QByteArray>

class BibleBook : public QObject {
    Q_OBJECT
    QML_READONLY_PROPERTY (QByteArray, bookId)
    QML_READONLY_PROPERTY (int,        firstChapterIdx)
    QML_READONLY_PROPERTY (int,        lastChapterIdx)

public:
    explicit BibleBook (QObject * parent = NULL);

    static BibleBook * fromQtVariant (QVariantMap variant, QObject * parent = NULL);
    QVariantMap toQtVariant () const;
    void updateWithQtVariant(QVariantMap variant);
};

#endif // BIBLEBOOK_H
