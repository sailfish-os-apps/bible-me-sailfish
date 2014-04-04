import QtQuick 2.0;
import Sailfish.Silica 1.0;
import harbour.bibleme.myPrivateImports 1.0;

Page {
    id: page;
    allowedOrientations: Orientation.All;

    function repositionView () {
        //console.debug ("repositionView", bibleEngine.currentPositionId);
        var tmp = String (bibleEngine.currentPositionId).split ('.');
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
        onCurrentTextKeyChanged   : { repositionView (); }
        onCurrentPositionIdChanged: { repositionView (); }
        Component.onCompleted:      { repositionView (); }
    }
    SilicaListView {
        id: view;
        onCountChanged: { repositionView (); }
        enabled: !busy.visible;
        opacity: (enabled ? 1.0 : 0.35);
        anchors.fill: parent;
        model: bibleEngine.modelVerses;
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
                title: formatReference (bibleEngine.currentPositionId);
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
                onClicked: { bibleEngine.setCurrentVerse (model.verseId); }

                property bool isCurrent : (bibleEngine.currentPositionId === String (model.verseId));

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
                enabled: (currentBookName !== "" && currentChapterIdx > 0 && currentChapterIdx < bibleEngine.modelChapters.count ());
                onClicked: {
                    view.positionViewAtBeginning ();
                    bibleEngine.setCurrentVerse ("%1.%2.1".arg (currentBookName).arg (currentChapterIdx +1));
                }

                property string currentBookName   : (bibleEngine.currentPositionId.split ('.') [0] || "");
                property int    currentChapterIdx : (parseInt (bibleEngine.currentPositionId.split ('.') [1] || "0"));
            }
        }
        ViewPlaceholder {
            text: (bibleEngine.currentTextKey === ""
                   ? qsTr ("No text available. Use the menu to choose one")
                   : qsTr ("No verse select. Click on header to select"));
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
