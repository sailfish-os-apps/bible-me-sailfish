import QtQuick 2.0;
import Sailfish.Silica 1.0;
import harbour.bibleme.myPrivateImports 1.0;
import "../components";

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
            enabled: repeaterFavoritesGroups.count;
            anchors {
                left: parent.left;
                margins: Theme.paddingMedium;
                verticalCenter: parent.verticalCenter;
            }
            onClicked: {
                for (var idx = 0; idx < repeaterFavoritesGroups.count; ++idx) {
                    repeaterFavoritesGroups.itemAt (idx) ["expanded"] = true;
                }
            }
        }
        Button {
            id: btnCollapse;
            text: $ (qsTr ("Collapse all"));
            enabled: repeaterFavoritesGroups.count;
            anchors {
                right: parent.right;
                margins: Theme.paddingMedium;
                verticalCenter: parent.verticalCenter;
            }
            onClicked: {
                for (var idx = 0; idx < repeaterFavoritesGroups.count; ++idx) {
                    repeaterFavoritesGroups.itemAt (idx) ["expanded"] = false;
                }
            }
        }
    }
    SilicaFlickable {
        id: view;
        clip: true;
        contentHeight: layoutFavorites.height;
        anchors.fill: parent;
        anchors.topMargin: header.height;
        anchors.bottomMargin: footer.height;

        Column {
            id: layoutFavorites;
            anchors {
                top: parent.top;
                left: parent.left;
                right: parent.right;
            }

            Repeater {
                id: repeaterFavoritesGroups;
                model: bibleEngine.favoritesModel;
                delegate: GroupListItem {
                    bibleGroup: model.qtObject;
                }
            }
        }
        ViewPlaceholder {
            text: $ (qsTr ("No bookmark."));
            enabled: (bibleEngine.favoritesTotal === 0);
        }
        VerticalScrollDecorator { }
    }
}
