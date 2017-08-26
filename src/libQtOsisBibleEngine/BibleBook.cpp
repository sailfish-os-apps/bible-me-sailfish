
#include "BibleBook.h"

BibleBook::BibleBook (const QString & bookId,
                      QObject       * parent)
    : QObject  { parent }
    , m_bookId { bookId }
{ }
