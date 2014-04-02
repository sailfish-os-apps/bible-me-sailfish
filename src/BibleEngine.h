#ifndef BIBLEENGINE_H
#define BIBLEENGINE_H

#include <QObject>
#include <QQmlObjectListModel>
#include <QQmlVariantListModel>
#include <QQmlHelpers>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QXmlStreamReader>
#include <QDir>
#include <QNetworkConfigurationManager>
#include <QGuiApplication>
#include <QRegularExpression>
#include <QSettings>
#include <QDateTime>
#include <QStringList>
#include <QTimer>
#include <QStandardPaths>

#define REPOS_BASEURL QString ("https://raw.githubusercontent.com/lookitscook/osis-bibles/master")
#define DATADIR_PATH  QStandardPaths::writableLocation (QStandardPaths::DataLocation)

class BibleText : public QObject {
    Q_OBJECT
    QML_READONLY_PROPERTY (bool,       hasLocal)
    QML_READONLY_PROPERTY (bool,       isLoading)
    QML_READONLY_PROPERTY (qreal,      percent)
    QML_READONLY_PROPERTY (QString,    languageTitle)
    QML_READONLY_PROPERTY (QString,    bibleTitle)
    QML_READONLY_PROPERTY (QByteArray, languageID)
    QML_READONLY_PROPERTY (QByteArray, bibleID)

public: explicit BibleText (QObject * parent = NULL);
};

class BibleBook : public QObject {
    Q_OBJECT
    QML_READONLY_PROPERTY (QByteArray,            bookId)
    QML_CONSTANT_PROPERTY (QQmlObjectListModel *, modelChapters)

public: explicit BibleBook (QObject * parent = NULL);
};

class BibleChapter : public QObject {
    Q_OBJECT
    QML_READONLY_PROPERTY (QByteArray,            chapterId)
    QML_CONSTANT_PROPERTY (QQmlObjectListModel *, modelVerses)

public: explicit BibleChapter (QObject * parent = NULL);
};

class BibleVerse : public QObject {
    Q_OBJECT
    QML_READONLY_PROPERTY (QByteArray, verseId)
    QML_READONLY_PROPERTY (QString,    textContent)

public: explicit BibleVerse (QObject * parent = NULL);
};

class BibleEngine : public QObject {
    Q_OBJECT
    Q_PROPERTY (bool hasConnection READ getConnection NOTIFY hasConnectionChanged)
    QML_READONLY_PROPERTY (bool, isFetching)
    QML_READONLY_PROPERTY (bool, isReloading)
    QML_READONLY_PROPERTY (bool, isSearching)
    QML_READONLY_PROPERTY (bool, isLoading)
    QML_WRITABLE_PROPERTY (bool, showLocalOnly)
    QML_READONLY_PROPERTY (int, searchPercent)
    QML_WRITABLE_PROPERTY (qreal, textFontSize)
    QML_WRITABLE_PROPERTY (QString, currentTextKey)
    QML_READONLY_PROPERTY (BibleBook *, currentBookObj)
    QML_READONLY_PROPERTY (BibleChapter *, currentChapterObj)
    QML_READONLY_PROPERTY (BibleVerse *, currentVerseObj)
    QML_CONSTANT_PROPERTY (QQmlObjectListModel *, modelTexts)
    QML_CONSTANT_PROPERTY (QQmlObjectListModel *, modelBooks)
    QML_CONSTANT_PROPERTY (QQmlVariantListModel *, modelSearchResults)
    QML_CONSTANT_PROPERTY (QQmlVariantListModel *, modelBookmarks)

public:
    explicit BibleEngine (QObject * parent = NULL);

    Q_INVOKABLE void requestIndex ();
    Q_INVOKABLE void requestText  (QString langId, QString bibleId);
    Q_INVOKABLE void loadText     (QString key);
    Q_INVOKABLE void removeText   (QString key);
    Q_INVOKABLE void reloadIndex  ();
    Q_INVOKABLE void searchInText (QString str);
    Q_INVOKABLE void setCurrent   (QString refId = "");

    bool getConnection () const;

signals:
    void hasConnectionChanged (bool arg);

protected:
    QString canonize (const QString & str);

protected slots:
    void onIndexRequestFinished ();
    void onTextRequestFinished  ();
    void onTextRequestProgress  (qint64 bytesReceived, qint64 bytesTotal);
    void saveCurrentId          (QString currentId);
    void saveTextFontSize       (qreal textFontSize);
    void saveShowLocalOnly      (bool showLocalOnly);

private slots:
    void doInit   ();
    void doLoad   ();
    void doSearch ();

private:
    QDir                              m_dataDir;
    QString                           m_searchToken;
    QSettings                       * m_settings;
    QRegularExpression                m_regxpSpace;
    QRegularExpression                m_regxpValid;
    QNetworkAccessManager          *  m_nam;
    QNetworkConfigurationManager   *  m_confMan;
    QMap<QByteArray, BibleVerse   *> m_hashVerses;
    QMap<QByteArray, BibleChapter *> m_hashChapters;
    QMap<QByteArray, BibleBook    *> m_hashBooks;
    QMap<QByteArray, BibleText    *> m_hashTexts;
};

#endif // BIBLEENGINE_H
