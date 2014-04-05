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
            title: qsTr ("Bookmarks");
        }
    }
    SilicaListView {
        id: view;
        clip: true;
        anchors.fill: parent;
        anchors.topMargin: header.height;
        model: bibleEngine.modelBookmarks;
        delegate:Component {
            ListItem {
                id: itemVerse;
                contentHeight: (layoutText.height + layoutText.anchors.margins * 2);
                menu: Component {
                    ContextMenu {
                        MenuItem {
                            text: qsTr ("Remove from bookmarks");
                            onClicked: { bibleEngine.removeBookmark (model.verseId); }
                        }
                    }
                }
                anchors {
                    left: parent.left;
                    right: parent.right;
                }
                onClicked: {
                    pageStack.navigateBack ();
                    bibleEngine.setCurrentVerse (model.verseId);
                }

                Column {
                    id: layoutText;
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
            }
        }

        ViewPlaceholder {
            text: qsTr ("No bookmark.");
            enabled: (!view.count);
        }
        VerticalScrollDecorator { }
    }
}
