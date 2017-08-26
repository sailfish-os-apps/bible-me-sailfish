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
            title: $ (qsTr ("Settings"));
        }
    }
    ComboBox {
        id: combo;
        label: $ (qsTr ("Language"));
        currentIndex: {
            var ret = 0;
            for (var idx = 0; idx < codes.length; ++idx) {
                if (codes [idx] === bibleEngine.currentTranslationCode) {
                    ret = idx;
                    break;
                }
            }
            return ret;
        }
        menu: ContextMenu {
            MenuItem { text: "English";  }
            MenuItem { text: "Deutsch";  }
            MenuItem { text: "Français"; }
            MenuItem { text: "Suomi";    }
        }
        anchors {
            top: header.bottom;
            left: parent.left;
            right: parent.right;
        }
        onCurrentIndexChanged: { bibleEngine.translateUi (codes [currentIndex]); }

        readonly property var codes : ["en", "de", "fr", "fi"];
    }
    Slider {
        id: slider;
        label: $ (qsTr ("Text font-size"));
        minimumValue: Theme.fontSizeTiny;
        maximumValue: Theme.fontSizeExtraLarge;
        stepSize: 1.0;
        valueText: (value.toFixed ());
        anchors {
            top: combo.bottom;
            left: parent.left;
            right: parent.right;
            margins: Theme.paddingLarge;
        }
        onValueChanged: { bibleEngine.textFontSize = value; }

        Binding on value {
            value: bibleEngine.textFontSize;
            when: !slider.down;
        }
    }
    Label {
        text: $ (qsTr ("Sample text"));
        color: Theme.primaryColor;
        textFormat: Text.PlainText;
        font.pixelSize: slider.value;
        font.family: Theme.fontFamilyHeading;
        verticalAlignment: Text.AlignVCenter;
        horizontalAlignment: Text.AlignHCenter;
        anchors {
            top: slider.bottom;
            left: parent.left;
            right: parent.right;
            bottom: parent.bottom;
            margins: Theme.paddingLarge;
        }

        Rectangle {
            z: -1;
            color: "black";
            opacity: 0.15;
            anchors.fill: parent;
        }
    }
}
