
#include "BibleText.h"

/*************************** TEXT *************************/

BibleText::BibleText (QObject * parent) : QObject (parent) {
    m_hasLocal      = false;
    m_isLoading     = false;
    m_percent       = 0;
    m_textKey       = QByteArrayLiteral ("");
    m_languageID    = QByteArrayLiteral ("");
    m_languageTitle = QStringLiteral ("");
    m_bibleID       = QByteArrayLiteral ("");
    m_bibleTitle    = QStringLiteral ("");
}

BibleText * BibleText::fromQtVariant (QVariantMap variant, QObject * parent) {
    BibleText * ret = new BibleText (parent);
    ret->updateWithQtVariant (variant);
    return ret;
}

void BibleText::updateWithQtVariant (QVariantMap variant) {
    if (variant.contains (QStringLiteral ("textKey"))) {
        update_textKey (variant.value (QStringLiteral ("textKey")).value<QByteArray> ());
    }
    if (variant.contains (QStringLiteral ("languageID"))) {
        update_languageID (variant.value (QStringLiteral ("languageID")).value<QByteArray> ());
    }
    if (variant.contains (QStringLiteral ("languageTitle"))) {
        update_languageTitle (variant.value (QStringLiteral ("languageTitle")).value<QString> ());
    }
    if (variant.contains (QStringLiteral ("bibleID"))) {
        update_bibleID (variant.value (QStringLiteral ("bibleID")).value<QByteArray> ());
    }
    if (variant.contains (QStringLiteral ("bibleTitle"))) {
        update_bibleTitle (variant.value (QStringLiteral ("bibleTitle")).value<QString> ());
    }
    if (variant.contains (QStringLiteral ("hasLocal"))) {
        update_hasLocal (variant.value (QStringLiteral ("hasLocal")).value<bool> ());
    }
    if (variant.contains (QStringLiteral ("isLoading"))) {
        update_isLoading (variant.value (QStringLiteral ("isLoading")).value<bool> ());
    }
    if (variant.contains (QStringLiteral ("percent"))) {
        update_percent (variant.value (QStringLiteral ("percent")).value<int> ());
    }
}

QVariantMap BibleText::toQtVariant () const {
    QVariantMap ret;
    ret.insert (QStringLiteral ("textKey"),       m_textKey);
    ret.insert (QStringLiteral ("languageID"),    m_languageID);
    ret.insert (QStringLiteral ("languageTitle"), m_languageTitle);
    ret.insert (QStringLiteral ("bibleID"),       m_bibleID);
    ret.insert (QStringLiteral ("bibleTitle"),    m_bibleTitle);
    ret.insert (QStringLiteral ("hasLocal"),      m_hasLocal);
    ret.insert (QStringLiteral ("isLoading"),     m_isLoading);
    ret.insert (QStringLiteral ("percent"),       m_percent);
    return ret;
}
