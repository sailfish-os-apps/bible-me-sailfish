
#include "BibleEngine.h"

#include "BibleWorker.h"
#include "BibleLanguage.h"
#include "BibleText.h"
#include "BibleBook.h"
#include "BibleChapter.h"
#include "BibleVerse.h"

#include <QFile>
#include <QDebug>
#include <QThread>
#include <QTranslator>
#include <QCoreApplication>

BibleEngine::BibleEngine (QObject * parent)
    : QObject          { parent    }
    , m_isRefreshing   { false     }
    , m_isLoading      { false     }
    , m_isSearching    { false     }
    , m_showLocalOnly  { false     }
    , m_textFontSize   { 32        }
    , m_refreshPercent { 0         }
    , m_searchPercent  { 0         }
    , m_translator     { Q_NULLPTR }
{
    qRegisterMetaType<QList<QVariantMap>> ();
    m_languagesModel = new QQmlObjectListModel<BibleLanguage> (this, "", "languageId");
    m_booksModel     = new QQmlObjectListModel<BibleBook>     (this, "", "bookId");
    m_chaptersModel  = new QQmlObjectListModel<BibleChapter>  (this, "", "chapterId");
    m_versesModel    = new QQmlObjectListModel<BibleVerse>    (this, "", "verseId");
    m_resultsModel   = new QQmlObjectListModel<BibleVerse>    (this, "", "verseId");
    m_favoritesModel = new QQmlObjectListModel<BibleVerse>    (this, "", "verseId");
    m_translator     = new QTranslator (this);
    m_settings       = new QSettings   (this);
    m_thread         = new QThread     (this);
    m_worker         = new BibleWorker;
    m_worker->moveToThread (m_thread);
    /// signals from engine to worker thread
    connect (this, &BibleEngine::refreshIndexRequested,   m_worker, &BibleWorker::refreshIndex);
    connect (this, &BibleEngine::downloadTextRequested,   m_worker, &BibleWorker::downloadText);
    connect (this, &BibleEngine::loadIndexRequested,      m_worker, &BibleWorker::loadIndex);
    connect (this, &BibleEngine::loadTextRequested,       m_worker, &BibleWorker::loadText);
    connect (this, &BibleEngine::removeTextRequested,     m_worker, &BibleWorker::removeText);
    connect (this, &BibleEngine::loadBookRequested,       m_worker, &BibleWorker::loadBook);
    connect (this, &BibleEngine::loadChapterRequested,    m_worker, &BibleWorker::loadChapter);
    connect (this, &BibleEngine::addBookmarkRequested,    m_worker, &BibleWorker::addBookmark);
    connect (this, &BibleEngine::removeBookmarkRequested, m_worker, &BibleWorker::removeBookmark);
    connect (this, &BibleEngine::searchContentRequested,  m_worker, &BibleWorker::searchContent);
    /// signals from worker thread to engine
    connect (m_worker, &BibleWorker::refreshStarted,   this, &BibleEngine::onRefreshStarted);
    connect (m_worker, &BibleWorker::refreshProgress,  this, &BibleEngine::onRefreshProgress);
    connect (m_worker, &BibleWorker::refreshFinished,  this, &BibleEngine::onRefreshFinished);
    connect (m_worker, &BibleWorker::loadStarted,      this, &BibleEngine::onLoadStarted);
    connect (m_worker, &BibleWorker::loadFinished,     this, &BibleEngine::onLoadFinished);
    connect (m_worker, &BibleWorker::textsLoaded,      this, &BibleEngine::onTextsLoaded);
    connect (m_worker, &BibleWorker::booksLoaded,      this, &BibleEngine::onBooksLoaded);
    connect (m_worker, &BibleWorker::chaptersLoaded,   this, &BibleEngine::onChaptersLoaded);
    connect (m_worker, &BibleWorker::versesLoaded,     this, &BibleEngine::onVersesLoaded);
    connect (m_worker, &BibleWorker::bookmarksLoaded,  this, &BibleEngine::onBookmarksLoaded);
    connect (m_worker, &BibleWorker::downloadStarted,  this, &BibleEngine::onDownloadStarted);
    connect (m_worker, &BibleWorker::downloadProgress, this, &BibleEngine::onDownloadProgress);
    connect (m_worker, &BibleWorker::downloadFinished, this, &BibleEngine::onDownloadFinished);
    connect (m_worker, &BibleWorker::textRemoved,      this, &BibleEngine::onTextRemoved);
    connect (m_worker, &BibleWorker::bookmarkAdded,    this, &BibleEngine::onBookmarkAdded);
    connect (m_worker, &BibleWorker::bookmarkRemoved,  this, &BibleEngine::onBookmarkRemoved);
    connect (m_worker, &BibleWorker::searchStarted,    this, &BibleEngine::onSearchStarted);
    connect (m_worker, &BibleWorker::searchProgress,   this, &BibleEngine::onSearchProgress);
    connect (m_worker, &BibleWorker::searchFound,      this, &BibleEngine::onSearchFound);
    connect (m_worker, &BibleWorker::searchFinished,   this, &BibleEngine::onSearchFinished);
    /// signals from engine to settings
    connect (this, &BibleEngine::textFontSizeChanged, [this] (void) {
        m_settings->setValue (QStringLiteral ("textFontSize"), m_textFontSize);
    });
    connect (this, &BibleEngine::currentTextKeyChanged, [this] (void) {
        m_settings->setValue (QStringLiteral ("currentTextKey"), m_currentTextKey);
    });
    connect (this, &BibleEngine::currentVerseIdChanged, [this] (void) {
        m_settings->setValue (QStringLiteral ("currPosId"), m_currentVerseId);
    });
    connect (this, &BibleEngine::showLocalOnlyChanged, [this] (void) {
        m_settings->setValue (QStringLiteral ("showLocalOnly"), m_showLocalOnly);
    });
    connect (this, &BibleEngine::currentTranslationCodeChanged, [this] (void) {
        m_settings->setValue (QStringLiteral ("locale"), m_currentTranslationCode);
    });
    m_showLocalOnly          = m_settings->value (QStringLiteral ("showLocalOnly")).value<bool> ();
    m_textFontSize           = m_settings->value (QStringLiteral ("textFontSize")).value<int> ();
    m_currentTextKey         = m_settings->value (QStringLiteral ("currentTextKey")).value<QString> ();
    m_currentTranslationCode = m_settings->value (QStringLiteral ("locale")).value<QString> ();
    m_thread->start (QThread::LowPriority);
    emit loadIndexRequested ();
    if (!m_currentTextKey.isEmpty ()) {
        emit loadTextRequested (m_currentTextKey);
    }
    if (m_currentTranslationCode.isEmpty ()) {
        QLocale locale;
        m_currentTranslationCode = locale.name ().split ('_').first ().toLower ();
    }
    translateUi (m_currentTranslationCode);
    changePosition (m_settings->value (QStringLiteral ("currPosId")).value<QString> ());
}

BibleEngine::~BibleEngine (void) {
    m_thread->quit ();
    m_thread->wait ();
    m_worker->deleteLater ();
}

void BibleEngine::refreshIndex (void) {
    emit refreshIndexRequested ();
}

void BibleEngine::downloadText (const QString & languageId, const QString & bibleId) {
    emit downloadTextRequested (languageId, bibleId);
}

void BibleEngine::loadText (const QString & textKey) {
    update_currentTextKey (textKey);
    m_favoritesModel->clear ();
    m_resultsModel->clear ();
    m_versesModel->clear ();
    m_chaptersModel->clear ();
    m_booksModel->clear ();
    emit loadTextRequested (m_currentTextKey);
}

void BibleEngine::removeText (const QString & textKey) {
    emit removeTextRequested (textKey);
}

void BibleEngine::loadBook (const QString & bookId) {
    m_versesModel->clear ();
    m_chaptersModel->clear ();
    emit loadBookRequested (bookId);
}

void BibleEngine::loadChapter (const QString & chapterId) {
    m_versesModel->clear ();
    emit loadChapterRequested (chapterId);
}

void BibleEngine::changePosition (const QString & verseId) {
    const QStringList newParts = verseId.split ('.');
    const QStringList oldParts = m_currentVerseId.split ('.');
    if (newParts.count () == 3 &&
        oldParts.count () == 3) {
        const QString newBook = newParts.at (0);
        const QString oldBook = oldParts.at (0);
        if (newBook != oldBook) {
            update_currentBookId (newBook);
            loadBook (m_currentBookId);
        }
        const QString newChapter = (newBook % '.' % newParts.at (1));
        const QString oldChapter = (oldBook % '.' % oldParts.at (1));
        if (newChapter != oldChapter) {
            update_currentChapterId (newChapter);
            loadChapter (m_currentChapterId);
        }
    }
    update_currentVerseId (verseId);
}

void BibleEngine::addBookmark (const QString & verseId) {
    emit addBookmarkRequested (verseId);
}

void BibleEngine::removeBookmark (const QString & verseId) {
    emit removeBookmarkRequested (verseId);
}

void BibleEngine::searchContent (const QString & filter) {
    m_resultsModel->clear ();
    emit searchContentRequested (filter);
}

void BibleEngine::translateUi (const QString & code) {
    QCoreApplication::instance ()->removeTranslator (m_translator);
    QString locale = code;
    const QString qm = QStringLiteral (":/lang/%1.qm").arg (locale);
    if (QFile::exists (qm)) {
        m_translator->load (qm);
    }
    else {
        m_translator->load (QStringLiteral (":/lang/en.qm"));
        locale = "en";
    }
    if (m_translator->isEmpty ()) {
        qWarning () << "Translation is empty for language" << code;
    }
    QCoreApplication::instance ()->installTranslator (m_translator);
    update_currentTranslationCode (locale);
}

void BibleEngine::onRefreshStarted (void) {
    update_isRefreshing (true);
}

void BibleEngine::onRefreshProgress (const int percent) {
    update_refreshPercent (percent);
}

void BibleEngine::onRefreshFinished (const bool ok) {
    Q_UNUSED (ok)
    update_isRefreshing (false);
}

void BibleEngine::onLoadStarted (void) {
    update_isLoading (true);
}

void BibleEngine::onTextsLoaded (const QList<QVariantMap> & texts) {
    m_languagesModel->clear ();
    for (const QVariantMap & entry : texts) {
        const QString languageId = entry.value ("languageId").value<QString> ();
        BibleLanguage * languageItem = m_languagesModel->getByUid (languageId);
        if (!languageItem) {
            m_languagesModel->append (new BibleLanguage {
                                          languageId,
                                          entry.value ("languageTitle").value<QString> (),
                                      });
            languageItem = m_languagesModel->last ();
        }
        const QString bibleId = entry.value ("bibleId").value<QString> ();
        const QString textKey = (languageId % "__" % bibleId);
        languageItem->get_textsModel ()->append (new BibleText {
                                                     textKey,
                                                     bibleId,
                                                     entry.value ("bibleTitle").value<QString> (),
                                                     entry.value ("hasLocal").value<bool> (),
                                                 });
    }
    doUpdateLanguagesFlags ();
}

void BibleEngine::onBooksLoaded (const QList<QVariantMap> & books) {
    m_booksModel->clear ();
    QList<BibleBook *> tmp;
    for (const QVariantMap & entry : books) {
        tmp.append (new BibleBook {
                        entry.value ("bookId").value<QString> (),
                    });
    }
    m_booksModel->append (tmp);
    if (!m_currentVerseId.isEmpty ()) {
        const QStringList parts = m_currentVerseId.split ('.');
        if (parts.count () == 3) {
            loadBook    (parts.at (0));
            loadChapter (parts.at (0) % '.' % parts.at (1));
        }
    }
}

void BibleEngine::onChaptersLoaded (const QList<QVariantMap> & chapters) {
    m_chaptersModel->clear ();
    QList<BibleChapter *> tmp;
    for (const QVariantMap & entry : chapters) {
        tmp.append (new BibleChapter {
                        entry.value ("chapterId").value<QString> (),
                    });
    }
    m_chaptersModel->append (tmp);
}

void BibleEngine::onVersesLoaded (const QList<QVariantMap> & verses) {
    m_versesModel->clear ();
    QList<BibleVerse *> tmp;
    for (const QVariantMap & entry : verses) {
        tmp.append (new BibleVerse {
                        entry.value ("verseId").value<QString> (),
                        entry.value ("textContent").value<QString> (),
                        entry.value ("marked").value<bool> (),
                    });
    }
    m_versesModel->append (tmp);
}

void BibleEngine::onBookmarksLoaded (const QList<QVariantMap> & bookmarks) {
    m_favoritesModel->clear ();
    QList<BibleVerse *> tmp;
    for (const QVariantMap & entry : bookmarks) {
        tmp.append (new BibleVerse {
                        entry.value ("verseId").value<QString> (),
                        entry.value ("textContent").value<QString> (),
                        entry.value ("marked").value<bool> (),
                    });
    }
    m_favoritesModel->append (tmp);
}

void BibleEngine::onLoadFinished (void) {
    update_isLoading (false);
}

void BibleEngine::onDownloadStarted (const QString & textKey) {
    const QString languageId = textKey.split ("__").first ();
    if (BibleLanguage * bibleLanguage = m_languagesModel->getByUid (languageId)) {
        if (BibleText * bibleText = bibleLanguage->get_textsModel ()->getByUid (textKey)) {
            bibleText->update_isLoading (true);
        }
    }
}

void BibleEngine::onDownloadProgress (const QString & textKey, const int percent) {
    const QString languageId = textKey.split ("__").first ();
    if (BibleLanguage * bibleLanguage = m_languagesModel->getByUid (languageId)) {
        if (BibleText * bibleText = bibleLanguage->get_textsModel ()->getByUid (textKey)) {
            bibleText->update_percent (percent);
        }
    }
}

void BibleEngine::onDownloadFinished (const QString & textKey, const bool ok) {
    const QString languageId = textKey.split ("__").first ();
    if (BibleLanguage * bibleLanguage = m_languagesModel->getByUid (languageId)) {
        if (BibleText * bibleText = bibleLanguage->get_textsModel ()->getByUid (textKey)) {
            bibleText->update_hasLocal (ok);
            bibleText->update_isLoading (false);
            doUpdateLanguagesFlags ();
        }
    }
}

void BibleEngine::onTextRemoved (const QString & textKey) {
    const QString languageId = textKey.split ("__").first ();
    if (BibleLanguage * bibleLanguage = m_languagesModel->getByUid (languageId)) {
        if (BibleText * bibleText = bibleLanguage->get_textsModel ()->getByUid (textKey)) {
            bibleText->update_hasLocal (false);
        }
    }
}

void BibleEngine::onBookmarkAdded (const QVariantMap & verse) {
    const QString verseId = verse.value ("verseId").value<QString> ();
    m_favoritesModel->append (new BibleVerse {
                                  verseId,
                                  verse.value ("textContent").value<QString> (),
                                  verse.value ("marked").value<bool> (),
                              });
    if (BibleVerse * bibleVerse = m_versesModel->getByUid (verseId)) {
        bibleVerse->update_marked (true);
    }
}

void BibleEngine::onBookmarkRemoved (const QString & verseId) {
    if (BibleVerse * bibleVerse = m_versesModel->getByUid (verseId)) {
        bibleVerse->update_marked (false);
    }
    if (BibleVerse * bibleVerse = m_favoritesModel->getByUid (verseId)) {
        m_favoritesModel->remove (bibleVerse);
    }
}

void BibleEngine::onSearchStarted (void) {
    update_isSearching (true);
}

void BibleEngine::onSearchProgress (const int percent) {
    update_searchPercent (percent);
}

void BibleEngine::onSearchFound (const QVariantMap & verse) {
    m_resultsModel->append (new BibleVerse {
                                verse.value ("verseId").toString (),
                                verse.value ("textContent").toString (),
                            });
}

void BibleEngine::onSearchFinished (void) {
    update_isSearching (false);
}

void BibleEngine::doUpdateLanguagesFlags (void) {
    for (BibleLanguage * bibleLanguage : (* m_languagesModel)) {
        bool hasLocal = false;
        for (BibleText * bibleText : (* bibleLanguage->get_textsModel ())) {
            if (bibleText->get_hasLocal ()) {
                hasLocal = true;
                break;
            }
        }
        bibleLanguage->update_hasLocal (hasLocal);
    }
}
