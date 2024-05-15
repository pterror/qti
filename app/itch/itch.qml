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
			onReloaded: {
				gamesTab.games = Qt.binding(() => db.query(gamesTab.query))
				installedTab.games = Qt.binding(() => db.query(installedTab.query))
				downloadedTab.games = Qt.binding(() => db.query(downloadedTab.query))
				collectionsTab.collections = Qt.binding(() => db.query(collectionsTab.query))
			}
		}
	}
	// source images are often: [315 250] or [560 310] or [630 500]
	property int imageWidth: 240
	property int imageHeight: imageWidth * 4 / 5

	function extractWords(text) {
		return text.match(/\S+/g)
	}

	function gamesSearchQuery(words, hasWhere = false) {
		return words.length === 0
			? ""
			: (hasWhere ? " AND " : " WHERE ") + words.map(word => "(title LIKE '%" + word + "%' OR short_text LIKE '%" + word + "%')").join(" AND ")
	}

	function collectionsSearchQuery(words, hasWhere = false) {
		return words.length === 0
			? ""
			: (hasWhere ? " AND " : " WHERE ") + words.map(word => "title LIKE '%" + word + "%'").join(" AND ")
	}
	// missing tables: [game_embed_data,uploads,profile_data,cave_historical_play_times,download_keys,fetch_infos,profiles,sales,install_locations,game_uploads,schema_versions,profile_games,users,builds,profile_collections]

	ColumnLayout {
		anchors.fill: parent

		TabBar {
			id: tabBar
			Layout.fillWidth: true
			TabButton { text: qsTr("Owned games") }
			TabButton { text: qsTr("Installed games") }
			TabButton { text: qsTr("Downloads") }
			TabButton { text: qsTr("Collections") }
		}

		StackLayout {
			Layout.fillWidth: true
			Layout.horizontalStretchFactor: 1
			currentIndex: tabBar.currentIndex

			ColumnLayout {
				id: gamesTab
				Layout.fillWidth: true
				Layout.horizontalStretchFactor: 1
				property string queryBase: "SELECT * FROM games AS g LEFT JOIN caves AS c ON g.id = c.game_id" + gamesSearchQuery(searchWords)
				property string querySuffix: ""
				property string query: queryBase + querySuffix
				property list<string> searchWords: []
				property var games: db.query(query)

				RowLayout {
					spacing: 16
					Text { text: gamesTab.games.rowCount() + " games" }

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
								case "Name ↑": { gamesTab.querySuffix = " ORDER BY title ASC"; break }
								case "Name ↓": { gamesTab.querySuffix = " ORDER BY title DESC"; break }
								case "Created ↑": { gamesTab.querySuffix = " ORDER BY created_at ASC"; break }
								case "Created ↓": { gamesTab.querySuffix = " ORDER BY created_at DESC"; break }
								case "Published ↑": { gamesTab.querySuffix = " ORDER BY published_at ASC"; break }
								case "Published ↓": { gamesTab.querySuffix = " ORDER BY published_at DESC"; break }
								case "None": default: { gamesTab.querySuffix = ""; break }
							}
						}
					}

					Text { text: "Search" }

					TextInput {
						Layout.fillWidth: true
						onTextEdited: gamesTab.searchWords = extractWords(text)
					}
				}

				GridView {
					id: gamesView
					Layout.alignment: Qt.AlignHCenter
					Layout.fillWidth: true
					Layout.fillHeight: true
					cellWidth: imageWidth + 20
					cellHeight: imageHeight + 90
					model: gamesTab.games
					delegate: GameDelegate { width: gamesView.cellWidth; height: gamesView.cellHeight }
				}
			}

			ColumnLayout {
				id: installedTab
				Layout.fillWidth: true
				Layout.horizontalStretchFactor: 1
				property string queryBase: "SELECT * FROM caves AS c LEFT JOIN games AS g ON c.game_id = g.id" + gamesSearchQuery(searchWords)
				property string querySuffix: ""
				property string query: queryBase + querySuffix
				property list<string> searchWords: []
				property var games: db.query(query)

				RowLayout {
					spacing: 16
					Text { text: installedTab.games.rowCount() + " games" }

					Text { text: "Sort" }

					ComboBox {
						model: [
							"None",
							"Installed ↑", "Installed ↓",
							"Last Played ↑", "Last Played ↓",
						]

						onCurrentTextChanged: {
							switch (currentText) {
								case "Installed ↑": { installedTab.querySuffix = " ORDER BY installed_at ASC"; break }
								case "Installed ↓": { installedTab.querySuffix = " ORDER BY installed_at DESC"; break }
								case "Last Played ↑": { installedTab.querySuffix = " ORDER BY last_touched_at ASC"; break }
								case "Last Played ↓": { installedTab.querySuffix = " ORDER BY last_touched_at DESC"; break }
								case "None": default: { installedTab.querySuffix = ""; break }
							}
						}
					}

					Text { text: "Search" }

					TextInput {
						Layout.fillWidth: true
						onTextEdited: installedTab.searchWords = extractWords(text)
					}
				}

				GridView {
					id: installedView
					Layout.alignment: Qt.AlignHCenter
					Layout.fillWidth: true
					Layout.fillHeight: true
					cellWidth: imageWidth + 20
					cellHeight: imageHeight + 100
					model: installedTab.games
					delegate: GameDelegate { width: installedView.cellWidth; height: installedView.cellHeight }
				}
			}

			ColumnLayout {
				id: downloadedTab
				Layout.fillWidth: true
				Layout.horizontalStretchFactor: 1
				property string queryBase: "SELECT * FROM downloads AS d LEFT JOIN games AS g ON d.game_id = g.id" + gamesSearchQuery(searchWords)
				property string querySuffix: ""
				property string query: queryBase + querySuffix
				property list<string> searchWords: []
				property var games: db.query(query)

				RowLayout {
					spacing: 16
					Text { text: downloadedTab.games.rowCount() + " games" }

					Text { text: "Sort" }

					ComboBox {
						model: [
							"None",
							"Started ↑", "Started ↓",
							"Finished ↑", "Finished ↓",
						]

						onCurrentTextChanged: {
							switch (currentText) {
								case "Started ↑": { downloadedTab.querySuffix = " ORDER BY started_at ASC"; break }
								case "Started ↓": { downloadedTab.querySuffix = " ORDER BY started_at DESC"; break }
								case "Finished ↑": { downloadedTab.querySuffix = " ORDER BY finished_at ASC"; break }
								case "Finished ↓": { downloadedTab.querySuffix = " ORDER BY finished_at DESC"; break }
								case "None": default: { downloadedTab.querySuffix = ""; break }
							}
						}
					}

					Text { text: "Search" }

					TextInput {
						Layout.fillWidth: true
						onTextEdited: downloadedTab.searchWords = extractWords(text)
					}
				}

				GridView {
					id: downloadedView
					Layout.alignment: Qt.AlignHCenter
					Layout.fillWidth: true
					Layout.fillHeight: true
					cellWidth: imageWidth + 20
					cellHeight: imageHeight + 100
					model: downloadedTab.games
					delegate: GameDelegate { width: downloadedView.cellWidth; height: downloadedView.cellHeight }
				}
			}

			ColumnLayout {
				id: collectionsTab
				Layout.fillWidth: true
				Layout.horizontalStretchFactor: 1
				property string queryBase: "SELECT * FROM collections" + collectionsSearchQuery(searchWords)
				property string querySuffix: ""
				property string query: queryBase + querySuffix
				property list<string> searchWords: []
				property var collections: db.query(query)
				property list<string> gameSearchWords: []

				RowLayout {
					spacing: 16
					Text { text: collectionsTab.collections.rowCount() + " collections" }

					Text { text: "Sort" }

					ComboBox {
						model: [
							"None",
							"Created ↑", "Created ↓",
							"Updated ↑", "Updated ↓",
						]

						onCurrentTextChanged: {
							switch (currentText) {
								case "Created ↑": { collectionsTab.querySuffix = " ORDER BY created_at ASC"; break }
								case "Created ↓": { collectionsTab.querySuffix = " ORDER BY created_at DESC"; break }
								case "Updated ↑": { collectionsTab.querySuffix = " ORDER BY updated_at ASC"; break }
								case "Updated ↓": { collectionsTab.querySuffix = " ORDER BY updated_at DESC"; break }
								case "None": default: { collectionsTab.querySuffix = ""; break }
							}
						}
					}

					Text { text: "Search collections" }

					TextInput {
						Layout.fillWidth: true
						Layout.horizontalStretchFactor: 1
						onTextEdited: collectionsTab.searchWords = extractWords(text)
					}

					Text { text: "Search games" }

					TextInput {
						Layout.fillWidth: true
						Layout.horizontalStretchFactor: 1
						onTextEdited: collectionsTab.gameSearchWords = extractWords(text)
					}
				}

				ListView {
					id: collectionsView
					Layout.alignment: Qt.AlignHCenter
					Layout.fillWidth: true
					Layout.fillHeight: true
					orientation: Qt.Vertical
					model: collectionsTab.collections
					delegate: ColumnLayout {
						id: collectionElement
						visible: games.rowCount() !== 0
						width: collectionsView.width
						height: games.rowCount() === 0 ? 0 : imageHeight + 140
						readonly property var modelData: model.display
						property string queryBase: "SELECT * FROM collection_games AS cg LEFT JOIN games AS g ON cg.game_id = g.id LEFT JOIN caves AS c ON cg.game_id = c.game_id WHERE collection_id = ?" + gamesSearchQuery(collectionsTab.gameSearchWords, true)
						property string query: queryBase
						property var games: db.query(query, [modelData.id])

						Text {
							id: collectionTitle
							text: modelData.title
							font.weight: 700; font.pointSize: 14
						}

						ListView {
							id: collectionView
							orientation: Qt.Horizontal
							width: collectionsView.width
							height: imageHeight + 90
							model: collectionElement.games
							delegate: GameDelegate { width: imageWidth + 20; height: collectionView.height }
						}
					}
				}
			}
		}
	}
}
