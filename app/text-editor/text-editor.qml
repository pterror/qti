import QtQuick
import QtQuick.Layouts
import QtQuick.Controls
import QtQuick.Dialogs
import Qti.Filesystem
import Qti.Stdlib

// TODO: https://doc.qt.io/qt-6/qfilesystemwatcher.html#fileChanged
// > As a safety measure, many applications save an open file by writing a new file
// > and then deleting the old one.
QtiWindow {
	id: window
	ColumnLayout {
		File {
			id: file
			readable: true; writable: true
			// FIXME: `open()` works but `opened = true` does not
			onPathChanged: { open(); editor.text = read() }
		}
		RowLayout {
			TextButton { text: qsTr("Open"); onClicked: openFileDialog.open() }
			TextButton { text: qsTr("Save"); onClicked: file.write(editor.text) }
			Text { text: "Path: " + file.path }
		}
		FileDialog {
			id: openFileDialog
			currentFolder: Qt.resolvedUrl(".", null)
			onAccepted: file.path = selectedFile.toString().slice("file://".length)
		}
		TextEdit { id: editor; color: Theme.foregroundColor }
	}
}
