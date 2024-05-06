import QtQuick
import QtQuick.Layouts
import QtQuick.Shapes
import Qti.Core
import Qti.Screenshot
import Qti.Clipboard
import "../_library"

// TODO: show decorations in a list
// TODO: actually implement eraser
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
		property var fillColor: "#00000000"
		property var strokeColor: "#80ffffff"
		property int cropStartX: 0
		property int cropStartY: 0
		property int cropX: 0
		property int cropY: 0
		property int cropWidth: window.width
		property int cropHeight: window.height
		property int strokeWidth: 4
		property int wheelEventCooldown: 50
		property real lastWheelEvent: 0
		property bool daemonizeOnCopy: true
		property var tools: ["pointer", "crop", "eraser", "pen", "ellipse", "rectangle"]
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

		function switchTool(tool) {
			colorPicker.owner = undefined
			window.currentTool = tool
		}

		MouseArea {
			id: mouseArea
			hoverEnabled: true
			anchors.fill: parent
			onWheel: event => {
				const epoch = Number(new Date())
				if (epoch - window.lastWheelEvent < window.wheelEventCooldown) return
				window.lastWheelEvent = epoch
				const delta = event.angleDelta.x + event.angleDelta.y > 0 ? -1 : 1
				if (event.modifiers & Qt.ControlModifier) {
					const newIndex = (window.tools.indexOf(window.currentTool) + delta + window.tools.length) % window.tools.length
					window.switchTool(window.tools[newIndex])
				} else {
					window.strokeWidth = Math.max(0, window.strokeWidth - delta)
				}
			}
			onPressed: event => {
				colorPicker.owner = undefined
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
					case "pen":
					case "ellipse":
					case "rectangle": {
						const props = Object.assign({
							startX: event.x, startY: event.y,
							color: window.fillColor,
						}, window.currentTool === "pen" ? {} : { x: event.x, y: event.y, width: 1, height: 1 })
						const component = {
							pen: penComponent,
							ellipse: ellipseComponent,
							rectangle: rectangleComponent,
						}[window.currentTool]
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
					case "pen": {
						if (!window.currentDecoration) return
						window.currentDecoration.lineTo(event.x, event.y)
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

		Item {
			id: root
			anchors.fill: parent

			Shortcut {
				sequence: "Escape"
				onActivated: {
					if (colorPicker.owner !== undefined) {
						colorPicker.owner = undefined
					} else {
						Qt.quit()
					}
				}
			}

			Shortcut {
				sequence: "Ctrl+Z"
				onActivated: window.redoStack.push(decorationsContainer.children.pop())
			}

			Shortcut {
				sequence: "Ctrl+Shift+Z"
				onActivated: {
					if (window.redoStack.length === 0) return
					decorationsContainer.children.push(window.redoStack.pop())
				}
			}

			Shortcut {
				sequence: "Ctrl+Y"
				onActivated: {
					if (window.redoStack.length === 0) return
					decorationsContainer.children.push(window.redoStack.pop())
				}
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

			Component { id: pathLineComponent; PathLine {} }

			Component {
				id: penComponent
				Shape {
					id: shape
					property int startX: 0
					property int startY: 0 
					property var color: ""
					property QtObject border: QtObject { property int width: 4; property var color: "" }
					preferredRendererType: Shape.CurveRenderer

					ShapePath {
						id: path
						strokeWidth: shape.border.width
						strokeColor: shape.border.color
						fillColor: shape.color
					}

					MouseArea {
						enabled: window.currentTool === "eraser" && mouseArea.containsPress
						hoverEnabled: true
						anchors.fill: parent
						onPositionChanged: {
							decorationsContainer.children = decorationsContainer.children.filter(c => c !== parent)
						}
					}

					function lineTo(x, y) {
						if (path.startX === 0 && path.startY === 0) {
							path.startX = x
							path.startY = y
						} else {
							path.pathElements.push(pathLineComponent.createObject(path, { x, y }))
						}
					}
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

					MouseArea {
						enabled: window.currentTool === "eraser" && mouseArea.containsPress
						hoverEnabled: true
						anchors.fill: parent
						onPositionChanged: {
							decorationsContainer.children = decorationsContainer.children.filter(c => c !== parent)
						}
					}
				}
			}

			Component {
				id: rectangleComponent

				Rectangle {
					property int startX: 0; property int startY: 0

					MouseArea {
						enabled: window.currentTool === "eraser" && mouseArea.containsPress
						hoverEnabled: true
						anchors.fill: parent
						onPositionChanged: {
							decorationsContainer.children = decorationsContainer.children.filter(c => c !== parent)
						}
					}
				}
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

		RowLayout {
			anchors.top: parent.top
			anchors.horizontalCenter: parent.horizontalCenter
			IconButton { icon.name: "pointer"; active: window.currentTool == "pointer"; onClicked: window.switchTool("pointer") }
			IconButton { icon.name: "crop"; active: window.currentTool == "crop"; onClicked: window.switchTool("crop") }
			IconButton { icon.name: "eraser"; active: window.currentTool == "eraser"; onClicked: window.switchTool("eraser") }
			IconButton { icon.name: "pen"; active: window.currentTool == "pen"; onClicked: window.switchTool("pen") }
			IconButton { icon.name: "ellipse"; active: window.currentTool == "ellipse"; onClicked: window.switchTool("ellipse") }
			IconButton { icon.name: "rectangle"; active: window.currentTool == "rectangle"; onClicked: window.switchTool("rectangle") }
			CustomButton {
				width: 32; height: 32
				onClicked: {
					colorPicker.owner = colorPicker.owner === "stroke" ? undefined : "stroke"
					if (colorPicker.owner !== "stroke") return
					const coord = mapToGlobal(width / 2, height)
					colorPicker.x = Qt.binding(() => coord.x - colorPicker.width / 2)
					colorPicker.y = coord.y
					colorPicker.color_ = window.strokeColor
					colorPicker.callback = color => { window.strokeColor = color }
				}

				Rectangle {
					anchors.fill: parent
					color: window.strokeColor
					border.color: "white"
					border.width: 1
					radius: Theme.button.radius
				}
			}
			CustomButton {
				width: 32; height: 32
				onClicked: {
					colorPicker.owner = colorPicker.owner === "fill" ? undefined : "fill"
					if (colorPicker.owner !== "fill") return
					const coord = mapToGlobal(width / 2, height)
					colorPicker.x = Qt.binding(() => coord.x - colorPicker.width / 2)
					colorPicker.y = coord.y
					colorPicker.color_ = window.fillColor
					colorPicker.color_PickedCallback = color => { window.fillColor = color }
				}

				Rectangle {
					anchors.fill: parent
					color: window.fillColor
					border.color: "white"
					border.width: 1
					radius: Theme.button.radius
				}
			}
		}

		Rectangle {
			id: colorPicker
			property string color_
			property var owner
			property var callback
			color: "transparent"
			visible: colorPicker.owner !== undefined
			width: colorPickerLayout.implicitWidth + Theme.panel.margin * 2
			height: colorPickerLayout.implicitHeight + Theme.panel.margin * 2
			Rectangle {
				anchors.fill: parent
				anchors.margins: Theme.panel.margin
				radius: Theme.panel.radius
				color: Theme.panel.backgroundColor
				ColumnLayout {
					id: colorPickerLayout

					TextInput {
						Layout.alignment: Qt.AlignHCenter
						text: colorPicker.color_
						color: Theme.foregroundColor
						onTextEdited: colorPicker.color_ = text
						onAccepted: colorPicker.callback(colorPicker.color_)
					}

					Rectangle {
						Layout.alignment: Qt.AlignHCenter
						width: 24
						height: 24
						color: colorPicker.color_
						border.color: "white"
						border.width: 1
						radius: Theme.button.radius
					}
				}
			}
		}
	}
}
