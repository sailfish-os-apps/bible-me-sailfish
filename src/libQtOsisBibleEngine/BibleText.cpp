
#include "BibleText.h"

BibleText::BibleText (const QString & textKey,
                      const QString & bibleId,
                      const QString & bibleTitle,
                      const bool      hasLocal,
                      QObject       * parent)
    : QObject      { parent     }
    , m_textKey    { textKey    }
    , m_bibleId    { bibleId    }
    , m_bibleTitle { bibleTitle }
    , m_hasLocal   { hasLocal   }
    , m_isLoading  { false      }
    , m_percent    { 0          }
{ }
