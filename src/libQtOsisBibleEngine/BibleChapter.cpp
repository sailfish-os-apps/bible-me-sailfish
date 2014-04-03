
#include "BibleChapter.h"

/*************************** CHAPTER *************************/

BibleChapter::BibleChapter (QObject * parent) : QObject (parent) {
    m_chapterId     = "";
    m_firstVerseIdx = -1;
    m_lastVerseIdx  = -1;
}

BibleChapter * BibleChapter::fromQtVariant (QVariantMap variant, QObject * parent) {
    BibleChapter * ret = new BibleChapter (parent);
    ret->updateWithQtVariant (variant);
    return ret;
}

void BibleChapter::updateWithQtVariant (QVariantMap variant) {
    if (variant.contains ("chapterId")) {
        update_chapterId (variant.value ("chapterId").value<QByteArray> ());
    }
    if (variant.contains ("firstVerseIdx")) {
        update_firstVerseIdx (variant.value ("firstVerseIdx").value<int> ());
    }
    if (variant.contains ("lastVerseIdx")) {
        update_lastVerseIdx (variant.value ("lastVerseIdx").value<int> ());
    }
}

QVariantMap BibleChapter::toQtVariant () const {
    QVariantMap ret;
    ret.insert ("chapterId",     m_chapterId);
    ret.insert ("firstVerseIdx", m_firstVerseIdx);
    ret.insert ("lastVerseIdx",  m_lastVerseIdx);
    return ret;
}
