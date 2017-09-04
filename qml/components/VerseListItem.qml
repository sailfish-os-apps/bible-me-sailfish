import QtQuick 2.0;
import Sailfish.Silica 1.0;
import harbour.bibleme.myPrivateImports 1.0;

ListItem {
    id: base;
    contentHeight: (layoutVerse.height + layoutVerse.anchors.margins * 2);
    menu: ContextMenu {
        MenuItem {
            text: $ (qsTr ("Copy text"));
            onClicked: { Clipboard.text = bibleVerse.textContent; }
        }
        MenuItem {
            text: $ (qsTr ("Copy reference"));
            onClicked: { Clipboard.text = $ (bibleEngine.formatReference (bibleVerse.verseId)); }
        }
        MenuItem {
            text: (bibleVerse ? (bibleVerse.marked ? $ (qsTr ("Remove from bookmarks")) : $ (qsTr ("Add to bookmarks"))) : "");
            onClicked: {
                if (bibleVerse.marked) {
                    remove ();
                }
                else {
                    bibleEngine.addBookmark (bibleVerse.verseId);
                }
            }
        }
    }
    anchors {
        left: (parent ? parent.left : undefined);
        right: (parent ? parent.right : undefined);
    }
    onClicked: {
        bibleEngine.changePosition (bibleVerse.verseId);
        if (secondaryView) {
            pageStack.navigateBack ();
        }
    }

    property bool dimmed        : false;
    property bool secondaryView : false;

    property BibleVerse bibleVerse : null;

    readonly property bool isCurrent : (bibleVerse && bibleVerse.verseId === bibleEngine.currentVerseId);

    function remove () {
        dimmed = true;
        remorse.execute (placeholder, $ (qsTr ("Removing %1")).arg (bibleEngine.formatReference (bibleVerse.verseId)));
    }

    RemorseItem {
        id: remorse;
        onTriggered: {
            bibleEngine.removeBookmark (bibleVerse.verseId);
            dimmed = false;
        }
        onCanceled: {
            dimmed = false;
        }
    }
    GlassItem {
        color: Theme.highlightColor;
        visible: (secondaryView && isCurrent);
        anchors {
            verticalCenter: parent.verticalCenter;
            horizontalCenter: parent.left;
        }
    }
    Column {
        id: layoutVerse;
        opacity: (dimmed ? 0.0 : 1.0);
        anchors {
            left: parent.left;
            right: parent.right;
            margins: Theme.paddingSmall;
            leftMargin: (secondaryView ? Theme.iconSizeMedium + Theme.paddingMedium * 2 : Theme.paddingSmall);
            verticalCenter: parent.verticalCenter;
        }

        Label {
            text: (bibleVerse ? $ (bibleEngine.formatReference (bibleVerse.verseId, false)) + " :" : "");
            color: Theme.highlightColor;
            visible: secondaryView;
            wrapMode: Text.WrapAtWordBoundaryOrAnywhere;
            textFormat: Text.PlainText;
            horizontalAlignment: Text.AlignLeft;
            font.family: Theme.fontFamilyHeading;
            anchors {
                left: parent.left;
                right: parent.right;
            }
        }
        Label {
            text: (bibleVerse
                   ? (secondaryView
                      ? bibleVerse.textContent
                      : '<font color="%1">%2.</font> %3'.arg (Theme.secondaryHighlightColor.toString ()).arg (model.index +1).arg (model.textContent))
                   : "");
            color: (secondaryView ? Theme.secondaryColor : (isCurrent ? Theme.highlightColor : Theme.primaryColor));
            wrapMode: Text.WrapAtWordBoundaryOrAnywhere;
            textFormat: (secondaryView ? Text.PlainText : Text.StyledText);
            horizontalAlignment: Text.AlignJustify;
            font {
                bold: (bibleVerse && bibleVerse.marked);
                family: Theme.fontFamilyHeading;
                pixelSize: (bibleEngine.textFontSize || Style.fontSizeNormal);
            }
            anchors {
                left: parent.left;
                right: parent.right;
            }
        }
    }
    Item {
        id: placeholder;
        anchors.fill: parent;
    }
}
