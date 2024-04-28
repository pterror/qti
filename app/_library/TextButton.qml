import QtQuick
import QtQuick.Controls

Button {
	id: root
	default property alias content: mouseArea.children
	flat: true
	display: AbstractButton.TextOnly
  palette.buttonText: Theme.button.foregroundColor
	background: Rectangle {
		anchors.margins: 4
		radius: 4
		color: root.pressed ? Theme.button.pressedBackgroundColor :
      root.hovered ? Theme.button.hoveredBackgroundColor :
      Theme.button.backgroundColor
	}

	MouseArea {
		id: mouseArea
		anchors.fill: parent
		anchors.margins: 0
		cursorShape: Qt.PointingHandCursor
		onClicked: root.clicked()
	}
}
