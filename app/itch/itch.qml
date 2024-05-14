import QtQuick
import QtQuick.Layouts
import QtQuick.Controls
import Qti.Stdlib
import Qti.Core
import Qti.Sql

QtiWindow {
	id: window
	Item { SqlDatabase { id: db; name: QtiCore.env("HOME") + "/.config/itch/db/butler.db" } }
	property var games: db.tables.games.rows
	property var gamesById: games.reduce((obj, game) => (obj[game.game_id] = game, obj), {})
	property var downloads: db.tables.downloads.rows
	property var downloadsById: downloads.reduce((obj, download) => (obj[download.game_id] = download, obj), {})
	// source images are often: [315 250] or [560 310] or [630 500]
	property int imageWidth: 240
	property int imageHeight: imageWidth * 4 / 5

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
			Layout.horizontalStretchFactor: 1
			currentIndex: tabBar.currentIndex

			ColumnLayout {
				Layout.fillWidth: true
				Layout.horizontalStretchFactor: 1

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

				GridView {
					id: gamesGrid
					Layout.alignment: Qt.AlignHCenter
					Layout.fillWidth: true
					Layout.fillHeight: true
					cellWidth: imageWidth + 20
					cellHeight: imageHeight + 100
					model: {
						switch (ownedGamesSort.currentText) {
							case "None": { return window.games }
							case "Name ↑": { return window.games.sort((a, b) => a.title > b.title ? 1 : a.title < b.title ? -1 : 0) }
							case "Name ↓": { return window.games.sort((a, b) => a.title > b.title ? -1 : a.title < b.title ? 1 : 0) }
							case "Created ↑": { return window.games.sort((a, b) => new Date(a.created_at) - new Date(b.created_at)) }
							case "Created ↓": { return window.games.sort((a, b) => new Date(b.created_at) - new Date(a.created_at)) }
							case "Published ↑": { return window.games.sort((a, b) => new Date(a.published_at) - new Date(b.published_at)) }
							case "Published ↓": { return window.games.sort((a, b) => new Date(b.published_at) - new Date(a.published_at)) }
							default: { return window.games }
						}
					}
					property var what: console.log("game", JSON.stringify(model[0]))
					delegate: gameDelegate
					Component {
						id: gameDelegate
						ColumnLayout {
							required property var modelData
							width: gamesGrid.cellWidth
							height: gamesGrid.cellHeight

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
										Layout.alignment: Qt.AlignHCenter
										text: modelData.title
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

								IconButton {
									icon.name: "web"
									onClicked: Qt.openUrlExternally(modelData.url)
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
					}
				}
			}

			ColumnLayout {
				RowLayout {
					RowLayout {
						Text { text: "Sort" }

						ComboBox {
							id: downloadedGamesSort
							model: [
								"None",
								"Started ↑", "Started ↓",
								"Finished ↑", "Finished ↓",
							]
						}
					}
				}

				GridView {
					Layout.fillWidth: true
					Repeater {
						model: {
							switch (downloadedGamesSort.currentText) {
								case "None": { return window.downloads }
								case "Started ↑": { return window.downloads.sort((a, b) => new Date(a.started_at) - new Date(b.started_at)) }
								case "Started ↓": { return window.downloads.sort((a, b) => new Date(b.started_at) - new Date(a.started_at)) }
								case "Finished ↑": { return window.downloads.sort((a, b) => new Date(a.finished_at) - new Date(b.finished_at)) }
								case "Finished ↓": { return window.downloads.sort((a, b) => new Date(b.finished_at) - new Date(a.finished_at)) }
								default: { return window.downloads }
							}
						}
						property var what: console.log("download", JSON.stringify(model[0]))
						// TODO:
					}
				}
			}


			GridView {
				Repeater {
					model: db.tables.collections.rows
					property var what: console.log("collection", JSON.stringify(model[0]))
					// TODO:
				}
			}
		}
	}
}
