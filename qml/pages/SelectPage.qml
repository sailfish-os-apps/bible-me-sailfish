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

        readonly property real itemWidth: ((width - 2) / 3);

        SilicaListView {
            id: viewBooks;
            width: parent.itemWidth;
            anchors {
                top: parent.top;
                bottom: parent.bottom;
            }
            model: bibleEngine.booksModel;
            delegate: ListItem {
                contentHeight: Theme.itemSizeSmall;
                anchors {
                    left: parent.left;
                    right: parent.right;
                }
                onClicked: { bibleEngine.changePosition (model.bookId + ".1.1"); }

                Label {
                    text: $ (bibleEngine.formatReference (model.bookId, true));
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
            delegate: ListItem {
                contentHeight: Theme.itemSizeSmall;
                anchors {
                    left: parent.left;
                    right: parent.right;
                }
                onClicked: { bibleEngine.changePosition (model.chapterId + ".1"); }

                Label {
                    text: $ (bibleEngine.formatReference (model.chapterId, true));
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
            model: bibleEngine.versesModel;
            delegate: ListItem {
                contentHeight: Theme.itemSizeSmall;
                anchors {
                    left: parent.left;
                    right: parent.right;
                }
                onClicked: {
                    bibleEngine.changePosition (model.verseId);
                    pageStack.navigateForward ();
                }

                Label {
                    text: $ (bibleEngine.formatReference (model.verseId, true));
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
            anchors {
                top: parent.top;
                bottom: parent.bottom;
            }

            VerticalScrollDecorator { }
        }
    }
}
