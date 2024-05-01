import QtQuick
import Qti.Screenshot
import "../_library"

QtiWindow {
	id: window
	property url screenshotUrl: ""
	property url oldScreenshotUrl
	Component.onCompleted: {
		Screenshot.captureAllScreens(url => {
			console.log('ok', url)
			image.source = url
		}, () => console.log('error'))
		oldScreenshotUrl = screenshotUrl
	}
	onScreenshotUrlChanged: {
		Screenshot.free(oldScreenshotUrl)
		oldScreenshotUrl = screenshotUrl
	}
	Image {
		id: image
		source: screenshotUrl
		anchors.fill: parent
	}
}
