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
        SearchField {
            id: inputSearch;
            label: qsTr ("Search...");
            placeholderText: label;
            anchors {
                left: parent.left;
                right: parent.right;
                leftMargin: Theme.itemSizeMedium;
                rightMargin: Theme.paddingSizeMedium;
            }
            Keys.onReturnPressed: {
                if (text !== "") {
                    bibleEngine.searchInText (text);
                }
                else {
                    bibleEngine.modelSearchResults.clear ();
                }
                focus = false;
            }
        }
    }
    SilicaListView {
        id: view;
        clip: true;
        enabled: !busy.visible;
        opacity: (enabled ? 1.0 : 0.35);
        anchors.fill: parent;
        anchors.topMargin: header.height;
        model: (bibleEngine.modelSearchResults);
        delegate:Component {
            ListItem {
                id: itemVerse;
                contentHeight: (layoutText.height + layoutText.anchors.margins * 2);
                anchors {
                    left: parent.left;
                    right: parent.right;
                }
                onClicked: {
                    pageStack.navigateBack ();
                    bibleEngine.setCurrent (bibleVerse.verseId);
                }

                property BibleVerse bibleVerse : model.qtVariant;

                Column {
                    id: layoutText;
                    anchors {
                        top: parent.top;
                        left: parent.left;
                        right: parent.right;
                        margins: Theme.paddingMedium;
                    }

                    Label {
                        text: itemVerse.bibleVerse.verseId;
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
                        text: itemVerse.bibleVerse.textContent;
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
            text: (inputSearch.text !== "" ? qsTr ("No matching results !") : "");
            enabled: (!view.count);
        }
        VerticalScrollDecorator { }
    }
    BusyIndicator {
        id: busy;
        running: (bibleEngine.isSearching);
        visible: running;
        size: BusyIndicatorSize.Large;
        anchors.centerIn: parent;
    }
    Label {
        text: qsTr ("%1%").arg (bibleEngine.searchPercent);
        textFormat: Text.PlainText;
        visible: busy.visible;
        color: Theme.secondaryHighlightColor;
        font.pixelSize: Theme.fontSizeLarge;
        anchors.centerIn: parent;
    }
}
