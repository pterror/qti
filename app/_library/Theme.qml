pragma Singleton
import QtQuick

QtObject {
	id: root

	property real iconOpacity: 0.75
	property int iconSize: 24
	property string foregroundColor: "#a0ffffff"
	property string backgroundColor: "#00ffffff" // "#20ffffff"
	property string maskColor: "#80000000"

	property QtObject window: QtObject {
		property int radius: 8
		property int margin: 8
		property string backgroundColor: root.backgroundColor
	}

	property QtObject button: QtObject {
		property int radius: 4
		property int margin: 4
		property real pressedScale: 0.9
		property real pressedScaleSpeed: 0.9
		property string foregroundColor: root.foregroundColor
		property string backgroundColor: root.backgroundColor
		property string hoveredBackgroundColor: "#40ffffff"
		property string pressedBackgroundColor: "#30ffffff"
	}
}
