import QtQuick 2.0;
import Sailfish.Silica 1.0;
import harbour.bibleme.myPrivateImports 1.0;
import "pages";
import "cover";

ApplicationWindow {
    id: appWin;
    cover: coverPage;
    initialPage: selectPage;
    Component.onCompleted: {
        pageStack.pushAttached (viewPage);
        pageStack.navigateForward (PageStackAction.Immediate);
    }

    function $ (str) {
        return (bibleEngine.currentTranslationCode !== "" ? str : str + "");
    }

    BibleEngine {
        id: bibleEngine;
    }
    Component {
        id: coverPage;

        CoverPage { }
    }
    Component {
        id: viewPage;

        ViewPage { }
    }
    Component {
        id: bookmarksPage;

        BookmarksPage { }
    }
    Component {
        id: searchPage;

        SearchPage { }
    }
    Component {
        id: selectPage;

        SelectPage { }
    }
    Component {
        id: filesPage;

        FilesPage { }
    }
    Component {
        id: settingsPage;

        SettingsPage { }
    }
}
