import QtQuick
import QtQuick.Layouts
import QtQuick.Controls
import Qti.Stdlib
import "./Functions.mjs" as Functions

ColumnLayout {
	id: ownedTab
	required property var database
	Layout.fillWidth: true
	Layout.horizontalStretchFactor: 1
	property string queryBase: "SELECT * FROM games AS g LEFT JOIN caves AS c ON g.id = c.game_id" +
		Functions.gamesSearchQuery(searchWords)
	property string querySuffix: ""
	property string query: queryBase + querySuffix
	property list<string> searchWords: []
	property var games: database.reactive.query(query)

	RowLayout {
		spacing: 16
		Text { text: ownedTab.games.rowCount() + " games" }

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
					case "Name ↑": { ownedTab.querySuffix = " ORDER BY title ASC"; break }
					case "Name ↓": { ownedTab.querySuffix = " ORDER BY title DESC"; break }
					case "Created ↑": { ownedTab.querySuffix = " ORDER BY created_at ASC"; break }
					case "Created ↓": { ownedTab.querySuffix = " ORDER BY created_at DESC"; break }
					case "Published ↑": { ownedTab.querySuffix = " ORDER BY published_at ASC"; break }
					case "Published ↓": { ownedTab.querySuffix = " ORDER BY published_at DESC"; break }
					case "None": default: { ownedTab.querySuffix = ""; break }
				}
			}
		}

		Text { text: "Search" }

		TextInput {
			Layout.fillWidth: true
			onTextEdited: ownedTab.searchWords = Functions.extractWords(text)
		}
	}

	GridView {
		id: ownedView
		Layout.alignment: Qt.AlignHCenter
		Layout.fillWidth: true
		Layout.fillHeight: true
		cellWidth: imageWidth + 20
		cellHeight: imageHeight + 90
		model: ownedTab.games
		delegate: GameDelegate { width: ownedView.cellWidth; height: ownedView.cellHeight }
	}
}
