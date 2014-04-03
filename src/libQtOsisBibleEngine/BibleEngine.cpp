
#include "BibleEngine.h"

#include <QDebug>

/*************************** ENGINE *******************************/

BibleEngine::BibleEngine (QObject * parent) : QObject (parent) {
    m_isSearching = false;
    m_isReloading = false;
    m_isFetching  = false;
    m_isLoading   = false;
    m_searchPercent = 0.0;

    QSettings::setDefaultFormat (QSettings::IniFormat);
    m_settings = new QSettings (this);
    m_settings->setValue ("lastStart", QDateTime::currentMSecsSinceEpoch ());

    m_textFontSize = m_settings->value   ("textFontSize", QVariant (32)).toReal ();
    m_currentTextKey = m_settings->value ("currentTextKey", QVariant ("")).toString ();
    m_showLocalOnly = m_settings->value  ("showLocalOnly", QVariant (false)).toBool ();

    QDir dataDir (QDir::homePath ());
    dataDir.mkpath (DATADIR_PATH);

    m_confMan            = new QNetworkConfigurationManager          (this);
    m_modelTexts         = QQmlObjectListModel::create<BibleText>    (this);
    m_modelBooks         = QQmlObjectListModel::create<BibleBook>    (this);
    m_modelChapters      = QQmlObjectListModel::create<BibleChapter> (this);
    m_modelVerses        = QQmlObjectListModel::create<BibleVerse>   (this);
    //m_modelBookmarks     = QQmlObjectListModel::create<BibleVerse>   (this);
    m_modelSearchResults = QQmlObjectListModel::create<BibleVerse>   (this);

    connect (m_confMan, &QNetworkConfigurationManager::onlineStateChanged, this,     &BibleEngine::hasConnectionChanged);
    connect (this,      &BibleEngine::showLocalOnlyChanged,                this,     &BibleEngine::saveShowLocalOnly);
    connect (this,      &BibleEngine::textFontSizeChanged,                 this,     &BibleEngine::saveTextFontSize);

    m_thread = new QThread (this);
    m_worker = new BibleWorker;

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

    m_worker->moveToThread (m_thread);
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
    qDebug () << "BibleEngine::requestIndex";
    emit refreshIndexRequested ();
}

void BibleEngine::requestText (QString langId, QString bibleId) {
    qDebug () << "BibleEngine::requestText" << langId << bibleId;
    emit downloadTextRequested (langId, bibleId);
}

void BibleEngine::removeText (QString key) {
    qDebug () << "BibleEngine::removeText" << key;
    emit removeTextRequested (key);
}

void BibleEngine::reloadIndex () {
    qDebug () << "BibleEngine::reloadIndex";
    emit loadIndexRequested ();
}

void BibleEngine::searchInText (QString str) {
    qDebug () << "BibleEngine::searchInText" << str;
    emit searchRequested (str);
}

void BibleEngine::setCurrentVerse (QString verseId) {
    qDebug () << "BibleEngine::setCurrentVerse" << verseId;
    emit setCurrentVerseRequested (verseId);
}

void BibleEngine::loadText (QString key) {
    qDebug () << "BibleEngine::loadText" << key;
    emit loadTextRequested (key);
}

void BibleEngine::loadBook (QString bookId) {
    qDebug () << "BibleEngine::loadBook" << bookId;
    emit loadBookRequested (bookId);
}

void BibleEngine::loadChapter (QString chapterId) {
    qDebug () << "BibleEngine::loadChapter" << chapterId;
    emit loadChapterRequested (chapterId);
}

void BibleEngine::saveTextFontSize (qreal textFontSize) {
    qDebug () << "BibleEngine::saveTextFontSize" << textFontSize;
    m_settings->setValue ("textFontSize", textFontSize);
}

void BibleEngine::saveShowLocalOnly (bool showLocalOnly) {
    qDebug () << "BibleEngine::saveShowLocalOnly" << showLocalOnly;
    m_settings->setValue ("showLocalOnly", showLocalOnly);
}

void BibleEngine::onTextsModelLoaded (QVariantList items) {
    qDebug () << "BibleEngine::onTextsModelLoaded" << items;
    m_indexTexts.clear ();
    m_modelTexts->clear ();
    foreach (QVariant variant, items) {
        QVariantMap item = variant.toMap ();
        BibleText * text = BibleText::fromQtVariant (item);
        m_modelTexts->append (text);

        QString langId  = item.value ("languageID").toString ();
        QString bibleId = item.value ("bibleID").toString ();
        QString key = QString ("%1__%2").arg (langId).arg (bibleId);
        m_indexTexts.insert (key, text);
    }
}

void BibleEngine::onBooksModelLoaded (QVariantList items) {
    qDebug () << "BibleEngine::onBooksModelLoaded" << items;
    m_modelBooks->clear ();
    foreach (QVariant variant, items) {
        BibleBook * book = BibleBook::fromQtVariant (variant.toMap ());
        m_modelBooks->append (book);
    }
}

void BibleEngine::onChaptersModelLoaded (QVariantList items) {
    qDebug () << "BibleEngine::onChaptersModelLoaded" << items;
    m_modelChapters->clear ();
    foreach (QVariant variant, items) {
        BibleChapter * chapter = BibleChapter::fromQtVariant (variant.toMap ());
        m_modelChapters->append (chapter);
    }
}

void BibleEngine::onVersesModelLoaded (QVariantList items) {
    qDebug () << "BibleEngine::onVersesModelLoaded" << items;
    m_modelVerses->clear ();
    foreach (QVariant variant, items) {
        BibleVerse * verse = BibleVerse::fromQtVariant (variant.toMap ());
        m_modelVerses->append (verse);
    }
}

void BibleEngine::onSearchStarted () {
    update_isSearching (true);
    m_modelSearchResults->clear ();
}

void BibleEngine::onSearchResultItem (QVariantMap verse) {
    m_modelSearchResults->append (BibleVerse::fromQtVariant (verse));
}

void BibleEngine::onSearchFinished () {
    update_isSearching (false);
}

void BibleEngine::onTextItemUpdated (QString textKey, QVariantMap item) {
    BibleText * text = m_indexTexts.value (textKey, NULL);
    if (text) {
        text->updateWithQtVariant (item);
    }
}

