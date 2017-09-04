import QtQuick 2.0;
import Sailfish.Silica 1.0;
import harbour.bibleme.myPrivateImports 1.0;

Page {
    id: page;
    allowedOrientations: Orientation.All;

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
            enabled: repeaterLanguages.count;
            anchors {
                left: parent.left;
                margins: Theme.paddingMedium;
                verticalCenter: parent.verticalCenter;
            }
            onClicked: {
                for (var idx = 0; idx < repeaterLanguages.count; ++idx) {
                    repeaterLanguages.itemAt (idx) ["expanded"] = true;
                }
            }
        }
        Button {
            id: btnCollapse;
            text: $ (qsTr ("Collapse all"));
            enabled: repeaterLanguages.count;
            anchors {
                right: parent.right;
                margins: Theme.paddingMedium;
                verticalCenter: parent.verticalCenter;
            }
            onClicked: {
                for (var idx = 0; idx < repeaterLanguages.count; ++idx) {
                    repeaterLanguages.itemAt (idx) ["expanded"] = false;
                }
            }
        }
    }
    SilicaFlickable {
        id: view;
        clip: true;
        enabled: !busy.visible;
        opacity: (enabled ? 1.0 : 0.35);
        contentHeight: layout.height;
        anchors.fill: parent;
        anchors.bottomMargin: footer.height;

        Column {
            id: layout;
            anchors {
                top: parent.top;
                left: parent.left;
                right: parent.right;
            }

            Item {
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
                    title: $ (qsTr ("The Holy Bible versions"));
                }
            }
            Repeater {
                id: repeaterLanguages;
                model: bibleEngine.languagesModel;
                delegate: Column {
                    id: itemLanguage;
                    visible: (!bibleEngine.showLocalOnly || itemLanguage.bibleLanguage.hasLocal);
                    anchors {
                        left: parent.left;
                        right: parent.right;
                    }

                    readonly property BibleLanguage bibleLanguage : model ['qtObject'];

                    property bool expanded : false;

                    ListItem {
                        contentHeight: Theme.itemSizeSmall;
                        onClicked: { itemLanguage.expanded = !itemLanguage.expanded; }

                        Image {
                            id: ico;
                            source: "image://theme/icon-m-right?%1".arg (Theme.highlightColor);
                            rotation: (itemLanguage.expanded ? 90 : 0);
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
                            text: itemLanguage.bibleLanguage.languageTitle.split ("; ").join (" | ");
                            color: Theme.highlightColor;
                            elide: Text.ElideRight;
                            font.family: Theme.fontFamilyHeading;
                            font.pixelSize: Theme.fontSizeLarge;
                            anchors {
                                left: ico.right;
                                right: parent.right;
                                margins: Theme.paddingLarge;
                                verticalCenter: parent.verticalCenter;
                            }
                        }
                        GlassItem {
                            color: Theme.highlightColor;
                            visible: itemLanguage.bibleLanguage.hasLocal;
                            anchors {
                                verticalCenter: parent.verticalCenter;
                                horizontalCenter: parent.right;
                            }
                        }
                    }
                    Repeater {
                        id: subrepeater;
                        model: (itemLanguage.expanded && itemLanguage.visible ? itemLanguage.bibleLanguage.textsModel : 0);
                        delegate: ListItem {
                            id: itemText;
                            visible: (!bibleEngine.showLocalOnly || itemText.bibleText.hasLocal);
                            enabled: !bibleText.isLoading;
                            contentHeight: Theme.itemSizeSmall;
                            menu: ContextMenu {
                                MenuItem {
                                    text: (itemText.bibleText.hasLocal
                                           ? $ (qsTr ("Remove this version"))
                                           : $ (qsTr ("Download this version")));
                                    onClicked: {
                                        if (itemText.bibleText.hasLocal) {
                                            itemText.remove ();
                                        }
                                        else {
                                            bibleEngine.downloadText (itemLanguage.bibleLanguage.languageId,
                                                                      itemText.bibleText.bibleId);
                                        }
                                    }
                                }
                            }
                            anchors {
                                left: parent.left;
                                right: parent.right;
                            }
                            onClicked: {
                                if (bibleText.hasLocal) {
                                    pageStack.navigateBack ();
                                    bibleEngine.loadText (itemText.bibleText.textKey);
                                }
                            }
                            //ListView.onAdd: AddAnimation { target: itemText; }

                            readonly property BibleText bibleText : model ['qtObject'];

                            property bool dimmed : false;

                            function remove () {
                                itemText.dimmed = true;
                                remorse.execute (placeholder, $ (qsTr ("Deleting '%1'").arg (itemText.bibleText.bibleTitle)));
                            }

                            RemorseItem {
                                id: remorse;
                                onTriggered: {
                                    bibleEngine.removeText (itemText.bibleText.textKey);
                                    itemText.dimmed = false;
                                }
                                onCanceled: {
                                    itemText.dimmed = false;
                                }
                            }
                            GlassItem {
                                color: Theme.highlightColor;
                                visible: itemText.bibleText.hasLocal;
                                opacity: (itemText.dimmed ? 0.35 : 1.0);
                                anchors {
                                    verticalCenter: parent.verticalCenter;
                                    horizontalCenter: parent.right;
                                }
                            }
                            Label {
                                id: lblTitle;
                                text: itemText.bibleText.bibleTitle;
                                color: (itemText.bibleText.textKey === bibleEngine.currentTextKey
                                        ? Theme.highlightColor
                                        : (itemText.bibleText.hasLocal
                                           ? Theme.primaryColor
                                           : Theme.secondaryColor));
                                horizontalAlignment: Text.AlignLeft;
                                font.pixelSize: Theme.fontSizeMedium;
                                font.family: Theme.fontFamilyHeading;
                                elide: Text.ElideRight;
                                opacity: (itemText.dimmed ? 0.0 : 1.0);
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
                                       ? "(%1%)".arg (itemText.bibleText.percent)
                                       : "");
                                visible: (text !== "");
                                opacity: (itemText.dimmed ? 0.0 : 1.0);
                                color: Theme.secondaryColor;
                                font.pixelSize: Theme.fontSizeSmall;
                                font.family: Theme.fontFamilyHeading;
                                anchors {
                                    right: parent.right;
                                    margins: Theme.paddingLarge;
                                    verticalCenter: parent.verticalCenter;
                                }
                            }
                            Item {
                                id: placeholder;
                                anchors.fill: parent;
                            }
                        }
                    }
                }
            }
        }
        PullDownMenu {
            MenuItem {
                text: $ (qsTr ("Refresh list from Web..."));
                font.family: Theme.fontFamilyHeading;
                onClicked: { bibleEngine.showLocalOnly = false; bibleEngine.refreshIndex ();}
            }
            MenuItem {
                text: $ (qsTr ("Show only local : <b>%1</b>").arg (bibleEngine.showLocalOnly ? qsTr ("ON") : qsTr ("OFF")));
                font.family: Theme.fontFamilyHeading;
                onClicked: { bibleEngine.showLocalOnly = !bibleEngine.showLocalOnly; }
            }
        }
        ViewPlaceholder {
            text: $ (qsTr ("No Bible available. Try refresh !"));
            enabled: (!repeaterLanguages.count);
        }
        VerticalScrollDecorator { }
    }
    BusyIndicator {
        id: busy;
        running: bibleEngine.isRefreshing;
        visible: running;
        size: BusyIndicatorSize.Large;
        anchors.centerIn: parent;
    }
    Label {
        text: "%1%".arg (bibleEngine.refreshPercent);
        textFormat: Text.PlainText;
        visible: busy.visible;
        color: Theme.secondaryHighlightColor;
        font.pixelSize: Theme.fontSizeLarge;
        anchors.centerIn: parent;
    }
}
