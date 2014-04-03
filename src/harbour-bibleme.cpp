
#include <QtQuick>
#include <QtQml>
#include <QAbstractItemModel>
#include <QAbstractProxyModel>
#include <QGuiApplication>
#include <QNetworkProxy>

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

int main (int argc, char * argv []){
    qmlRegisterUncreatableType<QAbstractItemModel>    (QML_MODULE, "AbstractItemModel",  "!!!");
    qmlRegisterUncreatableType<QAbstractListModel>    (QML_MODULE, "AbstractListModel",  "!!!");
    //qmlRegisterUncreatableType<QAbstractProxyModel>   (QML_MODULE, "AbstractProxyModel", "!!!");
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
    if (!qgetenv ("HTTP_PROXY").isEmpty ()) {
        QString proxyStr = QString::fromLocal8Bit (qgetenv ("HTTP_PROXY")).toLower ().remove ("http://");
        QNetworkProxy::setApplicationProxy (QNetworkProxy (QNetworkProxy::HttpProxy, proxyStr.split (':').first (), proxyStr.split (':').last ().toInt ()));
    }
    QQuickView * view = SailfishApp::createView ();
    view->setSource (QUrl ("qrc:/qml/harbour-bibleme.qml"));
    view->show ();
    return app->exec ();
}

