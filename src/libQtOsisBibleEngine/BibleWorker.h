#ifndef BIBLEWORKER_H
#define BIBLEWORKER_H

#include <QObject>
#include <QVector>
#include <QByteArray>
#include <QStandardPaths>
#include <QNetworkAccessManager>
#include <QSettings>

class BibleWorker : public QObject {
    Q_OBJECT

    struct AbstractMetaData {
        virtual QVariantMap toVariantMap (void) const = 0;
    };

    struct TextMetaData : public AbstractMetaData {
        const QByteArray languageId;
        const QByteArray bibleId;
        const QString    languageTitle;
        const QString    bibleTitle;

        bool hasLocal;
        bool isLoading;
        int  percent;

        explicit TextMetaData (const QByteArray & languageId,
                               const QByteArray & bibleId,
                               const QString    & languageTitle,
                               const QString    & bibleTitle,
                               const bool         hasLocal);

        QVariantMap toVariantMap (void) const Q_DECL_FINAL {
            return QVariantMap {
                { "languageId",    languageId    },
                { "languageTitle", languageTitle },
                { "bibleId",       bibleId       },
                { "bibleTitle",    bibleTitle    },
                { "hasLocal",      hasLocal      },
            };
        }
    };

    struct BookMetaData : public AbstractMetaData {
        const QByteArray bookId;
        const int        firstChapterIdx;
        const int        lastChapterIdx;

        explicit BookMetaData (const QByteArray & bookId,
                               const int          firstChapterIdx,
                               const int          lastChapterIdx);

        QVariantMap toVariantMap (void) const Q_DECL_FINAL {
            return QVariantMap {
                { "bookId", bookId },
            };
        }
    };

    struct ChapterMetaData : public AbstractMetaData {
        const QByteArray chapterId;
        const int        firstVerseIdx;
        const int        lastVerseIdx;

        explicit ChapterMetaData (const QByteArray & chapterId,
                                  const int          firstVerseIdx,
                                  const int          lastVerseIdx);

        QVariantMap toVariantMap (void) const Q_DECL_FINAL {
            return QVariantMap {
                { "chapterId", chapterId },
            };
        }
    };

    struct VerseMetaData : public AbstractMetaData {
        const QByteArray verseId;
        const QString    textContent;

        bool marked;

        explicit VerseMetaData (const QByteArray & verseId,
                                const QString    & textContent,
                                const bool         marked);

        QVariantMap toVariantMap (void) const Q_DECL_FINAL {
            return QVariantMap {
                { "verseId",     verseId     },
                { "textContent", textContent },
                { "marked",      marked      },
            };
        }
    };

public:
    explicit BibleWorker (QObject * parent = Q_NULLPTR);

public slots:
    void refreshIndex   (void);
    void downloadText   (const QString & langId, const QString & bibleId);
    void loadIndex      (void);
    void loadText       (const QString & textKey);
    void removeText     (const QString & textKey);
    void loadBook       (const QString & bookId);
    void loadChapter    (const QString & chapterId);
    void addBookmark    (const QString & verseId);
    void removeBookmark (const QString & verseId);
    void searchContent  (const QString & filter);

signals:
    // refresh
    void refreshStarted  (void);
    void refreshProgress (const int percent);
    void refreshFinished (const bool ok);

    // download
    void downloadStarted  (const QString & textKey);
    void downloadProgress (const QString & textKey, const int percent);
    void downloadFinished (const QString & textKey, const bool ok);

    // parse
    void loadStarted     (void);
    void textsLoaded     (const QList<QVariantMap> & texts);
    void booksLoaded     (const QList<QVariantMap> & books);
    void chaptersLoaded  (const QList<QVariantMap> & chapters);
    void versesLoaded    (const QList<QVariantMap> & verses);
    void bookmarksLoaded (const QList<QVariantMap> & bookmarks);
    void loadFinished    (void);
    void textRemoved     (const QString & textKey);

    // bookmarks
    void bookmarkAdded   (const QVariantMap & verse);
    void bookmarkRemoved (const QString     & verseId);

    // search
    void searchStarted  (void);
    void searchProgress (const int percent);
    void searchFound    (const QVariantMap & verse);
    void searchFinished (void);

protected slots:
    // refresh
    void onIndexDownloadProgress (qint64 bytesReceived, qint64 bytesTotal);
    void onIndexDownloadFinished (void);

    // download
    void onTextDownloadProgress  (qint64 bytesReceived, qint64 bytesTotal);
    void onTextDownloadFinished  (void);

    // parse
    void doParseIndex (void);
    void doParseText  (const QString & textKey);

    // bookmarks
    void doSaveBookmarks (void);

    // search
    void doSearchContent (const QString & filter);

protected:
    TextMetaData    * getTextMetaDataById    (const QByteArray & textKey)   const;
    BookMetaData    * getBookMetaDataById    (const QByteArray & bookId)    const;
    ChapterMetaData * getChapterMetaDataById (const QByteArray & chapterId) const;
    VerseMetaData   * getVerseMetaDataById   (const QByteArray & verseId)   const;

private:
    const QString REPOS_BASEURL { "http://gitlab.unique-conception.org/3rd-party/osis-bibles/raw/master" };
    const QString DATADIR_PATH  { QStandardPaths::writableLocation (QStandardPaths::DataLocation) };

    QVector<TextMetaData    *> m_listBibleTexts;
    QVector<BookMetaData    *> m_listBibleBooks;
    QVector<ChapterMetaData *> m_listBibleChapters;
    QVector<VerseMetaData   *> m_listBibleVerses;

    QHash<QByteArray, int> m_indexBibleTexts;
    QHash<QByteArray, int> m_indexBibleBooks;
    QHash<QByteArray, int> m_indexBibleChapters;
    QHash<QByteArray, int> m_indexBibleVerses;

    QVector<QByteArray> m_favorites;

    QSettings             m_conf;
    QNetworkAccessManager m_nam;
};

#endif // BIBLEWORKER_H
