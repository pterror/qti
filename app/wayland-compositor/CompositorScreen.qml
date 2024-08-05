import QtQuick
import QtQuick.Window
import QtWayland.Compositor

WaylandOutput {
	id: output
	property string modifier: "Mod"
	property ListModel shellSurfaces: ListModel {}
	property bool isNestedCompositor: Qt.platform.pluginName.startsWith("wayland") || Qt.platform.pluginName === "xcb"
	function handleShellSurface(shellSurface) {
		shellSurfaces.append({ shellSurface });
	}
	sizeFollowsWindow: output.isNestedCompositor
	window: Window {
		width: 1024
		height: 760
		visible: true
		WaylandMouseTracker {
			id: mouseTracker
			anchors.fill: parent
			windowSystemCursorEnabled: output.isNestedCompositor
			Rectangle {
				id: background
				anchors.fill: parent
				Repeater {
					model: output.shellSurfaces
					Chrome {
						shellSurface: modelData
						onDestroyAnimationFinished: output.shellSurfaces.remove(index)
					}
				}
			}
			Loader {
				anchors.fill: parent
				source: "Keyboard.qml"
			}
			WaylandCursorItem {
				inputEventsEnabled: false
				x: mouseTracker.mouseX
				y: mouseTracker.mouseY
				seat: output.compositor.defaultSeat
			}
		}
		Shortcut {
			sequence: output.modifier + "+M"
			onActivated: Qt.quit()
		}
	}
}
