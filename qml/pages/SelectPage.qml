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
            title: qsTr ("Select a verse");
        }
    }
    Row {
        clip: true;
        anchors.fill: parent;
        anchors.topMargin: header.height;

        property real itemWidth: (width / 3);

        SilicaListView {
            id: viewBooks;
            width: parent.itemWidth;
            anchors {
                top: parent.top;
                bottom: parent.bottom;
            }
            model: (bibleEngine.currentTextKey !== "" ? bibleEngine.modelBooks : 0);
            delegate:Component {
                ListItem {
                    contentHeight: Theme.itemSizeSmall;
                    anchors {
                        left: parent.left;
                        right: parent.right;
                    }
                    onClicked: { bibleEngine.setCurrent (model.bookId); }

                    Label {
                        text: model.bookId;
                        color: (bibleEngine.currentBookObj && model.bookId === bibleEngine.currentBookObj.bookId
                                ? Theme.highlightColor
                                : Theme.primaryColor);
                        horizontalAlignment: Text.AlignHCenter;
                        font.pixelSize: Theme.fontSizeMedium;
                        font.family: Theme.fontFamilyHeading;
                        elide: Text.ElideRight;
                        anchors {
                            left: parent.left;
                            right: parent.right;
                            margins: Theme.paddingLarge;
                            verticalCenter: parent.verticalCenter;
                        }
                    }
                }
            }

            VerticalScrollDecorator { }
        }
        SilicaListView {
            id: viewChapters;
            width: parent.itemWidth;
            anchors {
                top: parent.top;
                bottom: parent.bottom;
            }
            model: (bibleEngine.currentBookObj !== null ? bibleEngine.currentBookObj.modelChapters : 0);
            delegate:Component {
                ListItem {
                    contentHeight: Theme.itemSizeSmall;
                    anchors {
                        left: parent.left;
                        right: parent.right;
                    }
                    onClicked: { bibleEngine.setCurrent (model.chapterId); }

                    Label {
                        text: qsTr ("chap. %1").arg (String (model.chapterId).split ('.').pop ());
                        color: (bibleEngine.currentChapterObj && model.chapterId === bibleEngine.currentChapterObj.chapterId
                                ? Theme.highlightColor
                                : Theme.primaryColor);
                        horizontalAlignment: Text.AlignHCenter;
                        font.pixelSize: Theme.fontSizeMedium;
                        font.family: Theme.fontFamilyHeading;
                        elide: Text.ElideRight;
                        anchors {
                            left: parent.left;
                            right: parent.right;
                            margins: Theme.paddingLarge;
                            verticalCenter: parent.verticalCenter;
                        }
                    }
                }
            }

            VerticalScrollDecorator { }
        }
        SilicaListView {
            id: viewVerses;
            width: parent.itemWidth;
            anchors {
                top: parent.top;
                bottom: parent.bottom;
            }
            model: (bibleEngine.currentChapterObj !== null ? bibleEngine.currentChapterObj.modelVerses : 0);
            delegate:Component {
                ListItem {
                    contentHeight: Theme.itemSizeSmall;
                    anchors {
                        left: parent.left;
                        right: parent.right;
                    }
                    onClicked: { bibleEngine.setCurrent (model.verseId); }

                    Label {
                        text: qsTr ("v. %1").arg (String (model.verseId).split ('.').pop ());
                        color: (bibleEngine.currentVerseObj && model.verseId === bibleEngine.currentVerseObj.verseId
                                ? Theme.highlightColor
                                : Theme.primaryColor);
                        horizontalAlignment: Text.AlignHCenter;
                        font.pixelSize: Theme.fontSizeMedium;
                        font.family: Theme.fontFamilyHeading;
                        elide: Text.ElideRight;
                        anchors {
                            left: parent.left;
                            right: parent.right;
                            margins: Theme.paddingLarge;
                            verticalCenter: parent.verticalCenter;
                        }
                    }
                }
            }

            VerticalScrollDecorator { }
        }
    }

}
