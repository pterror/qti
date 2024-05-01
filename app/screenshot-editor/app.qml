import QtQuick
import Qti.Screenshot
import "../_library"

QtiWindow {
	id: window
	property url screenshotUrl: ""
	property url oldScreenshotUrl
	Component.onCompleted: {
		Screenshot.captureAllScreens((ret) => console.log('ok', ret), () => console.log('error'))
		oldScreenshotUrl = screenshotUrl
	}
	onScreenshotUrlChanged: {
		Screenshot.free(oldScreenshotUrl)
		oldScreenshotUrl = screenshotUrl
	}
	Image {
		source: screenshotUrl
	}
}
