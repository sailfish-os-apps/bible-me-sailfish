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
            title: $ (qsTr ("Select a verse"));
        }
    }
    Row {
        clip: true;
        anchors.fill: parent;
        anchors.topMargin: header.height;

        property real itemWidth: ((width - 2) / 3);

        SilicaListView {
            id: viewBooks;
            width: parent.itemWidth;
            anchors {
                top: parent.top;
                bottom: parent.bottom;
            }
            model: bibleEngine.booksModel;
            delegate:Component {
                ListItem {
                    contentHeight: Theme.itemSizeSmall;
                    anchors {
                        left: parent.left;
                        right: parent.right;
                    }
                    onClicked: { bibleEngine.loadBook (model.bookId); }

                    Label {
                        text: formatReference (model.bookId, true);
                        color: (bibleEngine.currentBookId === model.bookId
                                ? Theme.highlightColor
                                : Theme.primaryColor);
                        horizontalAlignment: Text.AlignHCenter;
                        font.pixelSize: Theme.fontSizeSmall;
                        font.family: Theme.fontFamilyHeading;
                        fontSizeMode: Text.HorizontalFit;
                        anchors {
                            left: parent.left;
                            right: parent.right;
                            margins: Theme.paddingSmall;
                            verticalCenter: parent.verticalCenter;
                        }
                    }
                }
            }

            VerticalScrollDecorator { }
        }
        Rectangle {
            color: "white";
            opacity: 0.15;
            width: 1;
            anchors {
                top: parent.top;
                bottom: parent.bottom;
            }
        }
        SilicaListView {
            id: viewChapters;
            width: parent.itemWidth;
            anchors {
                top: parent.top;
                bottom: parent.bottom;
            }
            model: bibleEngine.chaptersModel;
            delegate:Component {
                ListItem {
                    contentHeight: Theme.itemSizeSmall;
                    anchors {
                        left: parent.left;
                        right: parent.right;
                    }
                    onClicked: { bibleEngine.loadChapter (model.chapterId); }

                    Label {
                        text: formatReference (model.chapterId, true);
                        color: (bibleEngine.currentChapterId === model.chapterId
                                ? Theme.highlightColor
                                : Theme.primaryColor);
                        horizontalAlignment: Text.AlignHCenter;
                        font.pixelSize: Theme.fontSizeSmall;
                        font.family: Theme.fontFamilyHeading;
                        elide: Text.ElideRight;
                        anchors {
                            left: parent.left;
                            right: parent.right;
                            margins: Theme.paddingSmall;
                            verticalCenter: parent.verticalCenter;
                        }
                    }
                }
            }

            VerticalScrollDecorator { }
        }
        Rectangle {
            color: "white";
            opacity: 0.15;
            width: 1;
            anchors {
                top: parent.top;
                bottom: parent.bottom;
            }
        }
        SilicaListView {
            id: viewVerses;
            width: parent.itemWidth;
            anchors {
                top: parent.top;
                bottom: parent.bottom;
            }
            model: bibleEngine.versesModel;
            delegate:Component {
                ListItem {
                    contentHeight: Theme.itemSizeSmall;
                    anchors {
                        left: parent.left;
                        right: parent.right;
                    }
                    onClicked: {
                        bibleEngine.changePosition (model.verseId);
                        pageStack.navigateBack ();
                    }

                    Label {
                        text: formatReference (model.verseId, true);
                        color: (bibleEngine.currentVerseId === model.verseId
                                ? Theme.highlightColor
                                : Theme.primaryColor);
                        horizontalAlignment: Text.AlignHCenter;
                        font.pixelSize: Theme.fontSizeSmall;
                        font.family: Theme.fontFamilyHeading;
                        elide: Text.ElideRight;
                        anchors {
                            left: parent.left;
                            right: parent.right;
                            margins: Theme.paddingSmall;
                            verticalCenter: parent.verticalCenter;
                        }
                    }
                }
            }

            VerticalScrollDecorator { }
        }
    }

}
