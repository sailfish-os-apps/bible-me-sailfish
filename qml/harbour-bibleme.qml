import QtQuick 2.0;
import Sailfish.Silica 1.0;
import harbour.bibleme.myPrivateImports 1.0;
import "pages";
import "cover";

ApplicationWindow {
    id: appWin;
    cover: coverPage;
    initialPage: viewPage;

    function formatReference (refOsisId, onlyLast) {
        var ret = [];
        var tmp = String (refOsisId).split ('.');
        var book = (tmp [0]);
        var chap = (tmp [1]);
        var vers = (tmp [2]);
        if (book) {
            ret.push (booksFullname [book]);
            if (chap) {
                ret.push (qsTr ("chap. %1").arg (chap));
                if (vers) {
                    ret.push (qsTr ("v. %1").arg (vers));
                }
            }
        }
        return (onlyLast ? ret.pop () : ret.join (", "));
    }

    readonly property var booksFullname : { "1Chr" : qsTr ("1 Chronicles"),
                                            "1Cor" : qsTr ("1 Corinthians"),
                                            "1John" : qsTr ("1 John"),
                                            "1Kgs" : qsTr ("1 Kings"),
                                            "1Macc" : qsTr ("1 Maccabees"),
                                            "1Pet" : qsTr ("1 Peter"),
                                            "1Sam" : qsTr ("1 Samuel"),
                                            "1Thess" : qsTr ("1 Thessalonians"),
                                            "1Tim" : qsTr ("1 Timothy"),
                                            "2Chr" : qsTr ("2 Chronicles"),
                                            "2Cor" : qsTr ("2 Corinthians"),
                                            "2John" : qsTr ("2 John"),
                                            "2Kgs" : qsTr ("2 Kings"),
                                            "2Macc" : qsTr ("2 Maccabees"),
                                            "2Pet" : qsTr ("2 Peter"),
                                            "2Sam" : qsTr ("2 Samuel"),
                                            "2Thess" : qsTr ("2 Thessalonians"),
                                            "2Tim" : qsTr ("2 Timothy"),
                                            "3John" : qsTr ("3 John"),
                                            "Acts" : qsTr ("Acts"),
                                            "Amos" : qsTr ("Amos"),
                                            "Bar" : qsTr ("Baruch"),
                                            "Col" : qsTr ("Colossians"),
                                            "Dan" : qsTr ("Daniel"),
                                            "Deut" : qsTr ("Deuteronomy"),
                                            "Eccl" : qsTr ("Ecclesiastes"),
                                            "Eph" : qsTr ("Ephesians"),
                                            "Esth" : qsTr ("Esther"),
                                            "Exod" : qsTr ("Exodus"),
                                            "Ezek" : qsTr ("Ezekiel"),
                                            "Ezra" : qsTr ("Ezra"),
                                            "Gal" : qsTr ("Galatians"),
                                            "Gen" : qsTr ("Genesis"),
                                            "Hab" : qsTr ("Habakkuk"),
                                            "Hag" : qsTr ("Haggai"),
                                            "Heb" : qsTr ("Hebrews"),
                                            "Hos" : qsTr ("Hosea"),
                                            "Isa" : qsTr ("Isaiah"),
                                            "Jas" : qsTr ("James"),
                                            "Jer" : qsTr ("Jeremiah"),
                                            "Job" : qsTr ("Job"),
                                            "Joel" : qsTr ("Joel"),
                                            "John" : qsTr ("John"),
                                            "Jonah" : qsTr ("Jonah"),
                                            "Josh" : qsTr ("Joshua"),
                                            "Jude" : qsTr ("Jude"),
                                            "Judg" : qsTr ("Judges"),
                                            "Jdt" : qsTr ("Judith"),
                                            "Lam" : qsTr ("Lamentations"),
                                            "Lev" : qsTr ("Leviticus"),
                                            "Luke" : qsTr ("Luke"),
                                            "Mal" : qsTr ("Malachi"),
                                            "Mark" : qsTr ("Mark"),
                                            "Matt" : qsTr ("Matthew"),
                                            "Mic" : qsTr ("Micah"),
                                            "Nah" : qsTr ("Nahum"),
                                            "Neh" : qsTr ("Nehemiah"),
                                            "Num" : qsTr ("Numbers"),
                                            "Obad" : qsTr ("Obadiah"),
                                            "Phil" : qsTr ("Philippians"),
                                            "Phlm" : qsTr ("Philemon"),
                                            "Prov" : qsTr ("Proverbs"),
                                            "Ps" : qsTr ("Psalms"),
                                            "Rev" : qsTr ("Revelation"),
                                            "Rom" : qsTr ("Romans"),
                                            "Ruth" : qsTr ("Ruth"),
                                            "Sir" : qsTr ("Sirach"),
                                            "Song" : qsTr ("Song of Solomon"),
                                            "Titus" : qsTr ("Titus"),
                                            "Tob" : qsTr ("Tobit"),
                                            "Wis" : qsTr ("Wisdom"),
                                            "Zech" : qsTr ("Zechariah"),
                                            "Zeph" : qsTr ("Zephaniah"),
    };

    BibleEngine {
        id: bibleEngine;
    }
    Component {
        id: coverPage;

        CoverPage { }
    }
    Component {
        id: viewPage;

        ViewPage { }
    }
    Component {
        id: bookmarksPage;

        BookmarksPage { }
    }
    Component {
        id: searchPage;

        SearchPage { }
    }
    Component {
        id: selectPage;

        SelectPage { }
    }
    Component {
        id: filesPage;

        FilesPage { }
    }
    Component {
        id: settingsPage;

        SettingsPage { }
    }
}


