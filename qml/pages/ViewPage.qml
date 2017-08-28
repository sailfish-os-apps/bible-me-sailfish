import QtQuick 2.0;
import Sailfish.Silica 1.0;
import harbour.bibleme.myPrivateImports 1.0;

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
                title: formatReference (bibleEngine.currentVerseId);
            }
        }
        delegate: Component {
            ListItem {
                id: itemText;
                contentHeight: (lblVerse.contentHeight + lblVerse.anchors.margins * 2);
                menu: Component {
                    ContextMenu {
                        MenuItem {
                            text: $ (qsTr ("Copy text"));
                            onClicked: { Clipboard.text = model.textContent; }
                        }
                        MenuItem {
                            text: $ (qsTr ("Copy reference"));
                            onClicked: { Clipboard.text = formatReference (model.verseId); }
                        }
                        MenuItem {
                            text: $ (model.marked ? qsTr ("Remove from bookmarks") : qsTr ("Add to bookmarks"));
                            onClicked: {
                                if (model.marked) {
                                    bibleEngine.removeBookmark (model.verseId);
                                }
                                else {
                                    bibleEngine.addBookmark (model.verseId);
                                }
                            }
                        }
                    }
                }
                anchors {
                    left: (parent ? parent.left : undefined);
                    right: (parent ? parent.right : undefined);
                }
                onClicked: { bibleEngine.changePosition (model.verseId); }

                readonly property bool isCurrent : (bibleEngine.currentVerseId === model.verseId);

                Label {
                    id: lblVerse;
                    text: '<font color="%1">%2.</font> %3'.arg (Theme.secondaryHighlightColor.toString ()).arg (model.index +1).arg (model.textContent);
                    textFormat: Text.StyledText;
                    horizontalAlignment: Text.AlignJustify;
                    wrapMode: Text.WrapAtWordBoundaryOrAnywhere;
                    color: (isCurrent ? Theme.highlightColor : Theme.primaryColor);
                    font {
                        bold: model.marked;
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
