import QtQuick
import QtWayland.Compositor

ShellSurfaceItem {
	id: chrome
	property bool isChild: parent.shellSurface !== undefined
	signal destroyAnimationFinished
	onSurfaceDestroyed: {
		bufferLocked = true;
		destroyAnimation.start();
	}
	SequentialAnimation {
		id: destroyAnimation
		ParallelAnimation {
			NumberAnimation { target: scaleTransform; property: "yScale"; to: 2/height; duration: 150 }
			NumberAnimation { target: scaleTransform; property: "xScale"; to: 0.4; duration: 150 }
			NumberAnimation { target: chrome; property: "opacity"; to: chrome.isChild ? 0 : 1; duration: 150 }
		}
		NumberAnimation { target: scaleTransform; property: "xScale"; to: 0; duration: 150 }
		ScriptAction { script: destroyAnimationFinished() }
	}
	transform: [
		Scale {
			id: scaleTransform
			origin.x: chrome.width / 2
			origin.y: chrome.height / 2
		}
	]
	Connections {
		target: shellSurface.toplevel !== undefined ? shellSurface.toplevel : null
		ignoreUnknownSignals: true
		function onActivatedChanged() { // xdg_shell only
			if (shellSurface.toplevel.activated) {
				receivedFocusAnimation.start()
			}
		}
	}
	SequentialAnimation {
		id: receivedFocusAnimation
		ParallelAnimation {
			NumberAnimation { target: scaleTransform; property: "yScale"; to: 1.02; duration: 100; easing.type: Easing.OutQuad }
			NumberAnimation { target: scaleTransform; property: "xScale"; to: 1.02; duration: 100; easing.type: Easing.OutQuad }
		}
		ParallelAnimation {
			NumberAnimation { target: scaleTransform; property: "yScale"; to: 1; duration: 100; easing.type: Easing.InOutQuad }
			NumberAnimation { target: scaleTransform; property: "xScale"; to: 1; duration: 100; easing.type: Easing.InOutQuad }
		}
	}
}