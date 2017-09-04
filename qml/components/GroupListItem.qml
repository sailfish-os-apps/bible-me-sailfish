import QtQuick 2.0;
import Sailfish.Silica 1.0;
import harbour.bibleme.myPrivateImports 1.0;

Column {
    id: base;
    visible: (bibleGroup && bibleGroup.entriesModel.count);
    anchors {
        left: (parent ? parent.left : undefined);
        right: (parent ? parent.right : undefined);
    }

    property BibleGroup bibleGroup : null;

    property bool expanded : false;

    ListItem {
        contentHeight: (lblGroupTitle.height + lblGroupTitle.anchors.margins * 2);
        anchors {
            left: parent.left;
            right: parent.right;
        }
        onClicked: { expanded = !expanded; }

        Image {
            id: ico;
            source: "image://theme/icon-m-right?%1".arg (Theme.highlightColor);
            rotation: (expanded ? 90 : 0);
            anchors {
                left: parent.left;
                margins: Theme.paddingMedium;
                verticalCenter: parent.verticalCenter;
            }

            Behavior on rotation {
                NumberAnimation {
                    duration: 350;
                }
            }
        }
        Label {
            id: lblGroupTitle;
            text: (bibleGroup ? $ (bibleEngine.formatReference (bibleGroup.bookId)) : "");
            font.pixelSize: Theme.fontSizeLarge;
            font.family: Theme.fontFamilyHeading;
            anchors {
                left: ico.right;
                right: lblGroupCounter.left;
                margins: Theme.paddingMedium;
                verticalCenter: parent.verticalCenter;
            }
        }
        Label {
            id: lblGroupCounter;
            text: (bibleGroup ? bibleGroup.entriesModel.count : "-");
            color: Theme.highlightColor;
            font.bold: true;
            font.family: Theme.fontFamilyHeading;
            anchors {
                right: parent.right;
                margins: Theme.paddingMedium;
                verticalCenter: parent.verticalCenter;
            }

            Rectangle {
                z: -1;
                color: Theme.secondaryColor;
                radius: Math.min (width, height) * 0.5;
                opacity: 0.35;
                antialiasing: true;
                anchors {
                    fill: parent;
                    margins: -Theme.paddingSmall;
                }
            }
        }
    }
    Column {
        visible: expanded;
        anchors {
            left: parent.left;
            right: parent.right;
        }

        Repeater {
            id: repeaterGroupEntries;
            model: (base.visible && expanded && bibleGroup ? bibleGroup.entriesModel : 0);
            delegate: VerseListItem {
                bibleVerse: model.qtObject;
                secondaryView: true;
            }
        }
    }
}
