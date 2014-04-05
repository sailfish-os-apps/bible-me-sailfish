
#include "BibleWorker.h"
#include "BibleText.h"
#include "BibleBook.h"
#include "BibleChapter.h"
#include "BibleVerse.h"

#include <QNetworkRequest>
#include <QNetworkReply>
#include <QXmlStreamReader>
#include <QDateTime>
#include <QStringList>
#include <QDebug>

/************************** WORKER ****************************/

BibleWorker::BibleWorker (QObject * parent) : QObject (parent) {
    m_nam = new QNetworkAccessManager (this);
    m_settings = new QSettings (this);
    m_regxpSpace = QRegularExpression ("\\s+");
    m_regxpValid = QRegularExpression ("[^abcdefghijlkmnopqrstuvwxyz0123456789'\\s]");

    m_currTextKey   = m_settings->value ("currentTextKey").toString ();
    m_currVerseId   = m_settings->value ("currPosId").toString ();
    m_bookmarksList = m_settings->value ("bookmarks").toStringList ();
}

QString BibleWorker::canonize (const QString & str) {
    QString tmp = str;
    return tmp
            .toLower ()
            .trimmed ()
            .normalized (QString::NormalizationForm_KD)
            .replace (m_regxpSpace, " ")
            .replace (m_regxpValid, "");
}

BibleText * BibleWorker::getBibleTextFromKey (QString textKey) const {
    BibleText * ret = NULL;
    int idx = m_hashTextIndex.value (textKey.toLocal8Bit (), -1);
    if (idx >= 0 && idx < m_listBibleTexts.size ()) {
        ret = m_listBibleTexts.at (idx);
    }
    return ret;
}

BibleBook * BibleWorker::getBibleBookFromId (QString bookId) const {
    BibleBook * ret = NULL;
    int idx = m_hashBookIndex.value (bookId.toLocal8Bit (), -1);
    if (idx >= 0 && idx < m_listBibleBooks.size ()) {
        ret = m_listBibleBooks.at (idx);
    }
    return ret;
}

BibleChapter * BibleWorker::getBibleChapterFromId (QString chapterId) const {
    BibleChapter * ret = NULL;
    int idx = m_hashChapterIndex.value (chapterId.toLocal8Bit (), -1);
    if (idx >= 0 && idx < m_listBibleChapters.size ()) {
        ret = m_listBibleChapters.at (idx);
    }
    return ret;
}

BibleVerse * BibleWorker::getBibleVerseFromId (QString verseId) const {
    BibleVerse * ret = NULL;
    int idx = m_hashVerseIndex.value (verseId.toLocal8Bit (), -1);
    if (idx >= 0 && idx < m_listBibleVerses.size ()) {
        ret = m_listBibleVerses.at (idx);
    }
    return ret;
}

void BibleWorker::doInit () {
    qDebug () << "BibleWorker::doInit";

    QFile fileIndex (QString ("%1/index.xml").arg (DATADIR_PATH));
    if (fileIndex.exists ()) {
        doLoadIndex ();
    }
    if (!m_currTextKey.isEmpty ()) {
        qDebug () << "currTextKey=" << m_currTextKey;
        doLoadText (m_currTextKey);
    }
    else {
        qDebug () << "currTextKey is empty !";
    }
}

void BibleWorker::doLoadIndex () {
    qDebug () << "BibleWorker::doLoadIndex";

    reloadIndexStarted ();

    QFile fileIndex (QString ("%1/index.xml").arg (DATADIR_PATH));
    fileIndex.open (QIODevice::ReadOnly | QIODevice::Text | QIODevice::Unbuffered);

    QXmlStreamReader xml (&fileIndex);

    int     idx        = 0;
    QString langId     = "";
    QString langTitle  = "";
    QString bibleId    = "";
    QString bibleTitle = "";

    QVariantList ret;

    m_hashTextIndex.clear ();
    m_listBibleTexts.clear ();

    while (!xml.atEnd () && !xml.hasError ()) { /* We'll parse the XML until we reach end of it.*/
        /* Read next element.*/
        if (xml.readNext () == QXmlStreamReader::StartElement) {
            //qDebug () << "ELEMENT start";
            if (xml.name () == "bibles") {
                //qDebug () << "bibles";
            }
            else if (xml.name () == "language") {
                langId    = xml.attributes ().value ("id").toString ();
                langTitle = xml.attributes ().value ("title").toString ();

                //qDebug () << ">>> language" << "langId=" << langId << "langTitle=" << langTitle;
            }
            else if (xml.name () == "bible") {
                bibleId    = xml.attributes ().value ("id").toString ();
                bibleTitle = xml.attributes ().value ("title").toString ();

                QString key = QString ("%1__%2").arg (langId).arg (bibleId);

                //qDebug () << ">>> bible" << "bibleId=" << bibleId << "bibleTitle=" << bibleTitle;

                BibleText * text = new BibleText (this);
                text->update_languageID    (langId.toLocal8Bit ());
                text->update_languageTitle (langTitle);
                text->update_bibleID       (bibleId.toLocal8Bit ());
                text->update_bibleTitle    (bibleTitle);
                text->update_hasLocal      (QFile::exists (QString ("%1/%2.xml").arg (DATADIR_PATH).arg (key)));

                m_listBibleTexts.append (text);
                m_hashTextIndex.insert (key.toLocal8Bit (), idx);

                ret.append (text->toQtVariant ());

                idx++;
            }
        }
    }
    if (xml.hasError ()) { /* Error handling. */
        qWarning () << xml.errorString ();
    }

    qDebug () << "text keys=" << m_hashTextIndex.keys ();

    xml.clear ();
    fileIndex.close ();

    emit textsModelLoaded (ret);

    emit reloadIndexFinished ();
}

void BibleWorker::doRefreshIndex () {
    qDebug () << "BibleWorker::doRefreshIndex";

    emit refreshStarted ();

    QNetworkReply * reply = m_nam->get (QNetworkRequest (QUrl (QString ("%1/index.xml").arg (REPOS_BASEURL))));
    connect (reply, &QNetworkReply::finished, this, &BibleWorker::onIndexRequestFinished);
}

void BibleWorker::doDownloadText (QString langId, QString bibleId) {
    qDebug () << "BibleWorker::doDownloadText" << langId << bibleId;

    QString key = QString ("%1__%2").arg (langId).arg (bibleId);
    qWarning () << "requestText" << langId << bibleId;
    QFile fileText (QString ("%1/%2.xml").arg (DATADIR_PATH).arg (key));
    if (!fileText.exists ()) {
        BibleText * text = m_listBibleTexts.at (m_hashTextIndex.value (key.toLocal8Bit (), -1));
        if (text) {
            text->update_isLoading (true);
            emit textItemUpdated (key, text->toQtVariant ());
        }

        QNetworkReply * reply = m_nam->get (QNetworkRequest (QUrl (QString ("%1/%2/%3.xml").arg (REPOS_BASEURL).arg (langId).arg (bibleId))));
        reply->setProperty ("key", key);
        connect (reply, &QNetworkReply::finished,         this, &BibleWorker::onTextRequestFinished);
        connect (reply, &QNetworkReply::downloadProgress, this, &BibleWorker::onTextRequestProgress);
    }
}

void BibleWorker::doRemoveText (QString textKey) {
    qWarning () << "BibleWorker::doRemoveText" << textKey;

    QFile fileText (QString ("%1/%2.xml").arg (DATADIR_PATH).arg (textKey));
    if (fileText.exists ()) {
        BibleText * text = getBibleTextFromKey (textKey);
        if (text) {
            text->update_isLoading (false);
            text->update_hasLocal  (false);
            emit textItemUpdated (textKey, text->toQtVariant ());
        }

        fileText.remove ();

        if (textKey == m_currTextKey) {
            doLoadText ("");
        }
    }
}

void BibleWorker::doLoadText (QString textKey) {
    qDebug () << "BibleWorker::doLoadText" << textKey;

    emit loadTextStarted ();

    m_currTextKey = textKey;
    m_settings->setValue ("currentTextKey", m_currTextKey);

    m_hashBookIndex.clear ();
    m_hashChapterIndex.clear ();
    m_hashVerseIndex.clear ();

    m_listBibleBooks.clear ();
    m_listBibleChapters.clear ();
    m_listBibleVerses.clear ();

    QFile fileText (QString ("%1/%2.xml").arg (DATADIR_PATH).arg (m_currTextKey));
    if (fileText.exists ()) {
        fileText.open (QIODevice::ReadOnly | QIODevice::Text | QIODevice::Unbuffered);

        int bookAbsIdx    = 0;
        int chapterAbsIdx = 0;
        int verseAbsIdx   = 0;

        int bookRelIdx    = 0;
        int chapterRelIdx = 0;
        int verseRelIdx   = 0;

        QString bookId = "";
        QString chapId = "";
        QString versId = "";
        QString txtVal = "";

        BibleBook    * book    = NULL;
        BibleChapter * chapter = NULL;
        BibleVerse   * verse   = NULL;

        QXmlStreamReader xml (&fileText);

        while (!xml.atEnd () && !xml.hasError ()) { /* We'll parse the XML until we reach end of it.*/
            if (xml.readNext () == QXmlStreamReader::StartElement) { /* Read next element.*/
                if (xml.name () == "div") {
                    bookId = xml.attributes ().value ("osisID").toString ();

                    if (book) {
                        book->update_lastChapterIdx (chapterAbsIdx);
                    }

                    book = new BibleBook;
                    book->update_bookId (bookId.toLocal8Bit ());

                    m_listBibleBooks.append (book);
                    m_hashBookIndex.insert (bookId.toLocal8Bit (), bookAbsIdx);

                    bookAbsIdx++;
                    bookRelIdx++;

                    chapterRelIdx = 0;
                    verseRelIdx   = 0;
                }
                else if (xml.name () == "chapter") {
                    chapId = xml.attributes ().value ("osisID").toString ();

                    if (chapter) {
                        chapter->update_lastVerseIdx (verseAbsIdx);
                    }

                    chapter = new BibleChapter;
                    chapter->update_chapterId (chapId.toLocal8Bit ());

                    m_listBibleChapters.append (chapter);
                    m_hashChapterIndex.insert (chapId.toLocal8Bit (), chapterAbsIdx);

                    if (chapterRelIdx == 0) {
                        book->update_firstChapterIdx (chapterAbsIdx);
                    }
                    chapterAbsIdx++;
                    chapterRelIdx++;

                    verseRelIdx = 0;
                }
                else if (xml.name () == "verse") {
                    versId = xml.attributes ().value ("osisID").toString ();
                    txtVal = xml.readElementText (QXmlStreamReader::SkipChildElements);

                    verse = new BibleVerse;
                    verse->update_verseId (versId.toLocal8Bit ());
                    verse->update_textContent (txtVal);

                    m_listBibleVerses.append (verse);
                    m_hashVerseIndex.insert (versId.toLocal8Bit (), verseAbsIdx);

                    if (verseRelIdx == 0) {
                        chapter->update_firstVerseIdx (verseAbsIdx);
                    }
                    verseAbsIdx++;
                    verseRelIdx++;
                }
            }
        }
        if (xml.hasError ()) { /* Error handling. */
            qWarning () << xml.errorString ();
        }

        xml.clear ();
        fileText.close ();
    }

    QVariantList bookmarks;
    foreach (QString verseId, m_bookmarksList) {
        BibleVerse * verse = getBibleVerseFromId (verseId);
        if (verse) {
            verse->update_marked (true);
            bookmarks.append (verse->toQtVariant ());
        }
    }
    emit bookmarksLoaded (bookmarks);

    QVariantList books;
    foreach (BibleBook * book, m_listBibleBooks) {
        books.append (book->toQtVariant ());
    }
    emit booksModelLoaded    (books);

    emit currentTextChanged (m_currTextKey);

    if (!m_currVerseId.isEmpty ()) {
        doNavigateToRefId (m_currVerseId, true);
    }

    //qDebug () << "books="    << m_hashBookIndex.keys ();
    //qDebug () << "chapters=" << m_hashChapterIndex.keys ();
    //qDebug () << "verses="   << m_hashVerseIndex.keys ();

    emit loadTextFinished ();
}

void BibleWorker::doNavigateToRefId (QString refId, bool force) {
    qDebug () << "BibleWorker::doNavigateToRefId" << refId << force;

    if (!refId.isEmpty ()) {
        QStringList list       = (!refId.isEmpty () ? refId.split ('.') : QStringList ());
        QByteArray  bookName   = (list.count () >= 1 ? list.at (0).toLocal8Bit () : "Matt");
        QByteArray  chapterIdx = (list.count () >= 2 ? list.at (1).toLocal8Bit () : "1");
        QByteArray  verseIdx   = (list.count () >= 3 ? list.at (2).toLocal8Bit () : "1");

        QByteArray tmp;
        tmp.append (bookName);
        QString bookId = QString::fromLocal8Bit (tmp);
        doLoadBook (bookId, force);

        tmp.append ('.');
        tmp.append (chapterIdx);
        QString chapId = QString::fromLocal8Bit (tmp);
        doLoadChapter (chapId, force);

        tmp.append ('.');
        tmp.append (verseIdx);
        QString versId = QString::fromLocal8Bit (tmp);
        doSaveCurrPosId (versId);
    }
}

void BibleWorker::doAddBookmark (QString verseId) {
    qDebug () << "BibleWorker::doAddBookmark" << verseId;
    m_bookmarksList.append (verseId);
    m_settings->setValue ("bookmarks", m_bookmarksList);
    BibleVerse * verse = getBibleVerseFromId (verseId);
    if (verse) {
        verse->update_marked (true);
        emit bookmarkAdded (verse->toQtVariant ()); // FIXME : what if verse is not in current version of bible ?
    }
}

void BibleWorker::doRemoveBookmark (QString verseId) {
    qDebug () << "BibleWorker::doRemoveBookmark" << verseId;
    m_bookmarksList.removeAll (verseId);
    m_settings->setValue ("bookmarks", m_bookmarksList);
    BibleVerse * verse = getBibleVerseFromId (verseId);
    if (verse) {
        verse->update_marked (false);
    }
    emit bookmarkRemoved (verseId);
}

void BibleWorker::doSaveCurrPosId (QString currPosId) {
    qDebug () << "BibleWorker::doSaveCurrPosId" << currPosId;

    if (m_currVerseId != currPosId) {
        m_currVerseId = currPosId;
        m_settings->setValue ("currPosId", m_currVerseId);
        emit currentPositionChanged (m_currVerseId);
    }
}

void BibleWorker::doLoadBook (QString bookId, bool force) { // reset the chapters model
    qDebug () << "BibleWorker::doLoadBook" << bookId;

    if (bookId != m_currBookId || force) {
        m_currBookId = bookId;

        QVariantList ret;

        BibleBook * book = getBibleBookFromId (m_currBookId);
        if (book) {
            int start = book->get_firstChapterIdx ();
            int end   = book->get_lastChapterIdx  ();

            QList<BibleChapter *> tmp = m_listBibleChapters.mid (start, end - start);
            foreach (BibleChapter * chapter, tmp) {
                ret.append (chapter->toQtVariant ());
            }
        }

        emit chaptersModelLoaded (ret);

        doLoadChapter (QString ("%1.1").arg (m_currBookId));
    }
    else {
        qDebug () << "No need to reload chapters, same book";
    }
}

void BibleWorker::doLoadChapter (QString chapterId, bool force) { // reset the verses model
    qDebug () << "BibleWorker::doLoadChapter" << chapterId;

    if (chapterId != m_currChapId || force) {
        m_currChapId = chapterId;

        QVariantList ret;

        BibleChapter * chapter = getBibleChapterFromId (m_currChapId);
        if (chapter) {
            int start = chapter->get_firstVerseIdx ();
            int end   = chapter->get_lastVerseIdx  ();

            QList<BibleVerse *> tmp = m_listBibleVerses.mid (start, end - start);
            foreach (BibleVerse * verse, tmp) {
                ret.append (verse->toQtVariant ());
            }
        }

        emit versesModelLoaded (ret);

        doSaveCurrPosId (QString ("%1.1").arg (m_currChapId));
    }
    else {
        qDebug () << "No need to reload verses, same chapter";
    }
}

void BibleWorker::doSearchVerse (QString searchToken) {
    qDebug () << "BibleWorker::doSearchVerse" << searchToken;

    emit searchStarted ();

    QString canonicalToken = canonize (searchToken);

    qreal cnt = qreal (m_listBibleVerses.count ());
    qreal idx = 0.0;
    foreach (BibleVerse * verse, m_listBibleVerses) {
        if (canonize (verse->get_textContent ()).contains (canonicalToken)) {
            emit searchResultItem (verse->toQtVariant ());
        }
        idx += 1.0;
        emit searchPercentUpdated ((idx / cnt) * 100);
    }

    emit searchFinished ();
}

void BibleWorker::onIndexRequestFinished () {
    qDebug () << "BibleWorker::onIndexRequestFinished";

    QNetworkReply * reply = qobject_cast<QNetworkReply *>(sender ());
    if (reply) {
        if (reply->error () == QNetworkReply::NoError) {
            QByteArray data = reply->readAll ();
            //qDebug () << "data=" << data;

            QFile fileIndex (QString ("%1/index.xml").arg (DATADIR_PATH));
            fileIndex.open (QIODevice::ReadWrite | QIODevice::Truncate);
            if (data != fileIndex.readAll ()) {
                fileIndex.write (data);
                fileIndex.flush ();
            }
            fileIndex.close ();

            doLoadIndex ();
        }

        reply->deleteLater ();
    }

    emit refreshFinished ();
}

void BibleWorker::onTextRequestProgress (qint64 bytesReceived, qint64 bytesTotal) {
    qDebug () << "BibleWorker::onTextRequestProgress";

    QNetworkReply * reply = qobject_cast<QNetworkReply *>(sender ());
    if (reply) {
        QString key = reply->property ("key").toString ();
        qDebug () << "onTextRequestProgress:" << "key=" << key;

        BibleText * text = getBibleTextFromKey (key);
        if (text) {
            text->update_percent (qreal (bytesReceived) / qreal (bytesTotal) * 100);
            emit textItemUpdated (key, text->toQtVariant ());
        }
        qDebug () << "onTextRequestProgress:" << "text=" << text;

        qDebug () << "onTextRequestProgress:" << "bytesReceived=" << bytesReceived << "bytesTotal=" << bytesTotal;
    }
}

void BibleWorker::onTextRequestFinished () {
    qDebug () << "BibleWorker::onTextRequestFinished";

    QNetworkReply * reply = qobject_cast<QNetworkReply *>(sender ());
    if (reply) {
        QString key = reply->property ("key").toString ();
        qDebug () << "onTextRequestFinished:" << "key=" << key;

        BibleText * text = getBibleTextFromKey (key);
        if (reply->error () == QNetworkReply::NoError) {
            if (text) {
                text->update_isLoading (false);
                text->update_hasLocal  (true);
                emit textItemUpdated (key, text->toQtVariant ());
            }
            qDebug () << "onTextRequestFinished:" << "text=" << text;

            QByteArray data = reply->readAll ();
            //qDebug () << "data=" << data;

            QFile fileText (QString ("%1/%2.xml").arg (DATADIR_PATH).arg (key));
            fileText.open  (QIODevice::ReadWrite | QIODevice::Truncate);
            fileText.write (data);
            fileText.flush ();
            fileText.close ();
        }
        else {
            qWarning () << "Text request error :" << reply->errorString ();
            if (text) {
                text->update_isLoading (false);
                text->update_hasLocal  (false);
                emit textItemUpdated (key, text->toQtVariant ());
            }
        }

        reply->deleteLater ();
    }
}
