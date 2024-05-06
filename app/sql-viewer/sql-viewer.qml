import QtQuick
import QtQuick.Layouts
import Qt.labs.qmlmodels
import Qti.Sql
import Qti.Stdlib

QtiWindow {
	id: window
	property alias path: database.name
	property list<string> tableNames: []
	Component.onCompleted: update()
	function update() {
		const newTableNames = []
		database.transaction(tx => {
			const ret = tx.executeSql("SELECT name FROM sqlite_master WHERE type='table';")
			for (let i = 0; i < ret.rows.length; i += 1) {
				newTableNames.push(ret.rows.item(i).name)
			}
			tableNames = newTableNames
		})
	}

	SqlDatabase { id: database }

	RowLayout {
		ColumnLayout {
			Repeater {
				model: database.tables

				Text { // QtiTextButton {
					required property var modelData
					text: modelData.name
				}
			}
		}

		TableView {
			// model: TableModel {
			// 	Repeater {
			// 		model: table
			// 		TableModelColumn {
			// 			required property var modelData
			// 			display: modelData // FIXME:
			// 		}
			// 	}
			// }
		}
	}
}
