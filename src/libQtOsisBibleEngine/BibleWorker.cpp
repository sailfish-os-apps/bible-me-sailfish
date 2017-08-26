
#include "BibleWorker.h"

#include <QStringBuilder>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QFile>
#include <QTimer>
#include <QXmlStreamReader>
#include <QElapsedTimer>
#include <QDir>

BibleWorker::TextMetaData::TextMetaData (const QByteArray & languageId,
                                         const QByteArray & bibleId,
                                         const QString    & languageTitle,
                                         const QString    & bibleTitle,
                                         const bool         hasLocal)
    : languageId    { languageId    }
    , bibleId       { bibleId       }
    , languageTitle { languageTitle }
    , bibleTitle    { bibleTitle    }
    , hasLocal      { hasLocal      }
    , isLoading     { false         }
    , percent       { percent       }
{ }

BibleWorker::BookMetaData::BookMetaData (const QByteArray & bookId,
                                         const int          firstChapterIdx,
                                         const int          lastChapterIdx)
    : bookId          { bookId          }
    , firstChapterIdx { firstChapterIdx }
    , lastChapterIdx  { lastChapterIdx  }
{ }

BibleWorker::ChapterMetaData::ChapterMetaData (const QByteArray & chapterId,
                                               const int          firstVerseIdx,
                                               const int          lastVerseIdx)
    : chapterId     { chapterId     }
    , firstVerseIdx { firstVerseIdx }
    , lastVerseIdx  { lastVerseIdx  }
{ }

BibleWorker::VerseMetaData::VerseMetaData (const QByteArray & verseId,
                                           const QString    & textContent,
                                           const bool         marked)
    : verseId     { verseId     }
    , textContent { textContent }
    , marked      { marked      }
{ }

BibleWorker::BibleWorker (QObject * parent)
    : QObject { parent }
    , m_conf  { this   }
    , m_nam   { this   }
{
    QDir ().mkpath (DATADIR_PATH);
    const QStringList tmp = m_conf.value (QStringLiteral ("bookmarks")).value<QStringList> ();
    m_favorites.reserve (tmp.count ());
    for (const QString & verseId : tmp) {
        m_favorites.append (verseId.toLatin1 ());
    }
}

BibleWorker::TextMetaData * BibleWorker::getTextMetaDataById (const QByteArray & textKey) const {
    TextMetaData * ret = Q_NULLPTR;
    const int textIdx = m_indexBibleTexts.value (textKey, -1);
    if (textIdx >= 0 && textIdx < m_listBibleTexts.count ()) {
        ret = m_listBibleTexts.at (textIdx);
    }
    return ret;
}

BibleWorker::BookMetaData * BibleWorker::getBookMetaDataById (const QByteArray & bookId) const {
    BookMetaData * ret = Q_NULLPTR;
    const int bookIdx = m_indexBibleBooks.value (bookId, -1);
    if (bookIdx >= 0 && bookIdx < m_indexBibleBooks.count ()) {
        ret = m_listBibleBooks.at (bookIdx);
    }
    return ret;
}

BibleWorker::ChapterMetaData * BibleWorker::getChapterMetaDataById (const QByteArray & chapterId) const {
    ChapterMetaData * ret = Q_NULLPTR;
    const int chapterIdx = m_indexBibleChapters.value (chapterId, -1);
    if (chapterIdx >= 0 && chapterIdx < m_listBibleChapters.count ()) {
        ret = m_listBibleChapters.at (chapterIdx);
    }
    return ret;
}

BibleWorker::VerseMetaData * BibleWorker::getVerseMetaDataById (const QByteArray & verseId) const {
    VerseMetaData * ret = Q_NULLPTR;
    const int verseIdx = m_indexBibleVerses.value (verseId, -1);
    if (verseIdx >= 0 && verseIdx < m_listBibleVerses.count ()) {
        ret = m_listBibleVerses.at (verseIdx);
    }
    return ret;
}

void BibleWorker::refreshIndex (void) {
    QNetworkRequest request (QUrl (QStringLiteral ("%1/index.xml").arg (REPOS_BASEURL)));
    request.setRawHeader ("Accept-Encoding", "identity");
    if (QNetworkReply * reply = m_nam.get (request)) {
        emit refreshStarted ();
        connect (reply, &QNetworkReply::finished,         this, &BibleWorker::onIndexDownloadFinished);
        connect (reply, &QNetworkReply::downloadProgress, this, &BibleWorker::onIndexDownloadProgress);
    }
}

void BibleWorker::downloadText (const QString & langId, const QString & bibleId) {
    const QString textKey (langId % "__" % bibleId);
    QNetworkRequest request (QUrl (QStringLiteral ("%1/%2/%3.xml").arg (REPOS_BASEURL).arg (langId).arg (bibleId)));
    request.setRawHeader ("Accept-Encoding", "identity");
    if (QNetworkReply * reply = m_nam.get (request)) {
        emit downloadStarted (textKey);
        reply->setProperty ("textKey", textKey);
        connect (reply, &QNetworkReply::finished,         this, &BibleWorker::onTextDownloadFinished);
        connect (reply, &QNetworkReply::downloadProgress, this, &BibleWorker::onTextDownloadProgress);
    }
}

void BibleWorker::loadIndex (void) {
    emit loadStarted ();
    doParseIndex ();
    emit loadFinished ();
}

void BibleWorker::loadText (const QString & textKey) {
    emit loadStarted ();
    QElapsedTimer benchmark;
    benchmark.restart ();
    doParseText (textKey);
    qWarning () << "Loaded" << textKey << "in" << benchmark.elapsed () << "ms";
    if (!m_listBibleBooks.isEmpty ()) {
        QList<QVariantMap> books;
        const QVector<BookMetaData *> & listBibleBooks = m_listBibleBooks;
        for (BookMetaData * bookMetaData : listBibleBooks) {
            books.append (bookMetaData->toVariantMap ());
        }
        emit booksLoaded (books);
    }
    if (!m_favorites.isEmpty ()) {
        QList<QVariantMap> bookmarks;
        const QVector<QByteArray> favorites = m_favorites;
        for (const QByteArray & verseId : favorites) {
            if (VerseMetaData * verseMetaData = getVerseMetaDataById (verseId)) {
                bookmarks.append (verseMetaData->toVariantMap ());
            }
        }
        emit bookmarksLoaded (bookmarks);
    }
    emit loadFinished ();
}

void BibleWorker::removeText (const QString & textKey) {
    if (TextMetaData * textMetaData = getTextMetaDataById (textKey.toLatin1 ())) {
        QFile::remove (QStringLiteral ("%1/%2.xml").arg (DATADIR_PATH).arg (textKey));
        textMetaData->hasLocal = false;
        emit textRemoved (textKey);
    }
}

void BibleWorker::loadBook (const QString & bookId) {
    if (BookMetaData * bookMetaData = getBookMetaDataById (bookId.toLatin1 ())) {
        QList<QVariantMap> chapters;
        for (int chapterIdx = bookMetaData->firstChapterIdx; chapterIdx <= bookMetaData->lastChapterIdx; ++chapterIdx) {
            ChapterMetaData * chapterMetaData = m_listBibleChapters.at (chapterIdx);
            chapters.append (chapterMetaData->toVariantMap ());
        }
        emit chaptersLoaded (chapters);
    }
}

void BibleWorker::loadChapter (const QString & chapterId) {
    if (ChapterMetaData * chapterMetaData = getChapterMetaDataById (chapterId.toLatin1 ())) {
        QList<QVariantMap> verses;
        for (int verseIdx = chapterMetaData->firstVerseIdx; verseIdx <= chapterMetaData->lastVerseIdx; ++verseIdx) {
            VerseMetaData * verseMetaData = m_listBibleVerses.at (verseIdx);
            verses.append (verseMetaData->toVariantMap ());
        }
        emit versesLoaded (verses);
    }
}

void BibleWorker::doSaveBookmarks (void) {
    QStringList tmp;
    const QVector<QByteArray> favorites = m_favorites;
    tmp.reserve (favorites.count ());
    for (const QByteArray & verseId : favorites) {
        tmp.append (QString::fromLatin1 (verseId));
    }
    m_conf.setValue (QStringLiteral ("bookmarks"), tmp);
}

void BibleWorker::addBookmark (const QString & verseId) {
    if (!m_favorites.contains (verseId.toLatin1 ())) {
        m_favorites.append (verseId.toLatin1 ());
        if (VerseMetaData * verseMetaData = getVerseMetaDataById (verseId.toLatin1 ())) {
            verseMetaData->marked = true;
            emit bookmarkAdded (verseMetaData->toVariantMap ());
        }
        doSaveBookmarks ();
    }
}

void BibleWorker::removeBookmark (const QString & verseId) {
    if (m_favorites.contains (verseId.toLatin1 ())) {
        m_favorites.removeAll (verseId.toLatin1 ());
        if (VerseMetaData * verseMetaData = getVerseMetaDataById (verseId.toLatin1 ())) {
            verseMetaData->marked = false;
            emit bookmarkRemoved (verseId);
        }
        doSaveBookmarks ();
    }
}

void BibleWorker::searchContent (const QString & filter) {
    emit searchStarted ();
    QElapsedTimer benchmark;
    benchmark.restart ();
    doSearchContent (filter);
    qWarning () << "Searched" << filter << "in" << benchmark.elapsed () << "ms";
    emit searchFinished ();
}

void BibleWorker::onIndexDownloadProgress (qint64 bytesReceived, qint64 bytesTotal) {
    if (QNetworkReply * reply = qobject_cast<QNetworkReply *> (sender ())) {
        Q_UNUSED (reply)
        const int percent (bytesTotal > 0 ? int (bytesReceived * 100 / bytesTotal) : 0);
        emit refreshProgress (percent);
    }
}

void BibleWorker::onIndexDownloadFinished (void) {
    if (QNetworkReply * reply = qobject_cast<QNetworkReply *> (sender ())) {
        const bool ok = (reply->error () == QNetworkReply::NoError);
        if (ok) {
            QFile file (QStringLiteral ("%1/index.xml").arg (DATADIR_PATH));
            if (file.open  (QIODevice::WriteOnly | QIODevice::Truncate)) {
                file.write (reply->readAll ());
                file.flush ();
                file.close ();
                doParseIndex ();
            }
        }
        reply->deleteLater ();
        emit refreshFinished (ok);
    }
}

void BibleWorker::onTextDownloadProgress (qint64 bytesReceived, qint64 bytesTotal) {
    if (QNetworkReply * reply = qobject_cast<QNetworkReply *> (sender ())) {
        const QString textKey = reply->property ("textKey").value<QString> ();
        const int percent (bytesTotal > 0 ? int (bytesReceived * 100 / bytesTotal) : 0);
        if (TextMetaData * textMetaData = getTextMetaDataById (textKey.toLatin1 ())) {
            textMetaData->percent   = percent;
            textMetaData->isLoading = true;
        }
        emit downloadProgress (textKey, percent);
    }
}

void BibleWorker::onTextDownloadFinished (void) {
    if (QNetworkReply * reply = qobject_cast<QNetworkReply *> (sender ())) {
        const QString textKey = reply->property ("textKey").value<QString> ();
        const bool ok = (reply->error () == QNetworkReply::NoError);
        if (ok) {
            QFile file (QStringLiteral ("%1/%2.xml").arg (DATADIR_PATH).arg (textKey));
            if (file.open  (QIODevice::WriteOnly | QIODevice::Truncate)) {
                file.write (reply->readAll ());
                file.flush ();
                file.close ();
            }
        }
        if (TextMetaData * textMetaData = getTextMetaDataById (textKey.toLatin1 ())) {
            textMetaData->percent   = 100;
            textMetaData->isLoading = false;
            textMetaData->hasLocal  = ok;
        }
        reply->deleteLater ();
        emit downloadFinished (textKey, ok);
    }
}

void BibleWorker::doParseIndex (void) {
    QFile file (QStringLiteral ("%1/index.xml").arg (DATADIR_PATH));
    if (file.open (QIODevice::ReadOnly | QIODevice::Text)) {
        qDeleteAll (m_listBibleTexts);
        m_listBibleTexts.clear ();
        m_indexBibleTexts.clear ();
        int currentTextIdx = 0;
        QString currentLanguageName, currentBibleName;
        QByteArray currentLanguageId, currentBibleId;
        QXmlStreamReader xml (&file);
        while (!xml.atEnd () && !xml.hasError ()) {
            if (xml.readNext () == QXmlStreamReader::StartElement) {
                if (xml.name () == QStringLiteral ("bibles")) {
                    // NOTE : nothing special, just root node
                }
                else if (xml.name () == QStringLiteral ("language")) {
                    currentLanguageId   = xml.attributes ().value (QStringLiteral ("id")).toString ().toLatin1 ();
                    currentLanguageName = xml.attributes ().value (QStringLiteral ("title")).toString ();
                }
                else if (xml.name () == QStringLiteral ("bible")) {
                    currentBibleId   = xml.attributes ().value (QStringLiteral ("id")).toString ().toLatin1 ();
                    currentBibleName = xml.attributes ().value (QStringLiteral ("title")).toString ();
                    const QByteArray textKey (currentLanguageId % "__" % currentBibleId);
                    const bool isLocal = QFile::exists (QStringLiteral ("%1/%2.xml").arg (DATADIR_PATH).arg (QString::fromLatin1 (textKey)));
                    m_listBibleTexts.append (new TextMetaData {
                                                 currentLanguageId,
                                                 currentBibleId,
                                                 currentLanguageName,
                                                 currentBibleName,
                                                 isLocal,
                                             });
                    m_indexBibleTexts.insert (textKey, currentTextIdx);
                    ++currentTextIdx;
                }
            }
        }
        if (xml.hasError ()) {
            qWarning () << xml.errorString ();
        }
        xml.clear ();
        file.close ();
        QList<QVariantMap> texts;
        for (TextMetaData * textMetaData : m_listBibleTexts) {
            texts.append (textMetaData->toVariantMap ());
        }
        emit textsLoaded (texts);
    }
}

void BibleWorker::doParseText (const QString & textKey) {
    qDeleteAll (m_listBibleVerses);
    qDeleteAll (m_listBibleChapters);
    qDeleteAll (m_listBibleBooks);
    m_indexBibleVerses.clear ();
    m_indexBibleChapters.clear ();
    m_indexBibleBooks.clear ();
    m_listBibleBooks.clear ();
    m_listBibleChapters.clear ();
    m_listBibleVerses.clear ();
    QFile file (QStringLiteral ("%1/%2.xml").arg (DATADIR_PATH).arg (textKey));
    if (file.open (QIODevice::ReadOnly | QIODevice::Text)) {
        QXmlStreamReader xml (&file);
        int firstChapterIdx { 0 }, lastChapterIdx { 0 }, firstVerseIdx { 0 }, lastVerseIdx { 0 };
        QByteArray currentBookId, currentChapterId, currentVerseId;
        QString currentTextContent;
        static const QString OSIS_ID      = QStringLiteral ("osisID");
        static const QString TYPE_BOOK    = QStringLiteral ("div");
        static const QString TYPE_CHAPTER = QStringLiteral ("chapter");
        static const QString TYPE_VERSE   = QStringLiteral ("verse");
        while (!xml.atEnd () && !xml.hasError ()) {
            const QXmlStreamReader::TokenType token = xml.readNext ();
            const QStringRef type = xml.name ();
            if (type == TYPE_BOOK) {
                if (token == QXmlStreamReader::StartElement) {
                    firstChapterIdx = m_listBibleChapters.count ();
                    currentBookId   = xml.attributes ().value (OSIS_ID).toString ().toLatin1 ();
                }
                else if (token == QXmlStreamReader::EndElement) {
                    m_indexBibleBooks.insert (currentBookId, m_listBibleBooks.count ());
                    m_listBibleBooks.append (new BookMetaData {
                                                 currentBookId,
                                                 firstChapterIdx,
                                                 lastChapterIdx,
                                             });
                }
            }
            else if (type == TYPE_CHAPTER) {
                if (token == QXmlStreamReader::StartElement) {
                    firstVerseIdx    = m_listBibleVerses.count ();
                    currentChapterId = xml.attributes ().value (OSIS_ID).toString ().toLatin1 ();
                }
                else if (token == QXmlStreamReader::EndElement) {
                    lastChapterIdx = m_listBibleChapters.count ();
                    m_indexBibleChapters.insert (currentChapterId, lastChapterIdx);
                    m_listBibleChapters.append (new ChapterMetaData {
                                                    currentChapterId,
                                                    firstVerseIdx,
                                                    lastVerseIdx,
                                                });
                }
            }
            else if (type == TYPE_VERSE) {
                currentVerseId     = xml.attributes ().value (OSIS_ID).toString ().toLatin1 ();
                currentTextContent = xml.readElementText (QXmlStreamReader::SkipChildElements);
                lastVerseIdx = m_listBibleVerses.count ();
                m_indexBibleVerses.insert (currentVerseId, lastVerseIdx);
                m_listBibleVerses.append (new VerseMetaData {
                                              currentVerseId,
                                              currentTextContent,
                                              m_favorites.contains (currentVerseId),
                                          });
            }
            else { }
        }
        if (xml.hasError ()) {
            qWarning () << xml.errorString ();
        }
        xml.clear ();
        file.close ();
    }
}

static inline QString canonize (const QString & str) {
    return str.toLower ().trimmed ().normalized (QString::NormalizationForm_KD).replace ("\\s+", " ");
}

void BibleWorker::doSearchContent (const QString & filter) {
    const int total = m_listBibleVerses.count ();
    const QString canonicalToken = canonize (filter);
    const QVector<VerseMetaData *> & listBibleVerses = m_listBibleVerses;
    int idx { 0 }, percent { 0 };
    for (VerseMetaData * verseMetaData : listBibleVerses) {
        if (canonize (verseMetaData->textContent).contains (canonicalToken)) {
            emit searchFound (verseMetaData->toVariantMap ());
        }
        ++idx;
        const int tmp = (idx * 100 / total);
        if (tmp != percent) {
            percent = tmp;
            emit searchProgress (percent);
        }
    }
}
