import QtQuick
import QtQuick.Layouts
import QtQuick.Controls
import Qti.Filesystem
import "../_library"

// TODO: icons from QFileIconProvider
QtiWindow {
	id: window
	function fileName(path) { return path.match(/[^/]+$/)[0] }
	GridLayout {
		Folder {
			id: folder
			path: Qt.resolvedUrl(".", null).toString().replace(/^file:[/][/]|[/]$/g, "")
		}
		Repeater {
			model: folder.folders
			Text {
				required property var modelData
				text: window.fileName(modelData.path)
			}
		}
		Repeater {
			model: folder.files
			ColumnLayout {
				required property var modelData
				Button {
					Layout.alignment: Qt.AlignHCenter
					// TODO: get handling
					icon.name: modelData.mimeType || "Alacritty"
					icon.height: 64
					icon.width: 64
				}
				Text {
					Layout.alignment: Qt.AlignHCenter
					text: window.fileName(modelData.path)
				}
			}
		}
	}
}
