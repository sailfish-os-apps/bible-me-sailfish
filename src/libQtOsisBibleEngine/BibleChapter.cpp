
#include "BibleChapter.h"

BibleChapter::BibleChapter (const QString & chapterId,
                            QObject       * parent)
    : QObject     { parent    }
    , m_chapterId { chapterId }
{ }
