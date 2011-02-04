 import QtQuick 1.0

Item {
     id: screen; width: 200; height: 200
     property bool inListView : false

     ListModel {
         id: appModel
         ListElement { name: "Dr House"; icon: "house.jpg" }
         ListElement { name: "My Name Is Earl"; icon: "earl.jpg" }
         ListElement { name: "Grey's Anatomy"; icon: "greys.jpg" }
         ListElement { name: "Scrubs"; icon: "scrubs.jpg" }
         ListElement { name: "X-Files"; icon: "x-files.jpg" }
         ListElement { name: "Lost"; icon: "lost.jpg" }
         ListElement { name: "Dexter"; icon: "dexter.jpg" }
     }

     Component {
         id: appDelegate

         Item {
             width: 200; height: 200

             Image {
                 id: myIcon
                 y: 20;
		 anchors.horizontalCenter: parent.horizontalCenter
		 width: 100
		 height: 170
                 source: icon
             }
             Text {
                 anchors { top: myIcon.bottom; horizontalCenter: parent.horizontalCenter }
                 text: name
             }
         }
     }

     Component {
         id: appHighlight
         Rectangle { width: parent.cellWidth; height: parent.cellHeight; color: "lightsteelblue" }
     }

     Rectangle {
         id: background
         anchors.fill: parent; color: "#343434";

         Rectangle {
             id: taskbar
             x: 0
             y: parent.height - 48
             width: parent.width
             height: 48
             color: "#252323"
         }
     }

     GridView {
         id: contentView
         anchors.fill: parent
         cellWidth: 200
         cellHeight: 200
         highlight: appHighlight
         focus: true
         model: appModel
         delegate: appDelegate
     }
 }
