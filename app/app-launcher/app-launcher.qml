import QtQuick
import QtQuick.Layouts
import Qti.Stdlib
import Qti.ApplicationDatabase

QtiWindow {
	id: window
  property var what: console.log(
    ApplicationDatabase.applications[0],
    ApplicationDatabase.applications[0].type,
    ApplicationDatabase.applications[0].icon,
    ApplicationDatabase.applications[0].name,
    ApplicationDatabase.applications[0].categories,
    ApplicationDatabase.applications[0].mimeTypes,
  )
}
