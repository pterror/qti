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
			id: database; name: QtiCore.env("HOME") + "/.config/itch/db/butler.db"
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
			TabButton { text: qsTr("Installed games") }
			TabButton { text: qsTr("Owned games") }
			TabButton { text: qsTr("Browse") }
			TabButton { text: qsTr("Downloads") }
			TabButton { text: qsTr("Collections") }
		}

		StackLayout {
			Layout.fillWidth: true
			Layout.horizontalStretchFactor: 1
			currentIndex: tabBar.currentIndex
			InstalledTab { database: database }
			OwnedTab { database: database }
			BrowseTab { database: database }
			DownloadedTab { database: database }
			CollectionsTab { database: database }
		}
	}
}
