
#include "BibleEngine.h"

#include <QDir>
#include <QDateTime>
#include <QStringList>
//#include <QDebug>

#undef USE_OLD_QT_CONNECT

/*************************** ENGINE *******************************/

BibleEngine::BibleEngine (QObject * parent) : QObject (parent) {
    m_isSearching       = false;
    m_isReloading       = false;
    m_isFetching        = false;
    m_isLoading         = false;
    m_searchPercent     = 0.0;
    m_currentPositionId = QStringLiteral ("");
    m_currentTextKey    = QStringLiteral ("");

    m_settings = new QSettings (this);
    m_settings->setValue (QStringLiteral ("lastStart"), QDateTime::currentMSecsSinceEpoch ());

    if (!m_settings->contains (QStringLiteral ("bookmarks"))) {
        m_settings->setValue (QStringLiteral ("bookmarks"), QStringList ());
    }

    if (!m_settings->contains (QStringLiteral ("textFontSize"))) {
        m_settings->setValue (QStringLiteral ("textFontSize"), 32);
    }
    m_textFontSize = m_settings->value (QStringLiteral ("textFontSize")).toReal ();

    if (!m_settings->contains (QStringLiteral ("currentTextKey"))) {
        m_settings->setValue (QStringLiteral ("currentTextKey"), QStringLiteral (""));
    }
    m_currentTextKey = m_settings->value (QStringLiteral ("currentTextKey")).toString ();

    if (!m_settings->contains (QStringLiteral ("showLocalOnly"))) {
        m_settings->setValue (QStringLiteral ("showLocalOnly"), false);
    }
    m_showLocalOnly = m_settings->value (QStringLiteral ("showLocalOnly")).toBool ();

    if (!m_settings->contains (QStringLiteral ("currPosId"))) {
        m_settings->setValue (QStringLiteral ("currPosId"), QStringLiteral ("John.3.16"));
    }
    m_currentPositionId = m_settings->value (QStringLiteral ("currPosId")).toString ();

    QDir dataDir (QDir::homePath ());
    dataDir.mkpath (DATADIR_PATH);

    m_confMan            = new QNetworkConfigurationManager          (this);

    m_modelTexts         = new QQmlObjectListModel<BibleText>    (this, "", "textKey");
    m_modelBooks         = new QQmlObjectListModel<BibleBook>    (this, "", "bookId");
    m_modelChapters      = new QQmlObjectListModel<BibleChapter> (this, "", "chapterId");
    m_modelVerses        = new QQmlObjectListModel<BibleVerse>   (this, "", "verseId");
    m_modelBookmarks     = new QQmlObjectListModel<BibleVerse>   (this, "", "verseId");
    m_modelSearchResults = new QQmlObjectListModel<BibleVerse>   (this, "", "verseId");

    m_thread = new QThread (this);

    m_worker = new BibleWorker;
    m_worker->moveToThread (m_thread);

#ifndef USE_OLD_QT_CONNECT

    connect (m_confMan, &QNetworkConfigurationManager::onlineStateChanged, this,     &BibleEngine::hasConnectionChanged);
    connect (this,      &BibleEngine::showLocalOnlyChanged,                this,     &BibleEngine::saveShowLocalOnly);
    connect (this,      &BibleEngine::textFontSizeChanged,                 this,     &BibleEngine::saveTextFontSize);
    connect (m_thread,  &QThread::started,                                 m_worker, &BibleWorker::doInit);

    connect (this,      &BibleEngine::searchRequested,                     m_worker, &BibleWorker::doSearchVerse);
    connect (this,      &BibleEngine::refreshIndexRequested,               m_worker, &BibleWorker::doRefreshIndex);
    connect (this,      &BibleEngine::loadIndexRequested,                  m_worker, &BibleWorker::doLoadIndex);
    connect (this,      &BibleEngine::downloadTextRequested,               m_worker, &BibleWorker::doDownloadText);
    connect (this,      &BibleEngine::loadTextRequested,                   m_worker, &BibleWorker::doLoadText);
    connect (this,      &BibleEngine::removeTextRequested,                 m_worker, &BibleWorker::doRemoveText);
    connect (this,      &BibleEngine::loadBookRequested,                   m_worker, &BibleWorker::doLoadBook);
    connect (this,      &BibleEngine::loadChapterRequested,                m_worker, &BibleWorker::doLoadChapter);
    connect (this,      &BibleEngine::setCurrentVerseRequested,            m_worker, &BibleWorker::doNavigateToRefId);
    connect (this,      &BibleEngine::addBookmarkRequested,                m_worker, &BibleWorker::doAddBookmark);
    connect (this,      &BibleEngine::removeBookmark,                      m_worker, &BibleWorker::doRemoveBookmark);

    connect (m_worker,  &BibleWorker::textsModelLoaded,                    this,     &BibleEngine::onTextsModelLoaded);
    connect (m_worker,  &BibleWorker::booksModelLoaded,                    this,     &BibleEngine::onBooksModelLoaded);
    connect (m_worker,  &BibleWorker::chaptersModelLoaded,                 this,     &BibleEngine::onChaptersModelLoaded);
    connect (m_worker,  &BibleWorker::versesModelLoaded,                   this,     &BibleEngine::onVersesModelLoaded);
    connect (m_worker,  &BibleWorker::currentPositionChanged,              this,     &BibleEngine::update_currentPositionId);
    connect (m_worker,  &BibleWorker::searchStarted,                       this,     &BibleEngine::onSearchStarted);
    connect (m_worker,  &BibleWorker::searchResultItem,                    this,     &BibleEngine::onSearchResultItem);
    connect (m_worker,  &BibleWorker::searchFinished,                      this,     &BibleEngine::onSearchFinished);
    connect (m_worker,  &BibleWorker::searchPercentUpdated,                this,     &BibleEngine::update_searchPercent);
    connect (m_worker,  &BibleWorker::textItemUpdated,                     this,     &BibleEngine::onTextItemUpdated);
    connect (m_worker,  &BibleWorker::currentTextChanged,                  this,     &BibleEngine::update_currentTextKey);
    connect (m_worker,  &BibleWorker::loadTextStarted,                     this,     &BibleEngine::onLoadTextStarted);
    connect (m_worker,  &BibleWorker::loadTextFinished,                    this,     &BibleEngine::onLoadTextFinished);
    connect (m_worker,  &BibleWorker::refreshStarted,                      this,     &BibleEngine::onRefreshStarted);
    connect (m_worker,  &BibleWorker::refreshFinished,                     this,     &BibleEngine::onRefreshFinished);
    connect (m_worker,  &BibleWorker::bookmarkAdded,                       this,     &BibleEngine::onBookmarkAdded);
    connect (m_worker,  &BibleWorker::bookmarksLoaded,                     this,     &BibleEngine::onBookmarksLoaded);
    connect (m_worker,  &BibleWorker::bookmarkRemoved,                     this,     &BibleEngine::onBookmarkRemoved);

#else

    connect (m_confMan, SIGNAL(onlineStateChanged(bool)),               this,     SIGNAL(hasConnectionChanged(bool)));
    connect (this,      SIGNAL(showLocalOnlyChanged(bool)),             this,     SLOT(saveShowLocalOnly(bool)));
    connect (this,      SIGNAL(textFontSizeChanged(qreal)),             this,     SLOT(saveTextFontSize(qreal)));
    connect (m_thread,  SIGNAL(started()),                              m_worker, SLOT(doInit()));

    connect (this,      SIGNAL(searchRequested(QString)),               m_worker, SLOT(doSearchVerse(QString)));
    connect (this,      SIGNAL(refreshIndexRequested()),                m_worker, SLOT(doRefreshIndex()));
    connect (this,      SIGNAL(loadIndexRequested()),                   m_worker, SLOT(doLoadIndex()));
    connect (this,      SIGNAL(downloadTextRequested(QString,QString)), m_worker, SLOT(doDownloadText(QString,QString)));
    connect (this,      SIGNAL(loadTextRequested(QString)),             m_worker, SLOT(doLoadText(QString)));
    connect (this,      SIGNAL(removeTextRequested(QString)),           m_worker, SLOT(doRemoveText(QString)));
    connect (this,      SIGNAL(loadBookRequested(QString,bool)),        m_worker, SLOT(doLoadBook(QString,bool)));
    connect (this,      SIGNAL(loadChapterRequested(QString,bool)),     m_worker, SLOT(doLoadChapter(QString,bool)));
    connect (this,      SIGNAL(setCurrentVerseRequested(QString,bool)), m_worker, SLOT(doNavigateToRefId(QString,bool)));
    connect (this,      SIGNAL(addBookmarkRequested(QString)),          m_worker, SLOT(doAddBookmark(QString)));
    connect (this,      SIGNAL(removeBookmarkRequested(QString)),       m_worker, SLOT(doRemoveBookmark(QString)));

    connect (m_worker,  SIGNAL(textsModelLoaded(QVariantList)),         this,     SLOT(onTextsModelLoaded(QVariantList)));
    connect (m_worker,  SIGNAL(booksModelLoaded(QVariantList)),         this,     SLOT(onBooksModelLoaded(QVariantList)));
    connect (m_worker,  SIGNAL(chaptersModelLoaded(QVariantList)),      this,     SLOT(onChaptersModelLoaded(QVariantList)));
    connect (m_worker,  SIGNAL(versesModelLoaded(QVariantList)),        this,     SLOT(onVersesModelLoaded(QVariantList)));
    connect (m_worker,  SIGNAL(searchStarted()),                        this,     SLOT(onSearchStarted()));
    connect (m_worker,  SIGNAL(searchResultItem(QVariantMap)),          this,     SLOT(onSearchResultItem(QVariantMap)));
    connect (m_worker,  SIGNAL(searchFinished()),                       this,     SLOT(onSearchFinished()));
    connect (m_worker,  SIGNAL(textItemUpdated(QString,QVariantMap)),   this,     SLOT(onTextItemUpdated(QString,QVariantMap)));
    connect (m_worker,  SIGNAL(loadTextStarted()),                      this,     SLOT(onLoadTextStarted()));
    connect (m_worker,  SIGNAL(loadTextFinished()),                     this,     SLOT(onLoadTextFinished()));
    connect (m_worker,  SIGNAL(refreshStarted()),                       this,     SLOT(onRefreshStarted()));
    connect (m_worker,  SIGNAL(refreshFinished()),                      this,     SLOT(onRefreshFinished()));
    connect (m_worker,  SIGNAL(bookmarkAdded(QVariantMap)),             this,     SLOT(onBookmarkAdded(QVariantMap)));
    connect (m_worker,  SIGNAL(bookmarksLoaded(QVariantList)),          this,     SLOT(onBookmarksLoaded(QVariantList)));
    connect (m_worker,  SIGNAL(bookmarkRemoved(QString)),               this,     SLOT(onBookmarkRemoved(QString)));

    connect (m_worker,  &BibleWorker::currentPositionChanged,              this,     &BibleEngine::update_currentPositionId);
    connect (m_worker,  &BibleWorker::searchPercentUpdated,                this,     &BibleEngine::update_searchPercent);
    connect (m_worker,  &BibleWorker::currentTextChanged,                  this,     &BibleEngine::update_currentTextKey);

#endif

    m_thread->start ();
}

BibleEngine::~BibleEngine () {
    m_thread->quit ();
    m_thread->wait ();
}

bool BibleEngine::getConnection () const {
    return m_confMan->isOnline ();
}

void BibleEngine::requestIndex () {
    //qDebug () << "BibleEngine::requestIndex";
    emit refreshIndexRequested ();
}

void BibleEngine::requestText (QString langId, QString bibleId) {
    //qDebug () << "BibleEngine::requestText" << langId << bibleId;
    emit downloadTextRequested (langId, bibleId);
}

void BibleEngine::removeText (QString key) {
    //qDebug () << "BibleEngine::removeText" << key;
    emit removeTextRequested (key);
}

void BibleEngine::reloadIndex () {
    //qDebug () << "BibleEngine::reloadIndex";
    emit loadIndexRequested ();
}

void BibleEngine::searchInText (QString str) {
    //qDebug () << "BibleEngine::searchInText" << str;
    emit searchRequested (str);
}

void BibleEngine::setCurrentVerse (QString verseId, bool force) {
    //qDebug () << "BibleEngine::setCurrentVerse" << verseId << force;
    emit setCurrentVerseRequested (verseId, force);
}

void BibleEngine::addBookmark (QString verseId) {
    //qDebug () << "BibleEngine::addBookmark" << verseId;
    emit addBookmarkRequested (verseId);
}

void BibleEngine::removeBookmark (QString verseId){
    //qDebug () << "BibleEngine::removeBookmark" << verseId;
    emit removeBookmarkRequested (verseId);
}

void BibleEngine::loadText (QString key) {
    //qDebug () << "BibleEngine::loadText" << key;
    emit loadTextRequested (key);
}

void BibleEngine::loadBook (QString bookId, bool force) {
    //qDebug () << "BibleEngine::loadBook" << bookId << force;
    emit loadBookRequested (bookId, force);
}

void BibleEngine::loadChapter (QString chapterId, bool force) {
    //qDebug () << "BibleEngine::loadChapter" << chapterId << force;
    emit loadChapterRequested (chapterId, force);
}

void BibleEngine::saveTextFontSize (qreal textFontSize) {
    //qDebug () << "BibleEngine::saveTextFontSize" << textFontSize;
    m_settings->setValue (QStringLiteral ("textFontSize"), textFontSize);
}

void BibleEngine::saveShowLocalOnly (bool showLocalOnly) {
    //qDebug () << "BibleEngine::saveShowLocalOnly" << showLocalOnly;
    m_settings->setValue (QStringLiteral ("showLocalOnly"), showLocalOnly);
}

void BibleEngine::onTextsModelLoaded (QVariantList items) {
    //qDebug () << "BibleEngine::onTextsModelLoaded" << items;
    m_modelTexts->clear ();
    foreach (QVariant variant, items) {
        QVariantMap item = variant.toMap ();
        BibleText * text = BibleText::fromQtVariant (item);
        m_modelTexts->append (text);
    }
}

void BibleEngine::onBooksModelLoaded (QVariantList items) {
    //qDebug () << "BibleEngine::onBooksModelLoaded" << items.count ();
    m_modelBooks->clear ();
    foreach (QVariant variant, items) {
        BibleBook * book = BibleBook::fromQtVariant (variant.toMap ());
        m_modelBooks->append (book);
    }
}

void BibleEngine::onChaptersModelLoaded (QVariantList items) {
    //qDebug () << "BibleEngine::onChaptersModelLoaded" << items.count ();
    m_modelChapters->clear ();
    foreach (QVariant variant, items) {
        BibleChapter * chapter = BibleChapter::fromQtVariant (variant.toMap ());
        m_modelChapters->append (chapter);
    }
}

void BibleEngine::onVersesModelLoaded (QVariantList items) {
    //qDebug () << "BibleEngine::onVersesModelLoaded" << items.count ();
    m_modelVerses->clear ();
    foreach (QVariant variant, items) {
        BibleVerse * verse = BibleVerse::fromQtVariant (variant.toMap ());
        m_modelVerses->append (verse);
    }
}

void BibleEngine::onSearchStarted () {
    //qDebug () << "BibleEngine::onSearchStarted";
    update_isSearching (true);
    m_modelSearchResults->clear ();
}

void BibleEngine::onSearchResultItem (QVariantMap verse) {
    //qDebug () << "BibleEngine::onSearchResultItem" << verse;
    m_modelSearchResults->append (BibleVerse::fromQtVariant (verse));
}

void BibleEngine::onSearchFinished () {
    //qDebug () << "BibleEngine::onSearchFinished";
    update_isSearching (false);
}

void BibleEngine::onBookmarksLoaded (QVariantList items) {
    //qDebug () << "BibleEngine::onBookmarksLoaded" << items.count ();
    m_modelBookmarks->clear ();
    foreach (QVariant variant, items) {
        BibleVerse * bookmark = BibleVerse::fromQtVariant (variant.toMap ());
        m_modelBookmarks->append (bookmark);
        BibleVerse * verse = qobject_cast<BibleVerse *> (m_modelVerses->getByUid (QString::fromLocal8Bit (bookmark->get_verseId ())));
        if (verse) {
            verse->update_marked (true);
        }
    }
}

void BibleEngine::onBookmarkAdded (QVariantMap item) {
    //qDebug () << "BibleEngine::onBookmarkAdded" << item;
    BibleVerse * bookmark = BibleVerse::fromQtVariant (item);
    m_modelBookmarks->append (bookmark);
    BibleVerse * verse = qobject_cast<BibleVerse *> (m_modelVerses->getByUid (QString::fromLocal8Bit (bookmark->get_verseId ())));
    if (verse) {
        verse->update_marked (true);
    }
}

void BibleEngine::onBookmarkRemoved (QString verseId) {
    //qDebug () << "BibleEngine::onBookmarkRemoved" << verseId;
    for (int idx = 0; idx < m_modelBookmarks->count (); idx++) {
        if (m_modelBookmarks->get (idx)->property ("verseId") == verseId) {
            m_modelBookmarks->remove (idx);
            break;
        }
    }
    BibleVerse * verse = qobject_cast<BibleVerse *> (m_modelVerses->getByUid (verseId));
    if (verse) {
        verse->update_marked (false);
    }
}

void BibleEngine::onLoadTextStarted () {
    //qDebug () << "BibleEngine::onLoadTextStarted";
    update_isLoading (true);
}

void BibleEngine::onLoadTextFinished () {
    //qDebug () << "BibleEngine::onLoadTextFinished";
    update_isLoading (false);
}

void BibleEngine::onRefreshStarted () {
    //qDebug () << "BibleEngine::onRefreshStarted";
    update_isFetching (true);
}

void BibleEngine::onRefreshFinished () {
    //qDebug () << "BibleEngine::onRefreshFinished";
    update_isFetching (false);
}

void BibleEngine::onTextItemUpdated (QString textKey, QVariantMap item) {
    //qDebug () << "BibleEngine::onTextItemUpdated";
    BibleText * text = qobject_cast<BibleText *> (m_modelTexts->getByUid (textKey));
    if (text) {
        text->updateWithQtVariant (item);
    }
}

