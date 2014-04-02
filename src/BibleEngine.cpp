
#include "BibleEngine.h"

#include <QDebug>

BibleText::BibleText (QObject * parent) : QObject (parent) {
    m_hasLocal      = false;
    m_isLoading     = false;
    m_percent       = 0.0;
    m_languageID    = "";
    m_languageTitle = "";
    m_bibleID       = "";
    m_bibleTitle    = "";
}

BibleBook::BibleBook (QObject * parent) : QObject (parent) {
    m_bookId        = "";
    m_modelChapters = QQmlObjectListModel::create<BibleChapter> (this);
}

BibleChapter::BibleChapter (QObject * parent) : QObject (parent) {
    m_chapterId   = "";
    m_modelVerses = QQmlObjectListModel::create<BibleVerse> (this);
}

BibleVerse::BibleVerse (QObject * parent) : QObject (parent) {
    m_verseId     = "";
    m_textContent = "";
}

BibleEngine::BibleEngine (QObject * parent) : QObject (parent) {
    QSettings::setDefaultFormat (QSettings::IniFormat);
    m_settings = new QSettings (this);
    m_settings->setValue ("lastStart", QDateTime::currentMSecsSinceEpoch ());
    m_isSearching = false;
    m_isReloading = false;
    m_isFetching  = false;
    m_isLoading   = false;
    m_searchPercent = 0.0;
    m_currentBookObj = NULL;
    m_currentChapterObj = NULL;
    m_currentVerseObj = NULL;
    m_textFontSize = m_settings->value ("textFontSize", QVariant (32)).toReal ();
    m_currentTextKey = m_settings->value ("currentTextKey", QVariant ("")).toString ();
    m_showLocalOnly = m_settings->value ("showLocalOnly", QVariant (false)).toBool ();
    m_regxpSpace = QRegularExpression ("\\s+");
    m_regxpValid = QRegularExpression ("[^abcdefghijlkmnopqrstuvwxyz0123456789'\\s]");
    m_dataDir = QDir (QDir::homePath ());
    m_dataDir.mkpath (DATADIR_PATH);
    m_dataDir.cd     (DATADIR_PATH);
    m_nam                = new QNetworkAccessManager              (this);
    m_confMan            = new QNetworkConfigurationManager       (this);
    m_modelTexts         = QQmlObjectListModel::create<BibleText> (this);
    m_modelBooks         = QQmlObjectListModel::create<BibleBook> (this);
    m_modelBookmarks     = new QQmlVariantListModel               (this);
    m_modelSearchResults = new QQmlVariantListModel               (this);
    connect (m_confMan, &QNetworkConfigurationManager::onlineStateChanged,
             this, &BibleEngine::hasConnectionChanged);
    connect (this, &BibleEngine::currentTextKeyChanged,
             this, &BibleEngine::loadText);
    connect (this, &BibleEngine::showLocalOnlyChanged,
             this, &BibleEngine::saveShowLocalOnly);
    connect (this, &BibleEngine::textFontSizeChanged,
             this, &BibleEngine::saveTextFontSize);
    QTimer::singleShot (10, this, SLOT (doInit ()));
}

void BibleEngine::doInit () {
    reloadIndex ();
    if (m_currentTextKey != "") {
        loadText (m_currentTextKey);
        setCurrent (m_settings->value ("currentId", QVariant ("")).toString ());
    }
}

void BibleEngine::reloadIndex () {
    update_isReloading (true);
    QFile fileIndex (QString ("%1/index.xml").arg (m_dataDir.path ()));
    fileIndex.open (QIODevice::ReadOnly);

    QXmlStreamReader xml (&fileIndex);

    QString langId     = "";
    QString langTitle  = "";
    QString bibleId    = "";
    QString bibleTitle = "";

    m_modelTexts->clear ();

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

                BibleText * text = new BibleText;
                text->update_languageID    (langId.toLocal8Bit ());
                text->update_languageTitle (langTitle);
                text->update_bibleID       (bibleId.toLocal8Bit ());
                text->update_bibleTitle    (bibleTitle);
                text->update_hasLocal      (QFile::exists (QString ("%1/%2.xml").arg (m_dataDir.path ()).arg (key)));
                m_modelTexts->append (text);

                m_hashTexts.insert (key.toLocal8Bit (), text);
            }
        }
    }
    if (xml.hasError ()) { /* Error handling. */
        qWarning () << xml.errorString ();
    }

    //qDebug () << "keys=" << m_hashTexts.keys ();

    xml.clear ();
    fileIndex.close ();
    update_isReloading (false);
}


QString BibleEngine::canonize (const QString & str) {
    QString tmp = str;
    return tmp
            .toLower ()
            .trimmed ()
            .normalized (QString::NormalizationForm_KD)
            .replace (m_regxpSpace, " ")
            .replace (m_regxpValid, "");
}

void BibleEngine::searchInText (QString str) {
    update_isSearching (true);
    m_searchToken = canonize (str);
    //qDebug () << "m_searchToken=" << m_searchToken;
    QTimer::singleShot (10, this, SLOT (doSearch ()));
}

void BibleEngine::doSearch () {
    m_modelSearchResults->clear ();
    QGuiApplication::processEvents ();
    qreal cnt = qreal (m_hashVerses.count ());
    qreal idx = 0.0;
    foreach (BibleVerse * verse, m_hashVerses.values ()) {
        if (canonize (verse->get_textContent ()).contains (m_searchToken)) {
            m_modelSearchResults->append (QVariant::fromValue (verse));
        }
        update_searchPercent ((idx / cnt) * 100);
        QGuiApplication::processEvents ();
        idx += 1.0;
    }
    update_isSearching (false);
}

void BibleEngine::setCurrent (QString refId) {
    QStringList list    = (!refId.isEmpty () ? refId.split ('.') : QStringList ());
    QByteArray  book    = (list.count () >= 1 ? list.at (0).toLocal8Bit () : "Matt");
    QByteArray  chapter = (list.count () >= 2 ? list.at (1).toLocal8Bit () : "1");
    QByteArray  verse   = (list.count () >= 3 ? list.at (2).toLocal8Bit () : "1");
    QByteArray tmp;
    tmp.append (book);
    update_currentBookObj (m_hashBooks.value (tmp, NULL));
    if (m_currentBookObj) {
        tmp.append ('.');
        tmp.append (chapter);
        update_currentChapterObj (m_hashChapters.value (tmp, NULL));
        if (m_currentChapterObj) {
            tmp.append ('.');
            tmp.append (verse);
            update_currentVerseObj (m_hashVerses.value (tmp, NULL));
        }
        else {
            tmp.append ('.');
            tmp.append (verse);
            update_currentVerseObj (NULL);
        }
    }
    else {
        tmp.append ('.');
        tmp.append (chapter);
        tmp.append ('.');
        tmp.append (verse);
        update_currentChapterObj (NULL);
        update_currentVerseObj (NULL);
    }

    saveCurrentId (QString::fromLocal8Bit (tmp));

    qDebug () << "setCurrent" << refId
              << "book=" << book << m_currentBookObj
              << "chapter=" << chapter << m_currentChapterObj
              << "verse=" << verse << m_currentVerseObj;
}

bool BibleEngine::getConnection () const {
    return m_confMan->isOnline ();
}

void BibleEngine::requestIndex () {
    update_isFetching (true);
    QNetworkReply * reply = m_nam->get (QNetworkRequest (QUrl (QString ("%1/index.xml").arg (REPOS_BASEURL))));
    connect (reply, &QNetworkReply::finished, this, &BibleEngine::onIndexRequestFinished);
}

void BibleEngine::onIndexRequestFinished () {
    QNetworkReply * reply = qobject_cast<QNetworkReply *>(sender ());
    if (reply) {
        if (reply->error () == QNetworkReply::NoError) {
            QByteArray data = reply->readAll ();
            //qDebug () << "onIndexRequestFinished: data=" << data;

            QFile fileIndex (QString ("%1/index.xml").arg (m_dataDir.path ()));
            fileIndex.open (QIODevice::ReadWrite | QIODevice::Truncate);
            if (data != fileIndex.readAll ()) {
                fileIndex.write (data);
                fileIndex.flush ();
            }
            fileIndex.close ();

            reloadIndex ();
        }

        reply->deleteLater ();
    }
    update_isFetching (false);
}

void BibleEngine::requestText (QString langId, QString bibleId) {
    QString key = QString ("%1__%2").arg (langId).arg (bibleId);
    qWarning () << "requestText" << langId << bibleId;
    QFile fileText (QString ("%1/%2.xml").arg (m_dataDir.path ()).arg (key));
    if (!fileText.exists ()) {
        BibleText * text = m_hashTexts.value (key.toLocal8Bit (), NULL);
        if (text) {
            text->update_isLoading (true);
        }

        QNetworkReply * reply = m_nam->get (QNetworkRequest (QUrl (QString ("%1/%2/%3.xml").arg (REPOS_BASEURL).arg (langId).arg (bibleId))));
        reply->setProperty ("key", key);
        connect (reply, &QNetworkReply::finished,         this, &BibleEngine::onTextRequestFinished);
        connect (reply, &QNetworkReply::downloadProgress, this, &BibleEngine::onTextRequestProgress);
    }
}

void BibleEngine::removeText (QString key) {
    qWarning () << "removeText" << key;
    QFile fileText (QString ("%1/%2.xml").arg (m_dataDir.path ()).arg (key));
    if (fileText.exists ()) {
        BibleText * text = m_hashTexts.value (key.toLocal8Bit (), NULL);
        if (text) {
            text->update_isLoading (false);
            text->update_hasLocal  (false);
        }

        fileText.remove ();
    }
}

void BibleEngine::loadText (QString key) {
    update_isLoading (true);
    m_settings->setValue ("currentTextKey", key);
    QTimer::singleShot (10, this, SLOT (doLoad ()));
}

void BibleEngine::doLoad () {
    QGuiApplication::processEvents ();
    m_hashVerses.clear ();
    QGuiApplication::processEvents ();
    m_hashChapters.clear ();
    QGuiApplication::processEvents ();
    m_hashBooks.clear ();
    QGuiApplication::processEvents ();
    m_modelBooks->clear ();
    QGuiApplication::processEvents ();
    QFile fileText (QString ("%1/%2.xml").arg (m_dataDir.path ()).arg (m_currentTextKey));
    if (fileText.exists ()) {
        fileText.open (QIODevice::ReadOnly | QIODevice::Text | QIODevice::Unbuffered);
        QGuiApplication::processEvents ();

        QString bookId = "";
        QString chapId = "";
        QString versId = "";
        QString txtVal = "";

        BibleBook    * book    = NULL;
        BibleChapter * chapter = NULL;
        BibleVerse   * verse   = NULL;

        QXmlStreamReader xml (&fileText);
        QGuiApplication::processEvents ();

        while (!xml.atEnd () && !xml.hasError ()) { /* We'll parse the XML until we reach end of it.*/
            /* Read next element.*/
            if (xml.readNext () == QXmlStreamReader::StartElement) {
                //qDebug () << "ELEMENT start";
                if (xml.name () == "div") {
                    bookId = xml.attributes ().value ("osisID").toString ();

                    book = new BibleBook;
                    book->update_bookId (bookId.toLocal8Bit ());
                    get_modelBooks ()->append (book);
                    m_hashBooks.insert (bookId.toLocal8Bit (), book);
                }
                else if (xml.name () == "chapter") {
                    chapId = xml.attributes ().value ("osisID").toString ();

                    chapter = new BibleChapter;
                    chapter->update_chapterId (chapId.toLocal8Bit ());
                    book->get_modelChapters ()->append (chapter);
                    m_hashChapters.insert (chapId.toLocal8Bit (), chapter);

                    QGuiApplication::processEvents ();
                }
                else if (xml.name () == "verse") {
                    versId = xml.attributes ().value ("osisID").toString ();
                    txtVal = xml.readElementText (QXmlStreamReader::SkipChildElements);

                    verse = new BibleVerse;
                    verse->update_verseId (versId.toLocal8Bit ());
                    verse->update_textContent (txtVal.replace ("¶ ", "").replace ("*", ""));
                    chapter->get_modelVerses ()->append (verse);
                    m_hashVerses.insert (versId.toLocal8Bit (), verse);

                    //qDebug () << versId << ":" << txtVal;
                }
            }
        }
        if (xml.hasError ()) { /* Error handling. */
            qWarning () << xml.errorString ();
        }

        xml.clear ();
        fileText.close ();
        QGuiApplication::processEvents ();
    }

    update_isLoading (false);
    QGuiApplication::processEvents ();

    setCurrent (m_settings->value ("currentId", QVariant ("")).toString ());

    //qDebug () << "books=" << m_hashBooks;
    //qDebug () << "chapters=" << m_hashChapters;
    //qDebug () << "verses=" << m_hashVerses;
}

void BibleEngine::onTextRequestProgress (qint64 bytesReceived, qint64 bytesTotal) {
    QNetworkReply * reply = qobject_cast<QNetworkReply *>(sender ());
    if (reply) {
        QString key = reply->property ("key").toString ();
        qDebug () << "onTextRequestProgress:" << "key=" << key;

        BibleText * text = m_hashTexts.value (key.toLocal8Bit (), NULL);
        if (text) {
            text->update_percent (qreal (bytesReceived) / qreal (bytesTotal));
        }
        qDebug () << "onTextRequestProgress:" << "text=" << text;

        qDebug () << "onTextRequestProgress:" << "bytesReceived=" << bytesReceived << "bytesTotal=" << bytesTotal;
    }
}

void BibleEngine::saveCurrentId (QString currentId) {
    m_settings->setValue ("currentId", currentId);
}

void BibleEngine::saveTextFontSize (qreal textFontSize) {
    m_settings->setValue ("textFontSize", textFontSize);
}

void BibleEngine::saveShowLocalOnly (bool showLocalOnly) {
    m_settings->setValue ("showLocalOnly", showLocalOnly);
}

void BibleEngine::onTextRequestFinished () {
    QNetworkReply * reply = qobject_cast<QNetworkReply *>(sender ());
    if (reply) {
        QString key = reply->property ("key").toString ();
        qDebug () << "onTextRequestFinished:" << "key=" << key;

        BibleText * text = m_hashTexts.value (key.toLocal8Bit (), NULL);
        if (reply->error () == QNetworkReply::NoError) {
            if (text) {
                text->update_isLoading (false);
                text->update_hasLocal  (true);
            }
            qDebug () << "onTextRequestFinished:" << "text=" << text;

            QByteArray data = reply->readAll ();
            //qDebug () << "onTextRequestFinished:" << "data=" << data;

            QFile fileText (QString ("%1/%2.xml").arg (m_dataDir.path ()).arg (key));
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
            }
        }

        reply->deleteLater ();
    }
}
