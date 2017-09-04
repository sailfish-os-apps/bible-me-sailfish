import QtQuick 2.0;
import Sailfish.Silica 1.0;
import harbour.bibleme.myPrivateImports 1.0;
import "../components";

Page {
    id: page;
    allowedOrientations: Orientation.All;

    readonly property string currentBookId     : (bibleEngine.currentVerseId.split ('.') [0] || "");
    readonly property int    currentChapterNum : (parseInt (bibleEngine.currentVerseId.split ('.') [1] || "0"));

    function repositionView () {
        var tmp = bibleEngine.currentVerseId.split ('.');
        if (tmp.length === 3) {
            var idx = (parseInt (tmp.pop ()) -1);
            if (idx < view.count) {
                view.positionViewAtIndex (idx, ListView.Center);
            }
        }
        else {
            view.positionViewAtBeginning ();
        }
    }

    Connections {
        target: bibleEngine;
        onCurrentVerseIdChanged: { repositionView (); }
        onCurrentTextKeyChanged: { repositionView (); }
        Component.onCompleted:   { repositionView (); }
    }
    SilicaListView {
        id: view;
        onCountChanged: { repositionView (); }
        enabled: !busy.visible;
        opacity: (enabled ? 1.0 : 0.35);
        anchors.fill: parent;
        model: bibleEngine.versesModel;
        header: Item {
            height: Theme.itemSizeMedium;
            anchors {
                left:  (parent ? parent.left  : undefined);
                right: (parent ? parent.right : undefined);
            }

            Rectangle {
                color: "white";
                opacity: 0.15;
                anchors.fill: parent;
            }
            PageHeader {
                title: $ (bibleEngine.formatReference (bibleEngine.currentVerseId));
            }
        }
        delegate: VerseListItem {
            bibleVerse: model.qtObject;
        }

        PullDownMenu {
            MenuItem {
                text: $ (qsTr ("Settings"));
                font.family: Theme.fontFamilyHeading;
                onClicked: {
                    pageStack.push (settingsPage);
                }
            }
            MenuItem {
                text: $ (qsTr ("Bible version"));
                font.family: Theme.fontFamilyHeading;
                onClicked: {
                    pageStack.pushAttached (filesPage);
                    pageStack.navigateForward ();
                }
            }
            MenuItem {
                text: $ (qsTr ("My bookmarks"));
                font.family: Theme.fontFamilyHeading;
                onClicked: {
                    pageStack.pushAttached (bookmarksPage);
                    pageStack.navigateForward ();
                }
            }
            MenuItem {
                text: $ (qsTr ("Search"));
                font.family: Theme.fontFamilyHeading;
                onClicked: {
                    pageStack.pushAttached (searchPage);
                    pageStack.navigateForward ();
                }
            }
            MenuItem {
                text: $ (qsTr ("Back to previous chapter"));
                font.family: Theme.fontFamilyHeading;
                enabled: (currentBookId !== "" && currentChapterNum > 1 && currentChapterNum <= bibleEngine.chaptersModel.count);
                onClicked: {
                    view.positionViewAtBeginning ();
                    bibleEngine.changePosition ("%1.%2.1".arg (currentBookId).arg (currentChapterNum -1));
                }
            }
        }
        PushUpMenu {
            MenuItem {
                text: $ (qsTr ("Go to next chapter"));
                font.family: Theme.fontFamilyHeading;
                enabled: (currentBookId !== "" && currentChapterNum > 0 && currentChapterNum < bibleEngine.chaptersModel.count);
                onClicked: {
                    view.positionViewAtBeginning ();
                    bibleEngine.changePosition ("%1.%2.1".arg (currentBookId).arg (currentChapterNum +1));
                }
            }
        }
        ViewPlaceholder {
            text: $ (bibleEngine.currentTextKey === ""
                     ? qsTr ("No text available. Use the menu to choose one")
                     : qsTr ("No verse selected. Swipe right-to-left"));
            enabled: (!view.count);
        }
        VerticalScrollDecorator { }
    }
    BusyIndicator {
        id: busy;
        running: bibleEngine.isLoading;
        visible: running;
        size: BusyIndicatorSize.Large;
        anchors.centerIn: parent;
    }
}
