
#include "BibleBook.h"

/*************************** BOOK *************************/

BibleBook::BibleBook (QObject * parent) : QObject (parent) {
    m_bookId          = QByteArrayLiteral ("");
    m_firstChapterIdx = -1;
    m_lastChapterIdx  = -1;
}

BibleBook * BibleBook::fromQtVariant (QVariantMap variant, QObject * parent) {
    BibleBook * ret = new BibleBook (parent);
    ret->updateWithQtVariant (variant);
    return ret;
}

void BibleBook::updateWithQtVariant (QVariantMap variant) {
    if (variant.contains (QStringLiteral ("bookId"))) {
        update_bookId (variant.value (QStringLiteral ("bookId")).value<QByteArray> ());
    }
    if (variant.contains (QStringLiteral ("firstChapterIdx"))) {
        update_firstChapterIdx (variant.value (QStringLiteral ("firstChapterIdx")).value<int> ());
    }
    if (variant.contains (QStringLiteral ("lastChapterIdx"))) {
        update_lastChapterIdx (variant.value (QStringLiteral ("lastChapterIdx")).value<int> ());
    }
}

QVariantMap BibleBook::toQtVariant () const {
    QVariantMap ret;
    ret.insert (QStringLiteral ("bookId"),          m_bookId);
    ret.insert (QStringLiteral ("firstChapterIdx"), m_firstChapterIdx);
    ret.insert (QStringLiteral ("lastChapterIdx"),  m_lastChapterIdx);
    return ret;
}
