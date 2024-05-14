import QtQuick
import QtQuick.Layouts
import QtQuick.Controls
import Qti.Stdlib
import Qti.Core
import Qti.Sql

QtiWindow {
	id: window
	Item {
		SqlDatabase {
			id: db; name: QtiCore.env("HOME") + "/.config/itch/db/butler.db"
			onReloaded: games = Qt.binding(() => db.query(window.query))
		}
	}
	property string gamesQueryBase: "SELECT * FROM games"
	property string query: gamesQueryBase
	property var games: db.query(query)
	// source images are often: [315 250] or [560 310] or [630 500]
	property int imageWidth: 240
	property int imageHeight: imageWidth * 4 / 5

	// tables: [game_embed_data,uploads,profile_data,cave_historical_play_times,caves,download_keys,downloads,fetch_infos,profiles,collections,games,sales,install_locations,game_uploads,schema_versions,collection_games,profile_games,users,builds,profile_collections]

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

							onCurrentTextChanged: {
								switch (currentText) {
									case "Name ↑": { window.query = gamesQueryBase + " ORDER BY title ASC"; break }
									case "Name ↓": { window.query = gamesQueryBase + " ORDER BY title DESC"; break }
									case "Created ↑": { window.query = gamesQueryBase + " ORDER BY created_at ASC"; break }
									case "Created ↓": { window.query = gamesQueryBase + " ORDER BY created_at DESC"; break }
									case "Published ↑": { window.query = gamesQueryBase + " ORDER BY published_at ASC"; break }
									case "Published ↓": { window.query = gamesQueryBase + " ORDER BY published_at DESC"; break }
									case "None":
									default: { window.query = gamesQueryBase; break }
								}
							}
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
					model: window.games
					delegate: ColumnLayout {
						readonly property var modelData: model.display
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
									Layout.alignment: Qt.AlignHCenter // note: horizontal align does not work with elide
									Layout.preferredWidth: imageWidth
									elide: Text.ElideRight
									maximumLineCount: 1
									text: modelData.title
									font.weight: 700
									font.pointSize: 10
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
						// TODO:
					}
				}
			}


			GridView {
				Repeater {
					model: db.tables.collections.rows
					// TODO:
				}
			}
		}
	}
}
