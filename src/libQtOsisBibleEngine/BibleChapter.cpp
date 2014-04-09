
#include "BibleChapter.h"

/*************************** CHAPTER *************************/

BibleChapter::BibleChapter (QObject * parent) : QObject (parent) {
    m_chapterId     = QByteArrayLiteral ("");
    m_firstVerseIdx = -1;
    m_lastVerseIdx  = -1;
}

BibleChapter * BibleChapter::fromQtVariant (QVariantMap variant, QObject * parent) {
    BibleChapter * ret = new BibleChapter (parent);
    ret->updateWithQtVariant (variant);
    return ret;
}

void BibleChapter::updateWithQtVariant (QVariantMap variant) {
    if (variant.contains (QStringLiteral ("chapterId"))) {
        update_chapterId (variant.value (QStringLiteral ("chapterId")).value<QByteArray> ());
    }
    if (variant.contains (QStringLiteral ("firstVerseIdx"))) {
        update_firstVerseIdx (variant.value (QStringLiteral ("firstVerseIdx")).value<int> ());
    }
    if (variant.contains (QStringLiteral ("lastVerseIdx"))) {
        update_lastVerseIdx (variant.value (QStringLiteral ("lastVerseIdx")).value<int> ());
    }
}

QVariantMap BibleChapter::toQtVariant () const {
    QVariantMap ret;
    ret.insert (QStringLiteral ("chapterId"),     m_chapterId);
    ret.insert (QStringLiteral ("firstVerseIdx"), m_firstVerseIdx);
    ret.insert (QStringLiteral ("lastVerseIdx"),  m_lastVerseIdx);
    return ret;
}
