import QtQuick
import QtQuick.Layouts
import Qti.Stdlib
import Qti.Core
import Qti.Filesystem
import Qti.Process

ColumnLayout {
	id: root
	readonly property var modelData: model.display
	property var path: "verdict" in modelData ? JSON.parse(modelData.verdict || "{}").basePath : modelData.install_folder

	CustomButton {
		Layout.alignment: Qt.AlignHCenter
		ColumnLayout {
			Image {
				Layout.alignment: Qt.AlignHCenter
				Layout.preferredWidth: imageWidth
				Layout.preferredHeight: imageHeight
				fillMode: Image.PreserveAspectFit
				source: modelData.cover_url
			}

			Text {
				Layout.alignment: Qt.AlignHCenter // note: horizontal align does not work with elide
				Layout.preferredWidth: imageWidth
				elide: Text.ElideRight
				maximumLineCount: 1
				text: modelData.title
				font.weight: 700; font.pointSize: 10
			}

			Text {
				Layout.alignment: Qt.AlignHCenter
				Layout.preferredWidth: imageWidth
				elide: Text.ElideRight
				maximumLineCount: 1
				text: modelData.short_text
			}
		}
	}

	RowLayout {
		Layout.alignment: Qt.AlignHCenter

		RowLayout {
			spacing: 0
			IconButton { icon.name: "web"; onClicked: Qt.openUrlExternally(modelData.url) }
			IconButton {
				icon.name: "play"
				visible: root.path ?? false
				property var folder: Folder { path: root.path ?? "" }
				property var process: Process {}
				onClicked: {
					let filePaths = folder.files.map(file => file.path)
					if (filePaths.length === 0) {
						for (const childFolder of folder.folders) {
							if (/[.]itch$/.test(childFolder.path)) continue
							filePaths.push(...childFolder.files.map(file => file.path))
						}
					}
					const candidates = filePaths.filter(p => /[.](?:exe|html)$/.test(p) && !/UnityCrashHandler64[.]exe/.test(p))
					if (candidates.length === 0) {
						console.error("qti.itch: no launchable file in " + modelData.install_folder)
						return
					}
					const candidate = candidates[0] // assume first candidate is the right one
					const ext = candidate.split(".").slice(-1)[0]
					// TODO: update cave.last_played using game_id or id
					switch (ext) {
						case "html": { Qt.openUrlExternally(candidate); break }
						case "exe": {
							if (QtiCore.env("HOME")) {
								// assume posix
								process.program = "wine"
								process.arguments = [candidate]
								process.startDetached()
							} else {
								process.program = candidate
								process.startDetached()
							}
							break
						}
					}
				}
			}
		}

		Rectangle {
			Layout.fillWidth: true
			color: "transparent"
		}

		RowLayout {
			spacing: 0
			IconButton { icon.name: "logo-windows"; enabled: false; visible: modelData.windows === "all" }
			IconButton { icon.name: "logo-apple"; enabled: false; visible: modelData.osx === "all" }
			IconButton { icon.name: "logo-linux"; enabled: false; visible: modelData.linux === "all" }
		}
	}
}
