import QtQuick 2.0;
import Sailfish.Silica 1.0;
import harbour.bibleme.myPrivateImports 1.0;

Page {
    id: page;
    allowedOrientations: Orientation.All;

    SilicaListView {
        id: view;
        enabled: !busy.visible;
        opacity: (enabled ? 1.0 : 0.35);
        anchors.fill: parent;
        header: Item {
            id: header;
            height: Theme.itemSizeMedium;
            anchors {
                left: parent.left;
                right: parent.right;
            }

            Rectangle {
                color: "white";
                opacity: 0.15;
                anchors.fill: parent;
            }
            PageHeader {
                title: qsTr ("The Holy Bible versions");
            }
        }
        model: SortFilterProxyModel {
            dynamicSortFilter: true;
            filterKeyColumn: 0;
            filterRole: bibleEngine.modelTexts.roleForName ("hasLocal");
            filterRegExp: (bibleEngine.showLocalOnly ? /true/ : /[true|false]/);
            sourceModel: bibleEngine.modelTexts;
        }
        section {
            property: "languageTitle";
            criteria: ViewSection.FullString;
            delegate: Label {
                text: section.split ("; ").join (" | ");
                font.pixelSize: Theme.fontSizeExtraSmall;
                font.family: Theme.fontFamilyHeading;
                color: Theme.secondaryHighlightColor;
                height: contentHeight;
                horizontalAlignment: Text.AlignRight;
                anchors {
                    left: parent.left;
                    right: parent.right;
                    margins: Theme.paddingLarge;
                }
            }
        }
        delegate: Component {
            ListItem {
                id: itemText;
                enabled: !itemText.bibleText.isLoading;
                contentHeight: Theme.itemSizeSmall;
                menu: (itemText.bibleText.hasLocal ? menuLocal : menuRemote);
                anchors {
                    left: parent.left;
                    right: parent.right;
                }
                onClicked: {
                    if (itemText.bibleText.hasLocal) {
                        pageStack.navigateBack ();
                        bibleEngine.loadText ("%1__%2".arg (model.languageID).arg (model.bibleID));
                    }
                }
                ListView.onAdd: AddAnimation { target: itemText; }

                property BibleText bibleText : model ['qtObject'];
                property string    key       : "%1__%2".arg (bibleText.languageID).arg (bibleText.bibleID);

                function remove () {
                    var tmp = key;
                    remorse.execute (itemText,
                                     "Deleting",
                                     function () {
                                         bibleEngine.removeText (tmp);
                                     });
                }

                RemorseItem { id: remorse; }
                Component {
                    id: menuRemote;

                    ContextMenu {
                        MenuItem {
                            text: qsTr ("Download this version");
                            onClicked: {
                                bibleEngine.requestText (itemText.bibleText.languageID,
                                                         itemText.bibleText.bibleID);
                            }
                        }
                    }
                }
                Component {
                    id: menuLocal;

                    ContextMenu {
                        MenuItem {
                            text: qsTr ("Remove this version");
                            onClicked: { remove (); }
                        }
                    }
                }
                GlassItem {
                    color: Theme.highlightColor;
                    visible: itemText.bibleText.hasLocal;
                    anchors {
                        verticalCenter: parent.verticalCenter;
                        horizontalCenter: parent.right;
                    }
                }
                Label {
                    id: lblTitle;
                    text: itemText.bibleText.bibleTitle;
                    color: (itemText.key === bibleEngine.currentTextKey
                            ? Theme.highlightColor
                            : (itemText.bibleText.hasLocal
                               ? Theme.primaryColor
                               : Theme.secondaryColor));
                    horizontalAlignment: Text.AlignLeft;
                    font.pixelSize: Theme.fontSizeMedium;
                    font.family: Theme.fontFamilyHeading;
                    elide: Text.ElideRight;
                    anchors {
                        left: parent.left;
                        right: (lblStatus.visible ? lblStatus.left : parent.right);
                        margins: Theme.paddingLarge;
                        verticalCenter: parent.verticalCenter;
                    }
                }
                Label {
                    id: lblStatus;
                    text: (itemText.bibleText.isLoading
                           ? qsTr ("(%1%)").arg (itemText.bibleText.percent)
                           : "");
                    visible: (text !== "");
                    color: Theme.secondaryColor;
                    font.pixelSize: Theme.fontSizeSmall;
                    font.family: Theme.fontFamilyHeading;
                    anchors {
                        right: parent.right;
                        margins: Theme.paddingLarge;
                        verticalCenter: parent.verticalCenter;
                    }
                }
            }
        }

        PullDownMenu {
            MenuItem {
                text: qsTr ("Refresh list from Web...");
                font.family: Theme.fontFamilyHeading;
                enabled: bibleEngine.hasConnection;
                onClicked: { bibleEngine.requestIndex (); bibleEngine.showLocalOnly = false; }
            }
            MenuItem {
                text: qsTr ("Show only local : <b>%1</b>").arg (bibleEngine.showLocalOnly ? qsTr ("ON") : qsTr ("OFF"));
                font.family: Theme.fontFamilyHeading;
                onClicked: { bibleEngine.showLocalOnly = !bibleEngine.showLocalOnly; }
            }
        }
        ViewPlaceholder {
            text: qsTr ("No Bible available. Try refresh !");
            enabled: (!view.count);
        }
        VerticalScrollDecorator { }
    }
    BusyIndicator {
        id: busy;
        running: (bibleEngine.isFetching || bibleEngine.isReloading);
        visible: running;
        size: BusyIndicatorSize.Large;
        anchors.centerIn: parent;
    }
}
