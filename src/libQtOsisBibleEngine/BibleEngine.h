#ifndef BIBLEENGINE_H
#define BIBLEENGINE_H

#include <QObject>
#include <QSettings>
#include <QTranslator>

#include "QQmlVarPropertyHelpers.h"
#include "QQmlConstRefPropertyHelpers.h"
#include "QQmlPtrPropertyHelpers.h"
#include "QQmlObjectListModel.h"

class BibleWorker;
class BibleLanguage;
class BibleText;
class BibleBook;
class BibleChapter;
class BibleVerse;

class BibleEngine : public QObject {
    Q_OBJECT
    QML_READONLY_VAR_PROPERTY    (bool,          isRefreshing)
    QML_READONLY_VAR_PROPERTY    (bool,          isLoading)
    QML_READONLY_VAR_PROPERTY    (bool,          isSearching)
    QML_WRITABLE_VAR_PROPERTY    (bool,          showLocalOnly)
    QML_WRITABLE_VAR_PROPERTY    (int,           textFontSize)
    QML_READONLY_VAR_PROPERTY    (int,           refreshPercent)
    QML_READONLY_VAR_PROPERTY    (int,           searchPercent)
    QML_READONLY_CSTREF_PROPERTY (QString,       currentTextKey)
    QML_READONLY_CSTREF_PROPERTY (QString,       currentBookId)
    QML_READONLY_CSTREF_PROPERTY (QString,       currentChapterId)
    QML_READONLY_CSTREF_PROPERTY (QString,       currentVerseId)
    QML_READONLY_CSTREF_PROPERTY (QString,       currentTranslationCode)
    QML_OBJMODEL_PROPERTY        (BibleLanguage, languagesModel)
    QML_OBJMODEL_PROPERTY        (BibleBook,     booksModel)
    QML_OBJMODEL_PROPERTY        (BibleChapter,  chaptersModel)
    QML_OBJMODEL_PROPERTY        (BibleVerse,    versesModel)
    QML_OBJMODEL_PROPERTY        (BibleVerse,    resultsModel)
    QML_OBJMODEL_PROPERTY        (BibleVerse,    favoritesModel)

public:
    explicit BibleEngine (QObject * parent = Q_NULLPTR);
    virtual ~BibleEngine (void);

public slots:
    void refreshIndex   (void);
    void downloadText   (const QString & languageId, const QString & bibleId);
    void loadText       (const QString & textKey);
    void removeText     (const QString & textKey);
    void loadBook       (const QString & bookId);
    void loadChapter    (const QString & chapterId);
    void changePosition (const QString & verseId);
    void addBookmark    (const QString & verseId);
    void removeBookmark (const QString & verseId);
    void searchContent  (const QString & filter);
    void translateUi    (const QString & code);

signals:
    void refreshIndexRequested   (void);
    void downloadTextRequested   (const QString & langId, const QString & bibleId);
    void loadIndexRequested      (void);
    void loadTextRequested       (const QString & textKey);
    void removeTextRequested     (const QString & textKey);
    void loadBookRequested       (const QString & bookId);
    void loadChapterRequested    (const QString & chapterId);
    void addBookmarkRequested    (const QString & verseId);
    void removeBookmarkRequested (const QString & verseId);
    void searchContentRequested  (const QString & filter);

protected slots:
    void onRefreshStarted   (void);
    void onRefreshProgress  (const int percent);
    void onRefreshFinished  (const bool ok);

    void onLoadStarted      (void);
    void onLoadFinished     (void);

    void onTextsLoaded      (const QList<QVariantMap> & texts);
    void onBooksLoaded      (const QList<QVariantMap> & books);
    void onChaptersLoaded   (const QList<QVariantMap> & chapters);
    void onVersesLoaded     (const QList<QVariantMap> & verses);
    void onBookmarksLoaded  (const QList<QVariantMap> & bookmarks);

    void onDownloadStarted  (const QString & textKey);
    void onDownloadProgress (const QString & textKey, const int percent);
    void onDownloadFinished (const QString & textKey, const bool ok);
    void onTextRemoved      (const QString & textKey);

    void onBookmarkAdded    (const QVariantMap & verse);
    void onBookmarkRemoved  (const QString     & verseId);

    void onSearchStarted    (void);
    void onSearchProgress   (const int percent);
    void onSearchFound      (const QVariantMap & verse);
    void onSearchFinished   (void);

protected:
    void doUpdateLanguagesFlags (void);

private:
    BibleWorker * m_worker;
    QThread     * m_thread;
    QSettings   * m_settings;
    QTranslator * m_translator;
};

#endif // BIBLEENGINE_H
