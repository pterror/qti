import QtQuick

Window {
	color: "transparent"
	visible: true
	default property alias content: container.children
	property alias margins: container.anchors.margins

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
