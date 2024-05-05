import QtQuick
import QtQuick.Layouts
import QtQuick.Shapes
import Qti.Core
import Qti.Screenshot
import Qti.Clipboard
import "../_library"

QtObject {
	id: realRoot
	property real closeOnCopyDate: 0

	property Connections copyDetection: Connections {
		target: Clipboard
		function onChanged(mode) {
			if (closeOnCopyDate === 0 || Number(new Date()) < closeOnCopyDate || mode !== Clipboard.Global) return
			Qt.quit()
		}
	}

	property QtiWindow window: QtiWindow {
		id: window
		margins: 0
		property url screenshotUrl: ""
		property string currentTool: "crop"
		property var fillColor: "transparent"
		property var strokeColor: "white"
		property int cropStartX: 0
		property int cropStartY: 0
		property int cropX: 0
		property int cropY: 0
		property int cropWidth: image.implicitWidth
		property int cropHeight: image.implicitHeight
		property int strokeWidth: 4
		property int toolSwitchCooldown: 50
		property real lastToolSwitch: 0
		property bool daemonizeOnCopy: true
		property var tools: ["pointer", "crop", "pen", "ellipse", "rectangle"]
		property var currentDecoration: undefined
		property list<var> redoStack: []
		Component.onCompleted: {
			Screenshot.capture(
				Qt.application.screens,
				result => { screenshotUrl = result },
				() => { Qt.quit() },
			)
			QtiCore.quitOnLastWindowClosed = false
		}

		Item {
			id: root
			anchors.fill: parent

			Shortcut { sequence: "Escape"; onActivated: Qt.quit() }

			Shortcut {
				sequence: "Ctrl+Z"
				onActivated: window.redoStack.push(decorationsContainer.children.pop())
			}

			Shortcut {
				sequence: "Ctrl+C"
				onActivated: {
					root.grabToImage(result => {
						let image = result.image
						if (
							window.cropX !== 0 || window.cropY !== 0 ||
							window.cropWidth !== image.implicitWidth || window.cropHeight !== image.implicitHeight
						) {
							const cropRect = Qt.rect(window.cropX, window.cropY, window.cropWidth, window.cropHeight)
							image = QtiCore.copyImage(image, cropRect)
						}
						Clipboard.setImage(image)
						Screenshot.free(window.screenshotUrl)
						if (window.daemonizeOnCopy) {
							closeOnCopyDate = Number(new Date()) + 50
							window.close()
							QtiCore.deleteLater(window)
						}
					})
				}
			}

			Component {
				id: ellipseComponent
				Shape {
					id: ellipse
					property int startX: 0
					property int startY: 0 
					property var color: ""
					property QtObject border: QtObject { property int width: 4; property var color: "" }
					preferredRendererType: Shape.CurveRenderer
					ShapePath {
						strokeWidth: ellipse.border.width
						strokeColor: ellipse.border.color
						fillColor: ellipse.color
						startX: ellipse.width / 2
						PathArc { radiusX: ellipse.width / 2; radiusY: ellipse.height / 2; relativeX: 0; relativeY: ellipse.height }
						PathArc { radiusX: ellipse.width / 2; radiusY: ellipse.height / 2; relativeX: 0; relativeY: -ellipse.height }
					}
				}
			}

			Component {
				id: rectangleComponent
				Rectangle { property int startX: 0; property int startY: 0 }
			}

			Image {
				id: image
				anchors.fill: parent
				fillMode: Image.PreserveAspectFit
				source: window.screenshotUrl
			}

			Item {
				id: decorationsContainer
				anchors.fill: parent
			}
		}

		Rectangle {
			color: Theme.maskColor
			width: window?.width ?? 0
			height: window?.cropY ?? 0
		}

		Rectangle {
			color: Theme.maskColor
			y: window?.cropY + window?.cropHeight
			width: window?.width ?? 0
			height: window?.height - window?.cropY - window?.cropHeight
		}

		Rectangle {
			color: Theme.maskColor
			y: window?.cropY ?? 0
			width: window?.cropX ?? 0
			height: window?.cropHeight
		}

		Rectangle {
			color: Theme.maskColor
			x: window?.cropX + window?.cropWidth
			y: window?.cropY ?? 0
			width: window?.width - window?.cropWidth
			height: window?.cropHeight ?? 0
		}

		MouseArea {
			id: mouseArea
			hoverEnabled: true
			anchors.fill: parent
			onWheel: event => {
				const epoch = Number(new Date())
				if (epoch - window.lastToolSwitch < window.toolSwitchCooldown) return
				window.lastToolSwitch = epoch
				const delta = event.angleDelta.x + event.angleDelta.y > 0 ? -1 : 1
				const newIndex = (window.tools.indexOf(window.currentTool) + delta + window.tools.length) % window.tools.length
				window.currentTool = window.tools[newIndex]
			}
			onPressed: event => {
				window.redoStack = []
				switch (window.currentTool) {
					case "crop": {
						window.cropX = event.x
						window.cropStartX = window.cropX
						window.cropY = event.y
						window.cropStartY = window.cropY
						window.cropWidth = 0
						window.cropHeight = 0
						break
					}
					case "ellipse":
					case "rectangle": {
						const props = {
							startX: event.x, startY: event.y, x: event.x, y: event.y, width: 1, height: 1,
							color: window.fillColor,
						}
						const component = window.currentTool === "ellipse" ? ellipseComponent : rectangleComponent
						window.currentDecoration = component.createObject(decorationsContainer, props)
						window.currentDecoration.border.color = window.strokeColor
						window.currentDecoration.border.width = window.strokeWidth
						break
					}
				}
			}
			onReleased: {
				window.currentDecoration = undefined
			}
			onPositionChanged: event => {
				if (!containsPress) return
				switch (window.currentTool) {
					case "crop": {
						window.cropX = Math.min(event.x, window.cropStartX)
						window.cropY = Math.min(event.y, window.cropStartY)
						window.cropWidth = Math.abs(event.x - window.cropStartX)
						window.cropHeight = Math.abs(event.y - window.cropStartY)
						break
					}
					case "ellipse":
					case "rectangle": {
						if (!window.currentDecoration) return
						window.currentDecoration.x = Math.min(event.x, window.currentDecoration.startX)
						window.currentDecoration.y = Math.min(event.y, window.currentDecoration.startY)
						window.currentDecoration.width = Math.abs(event.x - window.currentDecoration.startX)
						window.currentDecoration.height = Math.abs(event.y - window.currentDecoration.startY)
						break
					}
				}
			}
		}

		RowLayout {
			anchors.top: parent.top
			anchors.horizontalCenter: parent.horizontalCenter
			IconButton { icon.name: "pointer"; active: window.currentTool == "pointer"; onClicked: window.currentTool = "pointer" }
			IconButton { icon.name: "crop"; active: window.currentTool == "crop"; onClicked: window.currentTool = "crop" }
			IconButton { icon.name: "pen"; active: window.currentTool == "pen"; onClicked: window.currentTool = "pen" }
			IconButton { icon.name: "ellipse"; active: window.currentTool == "ellipse"; onClicked: window.currentTool = "ellipse" }
			IconButton { icon.name: "rectangle"; active: window.currentTool == "rectangle"; onClicked: window.currentTool = "rectangle" }
		}
	}
}
