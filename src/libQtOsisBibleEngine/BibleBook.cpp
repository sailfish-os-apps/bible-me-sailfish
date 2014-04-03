
#include "BibleBook.h"

/*************************** BOOK *************************/

BibleBook::BibleBook (QObject * parent) : QObject (parent) {
    m_bookId          = "";
    m_firstChapterIdx = -1;
    m_lastChapterIdx  = -1;
}

BibleBook * BibleBook::fromQtVariant (QVariantMap variant, QObject * parent) {
    BibleBook * ret = new BibleBook (parent);
    ret->updateWithQtVariant (variant);
    return ret;
}

void BibleBook::updateWithQtVariant (QVariantMap variant) {
    if (variant.contains ("bookId")) {
        update_bookId (variant.value ("bookId").value<QByteArray> ());
    }
    if (variant.contains ("firstChapterIdx")) {
        update_firstChapterIdx (variant.value ("firstChapterIdx").value<int> ());
    }
    if (variant.contains ("lastChapterIdx")) {
        update_lastChapterIdx (variant.value ("lastChapterIdx").value<int> ());
    }
}

QVariantMap BibleBook::toQtVariant () const {
    QVariantMap ret;
    ret.insert ("bookId",          m_bookId);
    ret.insert ("firstChapterIdx", m_firstChapterIdx);
    ret.insert ("lastChapterIdx",  m_lastChapterIdx);
    return ret;
}
