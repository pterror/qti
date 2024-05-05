import QtQuick

Window {
	color: "transparent"
	visible: true
	default property alias content: container.children
	property alias margins: container.anchors.margins
	palette.text: Theme.foregroundColor
	palette.base: Theme.backgroundColor
	palette.windowText: Theme.foregroundColor
	palette.window: Theme.backgroundColor

	Rectangle {
		anchors.fill: parent
		color: Theme.window.backgroundColor
		radius: Theme.window.radius
		Rectangle {
			id: container
			color: "transparent"
			anchors.fill: parent
			anchors.margins: Theme.window.margin
		}
	}
}
