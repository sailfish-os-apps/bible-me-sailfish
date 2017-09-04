import QtQuick 2.0;
import Sailfish.Silica 1.0;
import harbour.bibleme.myPrivateImports 1.0;
import "../components";

Page {
    id: page;
    allowedOrientations: Orientation.All;

    Item {
        id: header;
        height: (colHeader.height + colHeader.anchors.margins * 2);
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
        Column {
            id: colHeader;
            enabled: !bibleEngine.isSearching;
            anchors {
                left: parent.left;
                right: parent.right;
                verticalCenter: parent.verticalCenter;
            }

            SearchField {
                id: inputSearch;
                label: $ (qsTr ("Search..."));
                placeholderText: label;
                anchors {
                    left: parent.left;
                    right: parent.right;
                    margins: Theme.paddingMedium;
                }
                Keys.onReturnPressed: {
                    if (text !== "") {
                        bibleEngine.searchContent (text, combo.scopes [combo.currentIndex]);
                    }
                    else {
                        bibleEngine.resultsModel.clear ();
                    }
                    focus = false;
                }
            }
            ComboBox {
                id: combo;
                label: $ (qsTr ("Scope :"));
                currentIndex: 0;
                menu: ContextMenu {
                    MenuItem { text: $ (qsTr ("Full Bible"));       }
                    MenuItem { text: $ (qsTr ("Current book"));     }
                    MenuItem { text: $ (qsTr ("Current chapter"));  }
                }
                anchors {
                    left: parent.left;
                    right: parent.right;
                }

                readonly property var scopes : [BibleScope.FULL_BIBLE, BibleScope.CURRENT_BOOK, BibleScope.CURRENT_CHAPTER];
            }
        }
    }
    Item {
        id: footer;
        height: Math.max ((btnExpand.height   + btnExpand.anchors.margins * 2),
                          (btnCollapse.height + btnCollapse.anchors.margins * 2));
        anchors {
            left: parent.left;
            right: parent.right;
            bottom: parent.bottom;
        }

        Rectangle {
            color: "white";
            opacity: 0.15;
            anchors.fill: parent;
        }
        Button {
            id: btnExpand;
            text: $ (qsTr ("Expand all"));
            enabled: repeaterResultGroups.count;
            anchors {
                left: parent.left;
                margins: Theme.paddingMedium;
                verticalCenter: parent.verticalCenter;
            }
            onClicked: {
                for (var idx = 0; idx < repeaterResultGroups.count; ++idx) {
                    repeaterResultGroups.itemAt (idx) ["expanded"] = true;
                }
            }
        }
        Button {
            id: btnCollapse;
            text: $ (qsTr ("Collapse all"));
            enabled: repeaterResultGroups.count;
            anchors {
                right: parent.right;
                margins: Theme.paddingMedium;
                verticalCenter: parent.verticalCenter;
            }
            onClicked: {
                for (var idx = 0; idx < repeaterResultGroups.count; ++idx) {
                    repeaterResultGroups.itemAt (idx) ["expanded"] = false;
                }
            }
        }
    }
    SilicaFlickable {
        id: view;
        clip: true;
        enabled: !busy.visible;
        opacity: (enabled ? 1.0 : 0.35);
        contentHeight: layoutSearch.height;
        anchors.fill: parent;
        anchors.topMargin: header.height;
        anchors.bottomMargin: footer.height;

        Column {
            id: layoutSearch;
            anchors {
                top: parent.top;
                left: parent.left;
                right: parent.right;
            }

            Repeater {
                id: repeaterResultGroups;
                model: bibleEngine.resultsModel;
                delegate: GroupListItem {
                    bibleGroup: model.qtObject;
                }
            }
        }
        ViewPlaceholder {
            text: $ (qsTr ("No matching results !"));
            enabled: (inputSearch.text !== "" && bibleEngine.resultsTotal === 0);
        }
        VerticalScrollDecorator { }
    }
    BusyIndicator {
        id: busy;
        running: bibleEngine.isSearching;
        visible: running;
        size: BusyIndicatorSize.Large;
        anchors.centerIn: parent;
    }
    Label {
        text: "%1%".arg (bibleEngine.searchPercent);
        textFormat: Text.PlainText;
        visible: busy.visible;
        color: Theme.secondaryHighlightColor;
        font.pixelSize: Theme.fontSizeLarge;
        anchors.centerIn: parent;
    }
}
