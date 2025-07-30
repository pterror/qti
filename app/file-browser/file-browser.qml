import QtQuick
import QtQuick.Layouts
import QtQuick.Controls
import Qti.Stdlib
import Qti.Filesystem

// TODO: icons from QFileIconProvider
QtiWindow {
	id: window
	function fileName(path) { return path.match(/[^/]+$/)[0] }
	property string path: Qt.resolvedUrl(".", null).toString().replace(/^file:[/][/]|[/]$/g, "")

	ColumnLayout {
		RowLayout {
			Repeater {
				model: {
					const segments = String(window.path).match(/[^/]+|[/]/g) ?? [];
					const model = []
					let path = "";
					for (const segment of segments) {
						path += segment;
						model.push({ segment, path });
					}
					return model;
				}
				TextButton {
					required property var modelData
					text: modelData.segment
					enabled: text !== "/"
					onClicked: window.path = modelData.path
				}
			}
		}

		GridLayout {
			uniformCellWidths: true
			uniformCellHeights: true
			implicitWidth: window.width

			Folder { id: folder; path: window.path }

			Repeater {
				model: folder.folders

				CustomButton {
					id: button
					required property var modelData
					implicitWidth: 64
					implicitHeight: 64
					onDoubleClicked: window.path = modelData.path

					ColumnLayout {
						// IconButton {
						// 	Layout.alignment: Qt.AlignHCenter
						// 	enabled: false
						// 	// icon.name: "folder"
						// 	icon.height: 64; icon.width: 64
						// }
						Rectangle {
							width: button.implicitWidth
							height: button.implicitHeight
							color: "white"
							Text {
								horizontalAlignment: Qt.AlignHCenter
								width: parent.width
								text: window.fileName(modelData.path)
								elide: Text.ElideRight
								maximumLineCount: 1
							}
						}
					}
				}
			}

			Repeater {
				model: folder.files

				CustomButton {
					id: button
					required property var modelData
					implicitWidth: 64
					implicitHeight: 64
					onDoubleClicked: Qt.openUrlExternally(modelData.path)

					ColumnLayout {
						// IconButton {
						// 	Layout.alignment: Qt.AlignHCenter
						// 	enabled: false
						// 	// TODO: get handling
						// 	// icon.name: modelData.mimeType || "text-x-generic"
						// 	icon.height: 64; icon.width: 64
						// }
						Rectangle {
							width: button.implicitWidth
							Text {
								horizontalAlignment: Qt.AlignHCenter
								width: parent.width
								text: window.fileName(modelData.path)
								elide: Text.ElideRight
								maximumLineCount: 1
							}
						}
					}
				}
			}
		}
	}
}
