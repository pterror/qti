import QtQuick
import QtQuick.VirtualKeyboard

InputPanel {
	visible: active
	y: active ? parent.height - height : parent.height
	anchors.left: parent.left
	anchors.right: parent.right
}