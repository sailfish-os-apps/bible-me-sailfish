
#include <QGuiApplication>
#include <QQuickView>
#include <QQmlEngine>
#include <QAbstractItemModel>
#include <QAbstractProxyModel>
#include <QUrl>
#include <QSettings>

#include <qqml.h>
#include <sailfishapp.h>

#include "QQmlObjectListModel.h"

#include "BibleEngine.h"
#include "BibleWorker.h"
#include "BibleLanguage.h"
#include "BibleText.h"
#include "BibleBook.h"
#include "BibleChapter.h"
#include "BibleVerse.h"
#include "BibleGroup.h"
#include "BibleScope.h"

#define QML_MODULE "harbour.bibleme.myPrivateImports", 1, 0

int main (int argc, char * argv []) {
    QSettings::setDefaultFormat (QSettings::IniFormat);
    qmlRegisterUncreatableType<QAbstractListModel>      (QML_MODULE, "AbstractListModel",  "!!!");
    qmlRegisterUncreatableType<QQmlObjectListModelBase> (QML_MODULE, "ObjectListModel",    "!!!");
    qmlRegisterUncreatableType<BibleScope>              (QML_MODULE, "BibleScope",         "!!!");
    qmlRegisterType<BibleEngine>                        (QML_MODULE, "BibleEngine");
    qmlRegisterType<BibleLanguage>                      (QML_MODULE, "BibleLanguage");
    qmlRegisterType<BibleText>                          (QML_MODULE, "BibleText");
    qmlRegisterType<BibleBook>                          (QML_MODULE, "BibleBook");
    qmlRegisterType<BibleChapter>                       (QML_MODULE, "BibleChapter");
    qmlRegisterType<BibleVerse>                         (QML_MODULE, "BibleVerse");
    qmlRegisterType<BibleGroup>                         (QML_MODULE, "BibleGroup");
    QGuiApplication * app = SailfishApp::application (argc, argv);
    app->setApplicationName ("harbour-bibleme");
    QQuickView * view = SailfishApp::createView ();
    view->setSource (QUrl ("qrc:/qml/harbour-bibleme.qml"));
    view->show ();
    return app->exec ();
}

