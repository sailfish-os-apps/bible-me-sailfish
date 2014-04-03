
#include "BibleText.h"

/*************************** TEXT *************************/

BibleText::BibleText (QObject * parent) : QObject (parent) {
    m_hasLocal      = false;
    m_isLoading     = false;
    m_percent       = 0.0;
    m_languageID    = "";
    m_languageTitle = "";
    m_bibleID       = "";
    m_bibleTitle    = "";
}

BibleText * BibleText::fromQtVariant (QVariantMap variant, QObject * parent) {
    BibleText * ret = new BibleText (parent);
    ret->updateWithQtVariant (variant);
    return ret;
}

void BibleText::updateWithQtVariant (QVariantMap variant) {
    if (variant.contains ("languageID")) {
        update_languageID (variant.value ("languageID").value<QByteArray> ());
    }
    if (variant.contains ("languageTitle")) {
        update_languageTitle (variant.value ("languageTitle").value<QString> ());
    }
    if (variant.contains ("bibleID")) {
        update_bibleID (variant.value ("bibleID").value<QByteArray> ());
    }
    if (variant.contains ("bibleTitle")) {
        update_bibleTitle (variant.value ("bibleTitle").value<QString> ());
    }
    if (variant.contains ("hasLocal")) {
        update_hasLocal (variant.value ("hasLocal").value<bool> ());
    }
    if (variant.contains ("isLoading")) {
        update_isLoading (variant.value ("isLoading").value<bool> ());
    }
    if (variant.contains ("percent")) {
        update_percent (variant.value ("percent").value<int> ());
    }
}

QVariantMap BibleText::toQtVariant () const {
    QVariantMap ret;
    ret.insert ("languageID",    m_languageID);
    ret.insert ("languageTitle", m_languageTitle);
    ret.insert ("bibleID",       m_bibleID);
    ret.insert ("bibleTitle",    m_bibleTitle);
    ret.insert ("hasLocal",      m_hasLocal);
    ret.insert ("isLoading",     m_isLoading);
    ret.insert ("percent",       m_percent);
    return ret;
}
