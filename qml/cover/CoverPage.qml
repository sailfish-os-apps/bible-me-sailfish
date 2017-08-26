import QtQuick 2.0;
import Sailfish.Silica 1.0;
import harbour.bibleme.myPrivateImports 1.0;

CoverBackground {

    Item {
        opacity: 0.15;
        rotation: +10;
        anchors.centerIn: parent;

        Rectangle {
            width: 25;
            height: (width + 50 * 3);
            radius: 3;
            antialiasing: true;
            color: "white";
            anchors.centerIn: parent;
        }
        Rectangle {
            width: (height + 50 * 2);
            height: 25.
            radius: 3;
            antialiasing: true;
            color: "white";
            anchors.centerIn: parent;
            anchors.verticalCenterOffset: -50;
        }
    }
    Column {
        anchors {
            top: parent.top;
            left: parent.left;
            right: parent.right;
            margins: Theme.paddingMedium;
        }

        Repeater {
            model: formatReference (bibleEngine.currentVerseId).split (",");
            delegate: Label {
                text: model.modelData.trim ();
                textFormat: Text.PlainText;
                fontSizeMode: Text.HorizontalFit;
                height: contentHeight;
                minimumPixelSize: 10;
                horizontalAlignment: Text.AlignHCenter;
                font {
                    pixelSize: Theme.fontSizeExtraLarge;
                    family: Theme.fontFamilyHeading;
                }
                anchors {
                    left: (parent ? parent.left : undefined);
                    right: (parent ? parent.right : undefined);
                }
            }
        }
    }
    CoverActionList {
        CoverAction {
            iconSource: "image://theme/icon-cover-search";
            onTriggered: {
                pageStack.navigateBack (PageStackAction.Immediate);
                pageStack.pushAttached (searchPage);
                pageStack.navigateForward (PageStackAction.Immediate);
                activate ();
            }
        }
        CoverAction {
            iconSource: "image://theme/icon-cover-location";
            onTriggered: {
                pageStack.navigateBack (PageStackAction.Immediate);
                pageStack.push (selectPage);
                activate ();
            }
        }
    }
}


