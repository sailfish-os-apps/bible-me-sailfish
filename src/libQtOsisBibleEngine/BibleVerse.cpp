
#include "BibleVerse.h"

BibleVerse::BibleVerse (const QString & verseId,
                        const QString & textContent,
                        const bool      marked,
                        QObject       * parent)
    : QObject       { parent      }
    , m_verseId     { verseId     }
    , m_textContent { textContent }
    , m_marked      { marked      }
{ }
