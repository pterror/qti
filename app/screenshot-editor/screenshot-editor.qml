import QtQuick
import QtQuick.Layouts
import QtQuick.Shapes
import QtQuick.Dialogs
import Qti.Core
import Qti.Screenshot
import Qti.Clipboard
import "../_library"

// TODO: show decorations in a list
// TODO: actually implement eraser
QtObject {
	id: realRoot
	property real windowCloseDate: 0

	property Connections copyDetection: Connections {
		target: Clipboard
		function onChanged(mode) {
			if (windowCloseDate === 0 || Number(new Date()) < windowCloseDate || mode !== Clipboard.Global) return
			Qt.quit()
		}
	}

	property QtiWindow window: QtiWindow {
		id: window
		margins: 0
		property url screenshotUrl: ""
		property string currentTool: "crop"
		property var fillColor: "#00000000"
		property var strokeColor: "#cccccc"
		property int cropStartX: 0
		property int cropStartY: 0
		property int cropX: 0
		property int cropY: 0
		property int cropWidth: image.sourceSize.width || window.width
		property int cropHeight: image.sourceSize.height || window.height
		property int strokeWidth: 4
		property int wheelEventCooldown: 50
		property real lastWheelEvent: 0
		property bool closeOnGrab: true
		property var tools: ["pointer", "crop", "eraser", "pen", "ellipse", "rectangle"]
		property var currentDecoration: undefined
		property string savePath: ""
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
				const scaledX = (event.x - (window.width - image.sourceSize.width * image.scale) / 2) / image.scale
				const scaledY = (event.y - (window.height - image.sourceSize.height * image.scale) / 2) / image.scale
				y: !window ? 0 : (window.height - image.sourceSize.height) / 2
				switch (window.currentTool) {
					case "crop": {
						window.cropX = scaledX
						window.cropStartX = window.cropX
						window.cropY = scaledY
						window.cropStartY = window.cropY
						window.cropWidth = 0
						window.cropHeight = 0
						break
					}
					case "pen":
					case "ellipse":
					case "rectangle": {
						const props = Object.assign({
							startX: scaledX, startY: scaledY,
							color: window.fillColor,
						}, window.currentTool === "pen" ? {} : { x: scaledX, y: scaledY, width: 1, height: 1 })
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
				const scaledX = (event.x - (window.width - image.sourceSize.width * image.scale) / 2) / image.scale
				const scaledY = (event.y - (window.height - image.sourceSize.height * image.scale) / 2) / image.scale
				switch (window.currentTool) {
					case "crop": {
						window.cropX = Math.min(scaledX, window.cropStartX)
						window.cropY = Math.min(scaledY, window.cropStartY)
						window.cropWidth = Math.abs(scaledX - window.cropStartX)
						window.cropHeight = Math.abs(scaledY - window.cropStartY)
						break
					}
					case "pen": {
						if (!window.currentDecoration) return
						window.currentDecoration.lineTo(scaledX, scaledY)
						break
					}
					case "ellipse":
					case "rectangle": {
						if (!window.currentDecoration) return
						window.currentDecoration.x = Math.min(scaledX, window.currentDecoration.startX)
						window.currentDecoration.y = Math.min(scaledY, window.currentDecoration.startY)
						window.currentDecoration.width = Math.abs(scaledX - window.currentDecoration.startX)
						window.currentDecoration.height = Math.abs(scaledY - window.currentDecoration.startY)
						break
					}
				}
			}
		}

		Item {
			id: root
			property bool grabbing: false
			width: !window ? 0 : grabbing ? window.cropWidth : parent.width
			height: !window ? 0 : grabbing ? window.cropHeight : parent.height

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

			function grab(callback) {
				root.grabbing = true
				root.grabToImage(result => {
					root.grabbing = false
					callback(result)
					if (window.closeOnGrab) {
						Screenshot.free(window.screenshotUrl)
						windowCloseDate = Number(new Date()) + 50
						window.close()
						QtiCore?.deleteLater(window)
					}
				})
			}

			Shortcut {
				sequence: "Ctrl+C"
				onActivated: root.grab(result => { Clipboard.setImage(result.image) })
			}

			Shortcut { sequence: "Ctrl+S"; onActivated: saveDialog.open() }

			FileDialog {
				id: saveDialog
				fileMode: FileDialog.SaveFile
				defaultSuffix: "png"
				onAccepted: root.grab(result => {
					result.saveToFile(selectedFile)
					if (window.closeOnGrab) Qt.quit()
				})
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
						capStyle: ShapePath.RoundCap
						joinStyle: ShapePath.RoundJoin
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
				source: window.screenshotUrl
				scale: root.grabbing ? 1 : Math.min(
					window.width / (image.sourceSize.width || window.width),
					window.height / (image.sourceSize.height || window.height)
				)
				x: !window ? 0 : root.grabbing ? -window.cropX : (window.width - image.sourceSize.width) / 2
				y: !window ? 0 : root.grabbing ? -window.cropY : (window.height - image.sourceSize.height) / 2

				Item {
					id: decorationsContainer
					anchors.fill: parent
				}

				Rectangle {
					visible: !root.grabbing
					color: Theme.maskColor
					width: image.sourceSize.width
					height: window?.cropY ?? 0
				}

				Rectangle {
					visible: !root.grabbing
					color: Theme.maskColor
					y: window?.cropY + window?.cropHeight
					width: image.sourceSize.width
					height: image.sourceSize.height - window?.cropY - window?.cropHeight
				}

				Rectangle {
					visible: !root.grabbing
					color: Theme.maskColor
					y: window?.cropY ?? 0
					width: window?.cropX ?? 0
					height: window?.cropHeight
				}

				Rectangle {
					visible: !root.grabbing
					color: Theme.maskColor
					x: window?.cropX + window?.cropWidth
					y: window?.cropY ?? 0
					width: image.sourceSize.width - window?.cropWidth
					height: window?.cropHeight ?? 0
				}
			}
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
