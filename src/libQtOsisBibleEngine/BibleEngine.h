#ifndef BIBLEENGINE_H
#define BIBLEENGINE_H

#include <QObject>
#include <QHash>
#include <QString>
#include <QVariant>
#include <QtGlobal>
#include <QNetworkConfigurationManager>
#include <QThread>
#include <QSettings>

#include "QQmlVarPropertyHelpers.h"
#include "QQmlPtrPropertyHelpers.h"
#include "QQmlConstRefPropertyHelpers.h"

#include "QQmlObjectListModel.h"

#include "BibleWorker.h"
#include "BibleText.h"
#include "BibleBook.h"
#include "BibleChapter.h"
#include "BibleVerse.h"

class BibleEngine : public QObject {
    Q_OBJECT

    Q_PROPERTY (bool hasConnection READ getConnection NOTIFY hasConnectionChanged)

    QML_READONLY_VAR_PROPERTY (int,  searchPercent)
    QML_READONLY_VAR_PROPERTY (bool, isFetching)
    QML_READONLY_VAR_PROPERTY (bool, isReloading)
    QML_READONLY_VAR_PROPERTY (bool, isSearching)
    QML_READONLY_VAR_PROPERTY (bool, isLoading)
    QML_READONLY_CSTREF_PROPERTY (QString, currentPositionId)
    QML_READONLY_CSTREF_PROPERTY (QString, currentTextKey)

    QML_WRITABLE_VAR_PROPERTY (bool, showLocalOnly)
    QML_WRITABLE_VAR_PROPERTY (qreal, textFontSize)

    QML_OBJMODEL_PROPERTY (BibleText, modelTexts)
    QML_OBJMODEL_PROPERTY (BibleBook, modelBooks)
    QML_OBJMODEL_PROPERTY (BibleChapter, modelChapters)
    QML_OBJMODEL_PROPERTY (BibleVerse, modelVerses)
    QML_OBJMODEL_PROPERTY (BibleVerse, modelBookmarks)
    QML_OBJMODEL_PROPERTY (BibleVerse, modelSearchResults)

public:
    explicit BibleEngine  (QObject * parent = NULL);
    virtual  ~BibleEngine  ();

    Q_INVOKABLE void requestIndex    ();
    Q_INVOKABLE void requestText     (QString langId, QString bibleId);
    Q_INVOKABLE void loadText        (QString key);
    Q_INVOKABLE void loadBook        (QString bookId, bool force = false);
    Q_INVOKABLE void loadChapter     (QString chapterId, bool force = false);
    Q_INVOKABLE void removeText      (QString key);
    Q_INVOKABLE void reloadIndex     ();
    Q_INVOKABLE void searchInText    (QString str);
    Q_INVOKABLE void setCurrentVerse (QString verseId, bool force = false);
    Q_INVOKABLE void addBookmark     (QString verseId);
    Q_INVOKABLE void removeBookmark  (QString verseId);

    bool getConnection () const;

signals:
    void hasConnectionChanged     (bool arg);
    void searchRequested          (QString token);
    void downloadTextRequested    (QString langId, QString bibleId);
    void refreshIndexRequested    ();
    void loadIndexRequested       ();
    void loadTextRequested        (QString textKey);
    void removeTextRequested      (QString textKey);
    void loadBookRequested        (QString bookId,    bool force = false);
    void loadChapterRequested     (QString chapterId, bool force = false);
    void setCurrentVerseRequested (QString verseId,   bool force = false);
    void addBookmarkRequested     (QString verseId);
    void removeBookmarkRequested  (QString verseId);

protected slots:
    void saveTextFontSize       (qreal textFontSize);
    void saveShowLocalOnly      (bool showLocalOnly);

private slots:
    void onTextsModelLoaded     (QVariantList items);
    void onBooksModelLoaded     (QVariantList items);
    void onChaptersModelLoaded  (QVariantList items);
    void onVersesModelLoaded    (QVariantList items);

    void onSearchStarted          ();
    void onSearchResultItem       (QVariantMap verse);
    void onSearchFinished         ();

    void onBookmarksLoaded        (QVariantList items);
    void onBookmarkAdded          (QVariantMap  item);
    void onBookmarkRemoved        (QString      verseId);

    void onLoadTextStarted        ();
    void onLoadTextFinished       ();

    void onRefreshStarted         ();
    void onRefreshFinished        ();

    void onTextItemUpdated     (QString textKey, QVariantMap item);

private:
    QThread                       * m_thread;
    BibleWorker                   * m_worker;
    QSettings                     * m_settings;
    QNetworkConfigurationManager  * m_confMan;
};

#endif // BIBLEENGINE_H
