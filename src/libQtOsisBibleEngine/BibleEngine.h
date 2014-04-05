#ifndef BIBLEENGINE_H
#define BIBLEENGINE_H

#include <QObject>
#include <QQmlHelpers>
#include <QHash>
#include <QString>
#include <QVariant>
#include <QtGlobal>
#include <QNetworkConfigurationManager>
#include <QThread>
#include <QSettings>

class QQmlObjectListModel;

class BibleWorker;
class BibleText;
class BibleBook;
class BibleChapter;
class BibleVerse;

class BibleEngine : public QObject {
    Q_OBJECT

    Q_PROPERTY (bool hasConnection READ getConnection NOTIFY hasConnectionChanged)

    QML_READONLY_PROPERTY (int,  searchPercent)
    QML_READONLY_PROPERTY (bool, isFetching)
    QML_READONLY_PROPERTY (bool, isReloading)
    QML_READONLY_PROPERTY (bool, isSearching)
    QML_READONLY_PROPERTY (bool, isLoading)
    QML_READONLY_PROPERTY (QString, currentPositionId)
    QML_READONLY_PROPERTY (QString, currentTextKey)

    QML_WRITABLE_PROPERTY (bool, showLocalOnly)
    QML_WRITABLE_PROPERTY (qreal, textFontSize)

    QML_CONSTANT_PROPERTY (QQmlObjectListModel  *, modelTexts)
    QML_CONSTANT_PROPERTY (QQmlObjectListModel  *, modelBooks)
    QML_CONSTANT_PROPERTY (QQmlObjectListModel  *, modelChapters)
    QML_CONSTANT_PROPERTY (QQmlObjectListModel  *, modelVerses)
    QML_CONSTANT_PROPERTY (QQmlObjectListModel  *, modelBookmarks)
    QML_CONSTANT_PROPERTY (QQmlObjectListModel  *, modelSearchResults)

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
    QHash<QString, BibleVerse     *> m_indexVerses;
    QHash<QString, BibleText      *> m_indexTexts;
    QThread                        * m_thread;
    BibleWorker                    * m_worker;
    QSettings                      * m_settings;
    QNetworkConfigurationManager   * m_confMan;
};

#endif // BIBLEENGINE_H
