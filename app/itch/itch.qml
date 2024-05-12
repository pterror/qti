import QtQuick
import QtQuick.Layouts
import QtQuick.Controls
import Qti.Stdlib
import Qti.Core
import Qti.Sql

QtiWindow {
	id: window
	property var db: SqlDatabase { name: QtiCore.env("HOME") + "/.config/itch/db/butler.db" }
	property int imageWidth: 320
	property int imageHeight: 240

	// property list<string> tables: db.getTables()
	// ["game_embed_data","uploads","profile_data","cave_historical_play_times","caves","download_keys","downloads","fetch_infos","profiles","collections","games","sales","install_locations","game_uploads","schema_versions","collection_games","profile_games","users","builds","profile_collections"]

	ColumnLayout {
		anchors.fill: parent

		TabBar {
			id: tabBar
			Layout.fillWidth: true
			TabButton { text: qsTr("Owned games") }
			TabButton { text: qsTr("Installed games") }
			TabButton { text: qsTr("Collections") }
		}

		StackLayout {
			Layout.fillWidth: true
			currentIndex: tabBar.currentIndex

			ColumnLayout {
				RowLayout {
					RowLayout {
						Text { text: "Sort" }

						ComboBox {
							id: ownedGamesSort
							model: [
								"None",
								"Name ↑", "Name ↓",
								"Created ↑", "Created ↓",
								"Published ↑", "Published ↓",
							]
						}
					}
				}

				GridLayout {
					columns: Math.floor(window.width / imageWidth)
					property list<var> list: db.getRows("games")

					Repeater {
						model: {
							switch (ownedGamesSort.currentText) {
								case "None": { return list }
								case "Name ↑": { return list.sort((a, b) => a.title > b.title ? 1 : a.title < b.title ? -1 : 0) }
								case "Name ↓": { return list.sort((a, b) => a.title > b.title ? -1 : a.title < b.title ? 1 : 0) }
								case "Created ↑": { return list.sort((a, b) => new Date(a.created_at) - new Date(b.created_at)) }
								case "Created ↓": { return list.sort((a, b) => new Date(b.created_at) - new Date(a.created_at)) }
								case "Published ↑": { return list.sort((a, b) => new Date(a.published_at) - new Date(b.published_at)) }
								case "Published ↓": { return list.sort((a, b) => new Date(b.published_at) - new Date(a.published_at)) }
							}
						}
						property var what: console.log("game", JSON.stringify(model[0]))

						ColumnLayout {
							required property var modelData
							width: imageWidth

							CustomButton {
								Layout.alignment: Qt.AlignVCenter
								ColumnLayout {
									Image {
										Layout.alignment: Qt.AlignVCenter
										width: imageWidth
										height: imageHeight
										source: modelData.cover_url
									}

									Text {
										Layout.alignment: Qt.AlignVCenter
										text: modelData.title
									}

									Text {
										Layout.alignment: Qt.AlignVCenter
										text: modelData.short_text
									}
								}
							}

							RowLayout {
								Layout.alignment: Qt.AlignVCenter
								TextButton {
									text: "Web"
									onClicked: Qt.openUrlExternally(modelData.url)
								}
							}

							RowLayout {
								Layout.alignment: Qt.AlignVCenter
								IconButton { icon.name: "logo-windows"; enabled: false; visible: modelData.windows === "all" }
								IconButton { icon.name: "logo-apple"; enabled: false; visible: modelData.osx === "all" }
								IconButton { icon.name: "logo-linux"; enabled: false; visible: modelData.linux === "all" }
							}
						}
					}
				}
			}

			GridLayout {
				Repeater {
					model: []
					Component.onCompleted: model = Qt.binding(() => db.getRows("downloads"))
					property var what: console.log("download", JSON.stringify(model[0]))
					// TODO:
				}
			}

			GridLayout {
				Repeater {
					model: []
					Component.onCompleted: model = Qt.binding(() => db.getRows("collections"))
					property var what: console.log("collection", JSON.stringify(model[0]))
					// TODO:
				}
			}
		}
	}
}
