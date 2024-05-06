pragma Singleton
import QtQuick

QtObject {
	id: root

	property int radius: 8
	property int margin: 8
	property real iconOpacity: 0.75
	property int iconSize: 24
	// TODO: switch to palettes
	property string foregroundColor: "#a0ffffff"
	property string backgroundColor: "#00ffffff" // "#20ffffff"
	property string maskColor: "#80000000"

	property QtObject window: QtObject {
		property int radius: root.radius
		property int margin: root.margin
		property string backgroundColor: root.backgroundColor
	}

	property QtObject panel: QtObject {
		property int radius: root.radius
		property int margin: root.margin
		property string backgroundColor: root.backgroundColor
	}

	property QtObject button: QtObject {
		property int radius: root.radius / 2
		property int margin: root.margin / 2
		property real pressedScale: 0.9
		property real pressedScaleSpeed: 2
		property string foregroundColor: root.foregroundColor
		property string backgroundColor: root.backgroundColor
		property string hoveredBackgroundColor: "#40ffffff"
		property string pressedBackgroundColor: "#30ffffff"
	}
}
