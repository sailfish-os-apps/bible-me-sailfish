#ifndef BIBLEWORKER_H
#define BIBLEWORKER_H

#include <QObject>
#include <QNetworkAccessManager>
#include <QDir>
#include <QRegularExpression>
#include <QSettings>
#include <QStandardPaths>

#include "QQmlVarPropertyHelpers.h"
#include "QQmlConstRefPropertyHelpers.h"

#define REPOS_BASEURL QString ("http://gitlab.unique-conception.org/3rd-party/osis-bibles/raw/master")
#define DATADIR_PATH  QStandardPaths::writableLocation (QStandardPaths::DataLocation)

class BibleText;
class BibleBook;
class BibleChapter;
class BibleVerse;

class BibleWorker : public QObject {
    Q_OBJECT

public:
    explicit BibleWorker (QObject * parent = NULL);

public slots:
    void doInit            ();
    void doLoadIndex       ();
    void doRefreshIndex    ();
    void doDownloadText    (QString langId, QString bibleId);
    void doRemoveText      (QString textKey);
    void doLoadText        (QString textKey);
    void doLoadBook        (QString bookId, bool force = false);
    void doLoadChapter     (QString chapterId, bool force = false);
    void doSearchVerse     (QString searchToken);
    void doSaveCurrPosId   (QString currPosId);
    void doNavigateToRefId (QString refId, bool force = false);
    void doAddBookmark     (QString verseId);
    void doRemoveBookmark  (QString verseId);

signals:
    // for texts index
    void reloadIndexStarted     ();
    void reloadIndexFinished    ();
    void refreshStarted         ();
    void refreshFinished        ();

    // for text handling
    void loadTextStarted        ();
    void loadTextFinished       ();
    void currentTextChanged     (QString textKey);
    void textsModelLoaded       (QVariantList items);
    void textItemUpdated        (QString textKey, QVariantMap item);

    // for navigation
    void booksModelLoaded       (QVariantList items);
    void chaptersModelLoaded    (QVariantList items);
    void versesModelLoaded      (QVariantList items);
    void currentPositionChanged (QString      osisId);

    // for search
    void searchStarted          ();
    void searchResultItem       (QVariantMap verse);
    void searchPercentUpdated   (int         percent);
    void searchFinished         ();

    // for bookmarks
    void bookmarksLoaded        (QVariantList items);
    void bookmarkAdded          (QVariantMap  item);
    void bookmarkRemoved        (QString      verseId);

protected:
    QString canonize (const QString & str);

    BibleText    * getBibleTextFromKey   (QString textKey)   const;
    BibleBook    * getBibleBookFromId    (QString bookId)    const;
    BibleChapter * getBibleChapterFromId (QString chapterId) const;
    BibleVerse   * getBibleVerseFromId   (QString verseId)   const;

private slots:
    void onIndexRequestFinished ();
    void onTextRequestFinished  ();
    void onTextRequestProgress  (qint64 bytesReceived, qint64 bytesTotal);

private:
    QString                 m_currTextKey;
    QString                 m_currBookId;
    QString                 m_currChapId;
    QString                 m_currVerseId;
    QNetworkAccessManager * m_nam;
    QSettings             * m_settings;
    QStringList             m_bookmarksList;
    QRegularExpression      m_regxpSpace;
    QRegularExpression      m_regxpValid;
    QList<BibleText    *>   m_listBibleTexts;    // text = lang, title, hasLocal, isLoading, percent
    QList<BibleBook    *>   m_listBibleBooks;    // book = id, start chapter index, end chapter index
    QList<BibleChapter *>   m_listBibleChapters; // chapter = id, start verse index, end chapter verse
    QList<BibleVerse   *>   m_listBibleVerses;   // verse = id, text content
    QHash<QByteArray, int>  m_hashTextIndex;
    QHash<QByteArray, int>  m_hashBookIndex;
    QHash<QByteArray, int>  m_hashChapterIndex;
    QHash<QByteArray, int>  m_hashVerseIndex;
};


#endif // BIBLEWORKER_H
