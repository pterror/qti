import QtQuick
import QtQml.Models
import QtWayland.Compositor
import QtWayland.Compositor.XdgShell
import Qti.Process

WaylandCompositor {
	id: waylandCompositor
	property var process: Process {}
	property int uninitializedScreens: Qt.application.screens.length

	function exec(program: string, args: list<string> = []) {
		process.program = program
		process.args = args
		process.startDetached()
	}

	onUninitializedScreensChanged: {
		if (uninitializedScreens !== 0) return
		exec("quickshell")
	}

	Instantiator {
		id: screens
		model: Qt.application.screens

		delegate: CompositorScreen {
			text: name
			compositor: waylandCompositor
			screen: modelData
			Component.onCompleted: {
				if (!waylandCompositor.defaultOutput) waylandCompositor.defaultOutput = this
				waylandCompositor.uninitializedScreens -= 1
			}
			position: Qt.point(virtualX, virtualY)
		}
	}
	
	XdgShell { onToplevelCreated: (toplevel: XdgToplevel, xdgSurface: XdgSurface) => screen.handleShellSurface(xdgSurface) }
	XdgDecorationManagerV1 { preferredMode: XdgToplevel.ServerSideDecoration }
	TextInputManager {}
	Shortcut { sequence: "Super+M"; onActivated: Qt.quit() }
	Shortcut { sequence: "Super+Q"; onActivated: exec("kitty") }
	Shortcut { sequence: "Super+A"; onActivated: exec("firefox") }
}
