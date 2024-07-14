# qti

a qt qml interpreter.

## qml plugins

located in `plugin/`

- `Qti.ApplicationDatabase` - xdg application database exposed as a model
- `Qti.Clipboard` - wrapper over qt clipboard functionality
- `Qti.Core` - general functionality not associated with a specific feature
- `Qti.Filesystem` - wrapper over filesystem apis (listing directories, reading to/writing from files) exposed as models
- `Qti.Screenshot` - wrapper over the wayland screencopy protocol
- `Qti.Sql` - wrapper over qt sql exposed as models
- `Qti.Stdlib` - custom components (and theme) written in qml

## qml apps

located in `app/`

- `screenshot-editor` - edits screenshots. inspired by satty/flameshot/lightshot
- `itch` - (unfinished) itch client
- other apps in `app/` do not work yet
