import QtQuick
import QtQuick.Controls

Button {
	id: root
	default property alias content: mouseArea.children
	property bool active: false
	flat: true
	display: AbstractButton.TextOnly
  palette.buttonText: Theme.button.foregroundColor
	background: Rectangle {
		anchors.margins: Theme.button.margin
		radius: Theme.button.radius
		color: mouseArea.containsPress ? Theme.button.pressedBackgroundColor :
			active || mouseArea.containsMouse ? Theme.button.hoveredBackgroundColor :
			Theme.button.backgroundColor
	}

	scale: mouseArea.pressed ? Theme.button.pressedScale : 1
	Behavior on scale { SmoothedAnimation { velocity: Theme.button.pressedScaleSpeed } }

	MouseArea {
		id: mouseArea
		hoverEnabled: true
		anchors.fill: parent
		anchors.margins: 0
		cursorShape: Qt.PointingHandCursor
		onClicked: root.clicked()
	}
}
