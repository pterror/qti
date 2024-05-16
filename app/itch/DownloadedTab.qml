import QtQuick
import QtQuick.Layouts
import QtQuick.Controls
import Qti.Stdlib
import "./Functions.mjs" as Functions

ColumnLayout {
	id: downloadedTab
	required property var database
	Layout.fillWidth: true
	Layout.horizontalStretchFactor: 1
	property string queryBase: "SELECT * FROM downloads AS d LEFT JOIN games AS g ON d.game_id = g.id" + Functions.gamesSearchQuery(searchWords)
	property string querySuffix: ""
	property string query: queryBase + querySuffix
	property list<string> searchWords: []
	property var games: database.reactive.query(query)

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
			onTextEdited: downloadedTab.searchWords = Functions.extractWords(text)
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
