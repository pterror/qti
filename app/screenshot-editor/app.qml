import QtQuick
import Qti.Screenshot
import "../_library"

QtiWindow {
	id: window
	property url screenshotUrl: ""
	property url oldScreenshotUrl
	Component.onCompleted: {
		Screenshot.captureAllScreens(url => { screenshotUrl = url })
		oldScreenshotUrl = screenshotUrl
	}
	onScreenshotUrlChanged: {
		Screenshot.free(oldScreenshotUrl)
		oldScreenshotUrl = screenshotUrl
	}
	Image {
		id: image
		anchors.fill: parent
		fillMode: Image.PreserveAspectFit
		source: screenshotUrl
	}
}
