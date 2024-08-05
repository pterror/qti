import QtQuick
import QtWayland.Compositor
import QtWayland.Compositor.XdgShell
import QtWayland.Compositor.WlShell
import QtWayland.Compositor.IviApplication

WaylandCompositor {
	id: waylandCompositor
	CompositorScreen { id: screen; compositor: waylandCompositor }
	XdgShell {
		onToplevelCreated: (toplevel, xdgSurface) => screen.handleShellSurface(xdgSurface)
	}
	IviApplication {
		onIviSurfaceCreated: (iviSurface) => screen.handleShellSurface(iviSurface)
	}
	WlShell {
		onWlShellSurfaceCreated: (shellSurface) => screen.handleShellSurface(shellSurface)
	}
	TextInputManager {}
	QtTextInputMethodManager {}
}