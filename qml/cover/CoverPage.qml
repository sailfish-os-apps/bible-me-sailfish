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
    Label {
        id: label;
        anchors.centerIn: parent
        text: (bibleEngine.currentVerseObj ? bibleEngine.currentVerseObj.verseId : "-");
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
    }
}


