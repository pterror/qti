import QtQuick
import Qti.Screenshot

QtiWindow {
	id: window
	property url screenshotUrl: Screenshot.captureAllScreens()
	property url oldScreenshotUrl
	Component.onCompleted: oldScreenshotUrl = screenshotUrl
	onScreenshotUrlChange: {
		Screenshot.free(oldScreenshotUrl)
		oldScreenshotUrl = screenshotUrl
	}
	Image { source: screenshotUrl }
}
