
#include "BibleVerse.h"

/************************** VERSE **************************/

BibleVerse::BibleVerse (QObject * parent) : QObject (parent) {
    m_marked      = false;
    m_verseId     = QByteArrayLiteral ("");
    m_textContent = QStringLiteral ("");
}

BibleVerse * BibleVerse::fromQtVariant (QVariantMap variant, QObject * parent) {
    BibleVerse * ret = new BibleVerse (parent);
    ret->updateWithQtVariant (variant);
    return ret;
}

void BibleVerse::updateWithQtVariant (QVariantMap variant) {
    if (variant.contains (QStringLiteral ("verseId"))) {
        update_verseId (variant.value (QStringLiteral ("verseId")).value<QByteArray> ());
    }
    if (variant.contains (QStringLiteral ("textContent"))) {
        update_textContent (variant.value (QStringLiteral ("textContent")).value<QString> ());
    }
    if (variant.contains (QStringLiteral ("marked"))) {
        update_marked (variant.value (QStringLiteral ("marked")).value<bool> ());
    }
}

QVariantMap BibleVerse::toQtVariant () const {
    QVariantMap ret;
    ret.insert (QStringLiteral ("verseId"),     m_verseId);
    ret.insert (QStringLiteral ("textContent"), m_textContent);
    ret.insert (QStringLiteral ("marked"),      m_marked);
    return ret;
}
