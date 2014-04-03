
#include "BibleVerse.h"

/************************** VERSE **************************/

BibleVerse::BibleVerse (QObject * parent) : QObject (parent) {
    m_verseId     = "";
    m_textContent = "";
}

BibleVerse * BibleVerse::fromQtVariant (QVariantMap variant, QObject * parent) {
    BibleVerse * ret = new BibleVerse (parent);
    ret->updateWithQtVariant (variant);
    return ret;
}

void BibleVerse::updateWithQtVariant (QVariantMap variant) {
    if (variant.contains ("verseId")) {
        update_verseId (variant.value ("verseId").value<QByteArray> ());
    }
    if (variant.contains ("textContent")) {
        update_textContent (variant.value ("textContent").value<QString> ());
    }
}

QVariantMap BibleVerse::toQtVariant () const {
    QVariantMap ret;
    ret.insert ("verseId",     m_verseId);
    ret.insert ("textContent", m_textContent);
    return ret;
}
