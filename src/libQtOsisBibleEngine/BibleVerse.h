#ifndef BIBLEVERSE_H
#define BIBLEVERSE_H

#include <QObject>
#include <QQmlHelpers>
#include <QVariant>
#include <QString>
#include <QByteArray>

class BibleVerse : public QObject {
    Q_OBJECT
    QML_READONLY_PROPERTY (QByteArray, verseId)
    QML_READONLY_PROPERTY (QString,    textContent)

public:
    explicit BibleVerse (QObject * parent = NULL);

    static BibleVerse * fromQtVariant (QVariantMap variant, QObject * parent = NULL);
    QVariantMap toQtVariant () const;
    void updateWithQtVariant(QVariantMap variant);
};

#endif // BIBLEVERSE_H
