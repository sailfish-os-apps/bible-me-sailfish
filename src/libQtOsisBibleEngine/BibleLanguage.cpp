
#include "BibleLanguage.h"

#include "BibleText.h"

BibleLanguage::BibleLanguage (const QString & languageId,
                              const QString & languageTitle,
                              QObject       * parent)
    : QObject         { parent        }
    , m_hasLocal      { false         }
    , m_languageId    { languageId    }
    , m_languageTitle { languageTitle }
{
    m_textsModel = new QQmlObjectListModel<BibleText> (this, "", "textKey");
}
