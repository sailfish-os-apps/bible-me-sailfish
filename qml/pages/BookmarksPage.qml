import QtQuick 2.0;
import Sailfish.Silica 1.0;
import harbour.bibleme.myPrivateImports 1.0;

Page {
    id: page;
    allowedOrientations: Orientation.All;

    Item {
        id: header;
        height: Theme.itemSizeMedium;
        anchors {
            top: parent.top;
            left: parent.left;
            right: parent.right;
        }

        Rectangle {
            color: "white";
            opacity: 0.15;
            anchors.fill: parent;
        }
        PageHeader {
            title: $ (qsTr ("Bookmarks"));
        }
    }
    SilicaListView {
        id: view;
        clip: true;
        anchors.fill: parent;
        anchors.topMargin: header.height;
        model: bibleEngine.favoritesModel;
        delegate:Component {
            ListItem {
                id: itemVerse;
                contentHeight: (layoutText.height + layoutText.anchors.margins * 2);
                menu: Component {
                    ContextMenu {
                        MenuItem {
                            text: $ (qsTr ("Remove from bookmarks"));
                            onClicked: { remove (); }
                        }
                    }
                }
                anchors {
                    left: parent.left;
                    right: parent.right;
                }
                onClicked: {
                    pageStack.navigateBack ();
                    bibleEngine.changePosition (bibleVerse.verseId);
                }

                readonly property BibleVerse bibleVerse : model ['qtObject'];

                property bool dimmed : false;

                function remove () {
                    itemVerse.dimmed = true;
                    remorse.execute (placeholder, $ (qsTr ("Removing %1")).arg (formatReference (bibleVerse.verseId)));
                }

                RemorseItem {
                    id: remorse;
                    onTriggered: {
                        bibleEngine.removeBookmark (itemVerse.bibleVerse.verseId);
                        itemVerse.dimmed = false;
                    }
                    onCanceled: {
                        itemVerse.dimmed = false;
                    }
                }
                Column {
                    id: layoutText;
                    opacity: (itemVerse.dimmed ? 0.0 : 1.0);
                    anchors {
                        top: parent.top;
                        left: parent.left;
                        right: parent.right;
                        margins: Theme.paddingMedium;
                    }

                    Label {
                        text: formatReference (model.verseId);
                        color: Theme.primaryColor;
                        textFormat: Text.PlainText;
                        horizontalAlignment: Text.AlignLeft;
                        wrapMode: Text.WrapAtWordBoundaryOrAnywhere;
                        font {
                            family: Theme.fontFamilyHeading;
                            pixelSize: Theme.fontSizeMedium;
                        }
                        anchors {
                            left: parent.left;
                            right: parent.right;
                        }
                    }
                    Label {
                        text: model.textContent;
                        color: Theme.secondaryColor;
                        textFormat: Text.PlainText;
                        horizontalAlignment: Text.AlignJustify;
                        wrapMode: Text.WrapAtWordBoundaryOrAnywhere;
                        font {
                            family: Theme.fontFamilyHeading;
                            pixelSize: Theme.fontSizeTiny;
                        }
                        anchors {
                            left: parent.left;
                            right: parent.right;
                        }
                    }
                }
                Item {
                    id: placeholder;
                    anchors.fill: parent;
                }
            }
        }

        ViewPlaceholder {
            text: $ (qsTr ("No bookmark."));
            enabled: (!view.count);
        }
        VerticalScrollDecorator { }
    }
}
