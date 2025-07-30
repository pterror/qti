import QtQuick
import QtWayland.Compositor
import QtWayland.Compositor.XdgShell
import QtWayland.Compositor.WlShell

WaylandCompositor {
	id: waylandCompositor
	CompositorScreen { id: screen; compositor: waylandCompositor }
	XdgShell {
		onToplevelCreated: (toplevel, xdgSurface) => screen.handleShellSurface(xdgSurface)
	}
	WlShell {
		onWlShellSurfaceCreated: (shellSurface) => screen.handleShellSurface(shellSurface)
	}
	TextInputManager {}
	QtTextInputMethodManager {}
}
