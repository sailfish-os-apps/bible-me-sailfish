
#include <QGuiApplication>
#include <QQuickView>
#include <QQmlEngine>
#include <QAbstractItemModel>
#include <QAbstractProxyModel>
#include <QSortFilterProxyModel>
#include <QNetworkProxy>
#include <QUrl>
#include <QLocale>
#include <QTranslator>

#include <qqml.h>
#include <sailfishapp.h>

#include <QQmlObjectListModel>
#include <QQmlVariantListModel>

#include "BibleEngine.h"
#include "BibleWorker.h"
#include "BibleText.h"
#include "BibleBook.h"
#include "BibleChapter.h"
#include "BibleVerse.h"

#define QML_MODULE "harbour.bibleme.myPrivateImports", 1, 0

int main (int argc, char * argv []) {
    QSettings::setDefaultFormat (QSettings::IniFormat);
    qmlRegisterUncreatableType<QAbstractItemModel>    (QML_MODULE, "AbstractItemModel",  "!!!");
    qmlRegisterUncreatableType<QAbstractListModel>    (QML_MODULE, "AbstractListModel",  "!!!");
    qmlRegisterUncreatableType<QQmlObjectListModel>   (QML_MODULE, "ObjectListModel",    "!!!");
    qmlRegisterUncreatableType<QQmlVariantListModel>  (QML_MODULE, "VariantListModel",   "!!!");
    qmlRegisterType<QSortFilterProxyModel>            (QML_MODULE, "SortFilterProxyModel");
    qmlRegisterType<BibleEngine>                      (QML_MODULE, "BibleEngine");
    qmlRegisterType<BibleText>                        (QML_MODULE, "BibleText");
    qmlRegisterType<BibleBook>                        (QML_MODULE, "BibleBook");
    qmlRegisterType<BibleChapter>                     (QML_MODULE, "BibleChapter");
    qmlRegisterType<BibleVerse>                       (QML_MODULE, "BibleVerse");
    QGuiApplication * app = SailfishApp::application (argc, argv);
    app->setApplicationName ("harbour-bibleme");
    QLocale locale;
    QString translationFilename = QString (":/lang/%1.qm").arg (locale.name ().split ('_').first ().toLower ());
    QFile file (translationFilename);
    QTranslator translator;
    translator.load (file.exists () ? translationFilename : ":/lang/en.qm");
    app->installTranslator (&translator);
    if (!qgetenv ("HTTP_PROXY").isEmpty ()) {
        QString proxyStr = QString::fromLocal8Bit (qgetenv ("HTTP_PROXY")).toLower ().remove ("http://");
        QNetworkProxy::setApplicationProxy (QNetworkProxy (QNetworkProxy::HttpProxy, proxyStr.split (':').first (), proxyStr.split (':').last ().toInt ()));
    }
    QQuickView * view = SailfishApp::createView ();
    view->setSource (QUrl ("qrc:/qml/harbour-bibleme.qml"));
    view->show ();
    return app->exec ();
}

