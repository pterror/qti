import QtQuick
import QtQuick.Layouts
import QtQuick.Controls
import Qti.Stdlib
import "./Functions.mjs" as Functions

ColumnLayout {
	id: installedTab
	required property var database
	Layout.fillWidth: true
	Layout.horizontalStretchFactor: 1
	property string queryBase: "SELECT * FROM caves AS c LEFT JOIN games AS g ON c.game_id = g.id" + Functions.gamesSearchQuery(searchWords)
	property string querySuffix: ""
	property string query: queryBase + querySuffix
	property list<string> searchWords: []
	property var games: database.reactive.query(query)

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
			onTextEdited: installedTab.searchWords = Functions.extractWords(text)
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
