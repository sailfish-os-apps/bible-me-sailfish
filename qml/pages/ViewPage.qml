import QtQuick 2.0;
import Sailfish.Silica 1.0;
import harbour.bibleme.myPrivateImports 1.0;

Page {
    id: page;
    allowedOrientations: Orientation.All;

    function repositionView () {
        console.warn ("repositionView", bibleEngine.currentChapterObj, bibleEngine.currentVerseObj);
        if (bibleEngine.currentChapterObj && bibleEngine.currentVerseObj) {
            for (var idx = 0; idx < bibleEngine.currentChapterObj.modelVerses.count (); idx++) {
                if (bibleEngine.currentChapterObj.modelVerses.get (idx) === bibleEngine.currentVerseObj) {
                    view.positionViewAtIndex (idx, ListView.Center);
                }
            }
        }
        else {
            view.positionViewAtBeginning ();
        }
    }

    Connections {
        target: bibleEngine;
        onCurrentBookObjChanged:    { repositionView (); }
        onCurrentChapterObjChanged: { repositionView (); }
        onCurrentVerseObjChanged:   { repositionView (); }
        Component.onCompleted:      { repositionView (); }
    }
    SilicaListView {
        id: view;
        enabled: !busy.visible;
        opacity: (enabled ? 1.0 : 0.35);
        anchors.fill: parent;
        model: (bibleEngine.currentChapterObj ? bibleEngine.currentChapterObj.modelVerses : 0);
        header: BackgroundItem {
            height: Theme.itemSizeMedium;
            anchors {
                left:  (parent ? parent.left  : undefined);
                right: (parent ? parent.right : undefined);
            }
            onClicked: { pageStack.push (selectPage); }

            Rectangle {
                color: "white";
                opacity: 0.15;
                anchors.fill: parent;
            }
            PageHeader {
                title: (bibleEngine.currentChapterObj ? String (bibleEngine.currentChapterObj.chapterId).split ('.').join (qsTr (", chap. ")) : "");
            }
        }
        delegate: Component {
            ListItem {
                id: itemText;
                contentHeight: (lblVerse.contentHeight + lblVerse.anchors.margins * 2);
                menu: Component {
                    ContextMenu {
                        MenuItem {
                            text: qsTr ("Copy text");
                            onClicked: { Clipboard.text = model.textContent; }
                        }
                        MenuItem {
                            text: qsTr ("Copy reference");
                            onClicked: { Clipboard.text = model.verseId; }
                        }
//                        MenuItem {
//                            text: qsTr ("Add to bookmarks");
//                            onClicked: {
//                                // TODO
//                            }
//                        }
                    }
                }
                anchors {
                    left: parent.left;
                    right: parent.right;
                }
                onClicked: { bibleEngine.setCurrent (model.verseId); }

                property bool isCurrent : (bibleEngine.currentVerseObj && model.verseId === bibleEngine.currentVerseObj.verseId);

                Label {
                    id: lblVerse;
                    text: '<font color="%1">%2.</font> %3'.arg (Theme.secondaryHighlightColor.toString ()).arg (model.index +1).arg (model.textContent);
                    textFormat: Text.StyledText;
                    horizontalAlignment: Text.AlignJustify;
                    wrapMode: Text.WrapAtWordBoundaryOrAnywhere;
                    color: (isCurrent ? Theme.highlightColor : Theme.primaryColor);
                    font {
                        family: Theme.fontFamilyHeading;
                        pixelSize: bibleEngine.textFontSize;
                    }
                    anchors {
                        top: parent.top;
                        left: parent.left;
                        right: parent.right;
                        margins: Theme.paddingMedium;
                    }
                }
            }
        }

        PullDownMenu {
            MenuItem {
                text: qsTr ("Switch Bible version...");
                font.family: Theme.fontFamilyHeading;
                onClicked: {
                    pageStack.pushAttached (filesPage);
                    pageStack.navigateForward ();
                }
            }
            MenuItem {
                text: qsTr ("Settings...");
                font.family: Theme.fontFamilyHeading;
                onClicked: {
                    pageStack.pushAttached (settingsPage);
                    pageStack.navigateForward ();
                }
            }
//            MenuItem {
//                text: qsTr ("My bookmarks...");
//                font.family: Theme.fontFamilyHeading;
//                onClicked: {
//                    pageStack.pushAttached (bookmarksPage);
//                    pageStack.navigateForward ();
//                }
//            }
            MenuItem {
                text: qsTr ("Search...");
                font.family: Theme.fontFamilyHeading;
                onClicked: {
                    pageStack.pushAttached (searchPage);
                    pageStack.navigateForward ();
                }
            }
        }
        PushUpMenu {
            MenuItem {
                text: qsTr ("Go to next chapter");
                font.family: Theme.fontFamilyHeading;
                enabled: (bibleEngine.currentBookObj &&
                          bibleEngine.currentChapterObj &&
                          bibleEngine.currentBookObj.modelChapters.last () !== bibleEngine.currentChapterObj);
                onClicked: {
                    var idx = bibleEngine.currentBookObj.modelChapters.indexOf (bibleEngine.currentChapterObj);
                    bibleEngine.setCurrent (bibleEngine.currentBookObj.modelChapters.get (idx +1) ['chapterId']);
                }
            }
        }
        ViewPlaceholder {
            text: qsTr ("No text available. Use the menu to choose one");
            enabled: (!view.count);
        }
        VerticalScrollDecorator { }
    }
    BusyIndicator {
        id: busy;
        running: (bibleEngine.isLoading);
        visible: running;
        size: BusyIndicatorSize.Large;
        anchors.centerIn: parent;
    }
}
