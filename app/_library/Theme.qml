pragma Singleton
import QtQuick

QtObject {
	id: root

	property string foregroundColor: "#a0ffffff"
	property string backgroundColor: "#00ffffff" // "#20ffffff"

	property QtObject window: QtObject {
		property int radius: 8
		property int margin: 8
		property string backgroundColor: root.backgroundColor
	}

	property QtObject button: QtObject {
		property string foregroundColor: root.foregroundColor
		property string backgroundColor: root.backgroundColor
		property string hoveredBackgroundColor: "#40ffffff"
		property string pressedBackgroundColor: "#30ffffff"
	}
}
