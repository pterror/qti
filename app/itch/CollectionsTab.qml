import QtQuick
import QtQuick.Layouts
import QtQuick.Controls
import Qti.Stdlib
import "./Functions.mjs" as Functions

ColumnLayout {
	id: collectionsTab
	required property var database
	Layout.fillWidth: true
	Layout.horizontalStretchFactor: 1
	property string queryBase: "SELECT * FROM collections" + Functions.collectionsSearchQuery(searchWords)
	property string querySuffix: ""
	property string query: queryBase + querySuffix
	property list<string> searchWords: []
	property var collections: database.reactive.query(query)
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
			onTextEdited: collectionsTab.searchWords = Functions.extractWords(text)
		}

		Text { text: "Search games" }

		TextInput {
			Layout.fillWidth: true
			Layout.horizontalStretchFactor: 1
			onTextEdited: collectionsTab.gameSearchWords = Functions.extractWords(text)
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
			property string queryBase: "SELECT * FROM collection_games AS cg LEFT JOIN games AS g ON cg.game_id = g.id LEFT JOIN caves AS c ON cg.game_id = c.game_id WHERE collection_id = ?" + Functions.gamesSearchQuery(collectionsTab.gameSearchWords, true)
			property string query: queryBase
			property var games: database.reactive.query(query, [modelData.id])

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
