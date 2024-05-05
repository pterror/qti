import QtQuick
import QtQuick.Controls

Rectangle {
	id: root
	default property alias content: mouseArea.children
  signal clicked()
	property bool active: false
  palette.text: Theme.button.foregroundColor
  radius: Theme.button.radius
  color: mouseArea.containsPress ? Theme.button.pressedBackgroundColor :
    active || mouseArea.containsMouse ? Theme.button.hoveredBackgroundColor :
    Theme.button.backgroundColor
	scale: mouseArea.pressed ? Theme.button.pressedScale : 1
	Behavior on scale { SmoothedAnimation { velocity: Theme.button.pressedScaleSpeed } }

	MouseArea {
		id: mouseArea
		hoverEnabled: true
		anchors.fill: parent
		anchors.margins: Theme.button.margin
		cursorShape: Qt.PointingHandCursor
		onClicked: root.clicked()
	}
}
