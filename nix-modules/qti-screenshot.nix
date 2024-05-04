{ qtModule
, qtbase
, libwebp
, jasper
, libmng
, zlib
, pkg-config
, lib
, stdenv
, enableWayland ? true
}:

qtModule {
  pname = "qti-screenshot";
  propagatedBuildInputs = [ qtbase ];
  buildInputs = [ ];
  nativeBuildInputs = [ pkg-config ] ++ (lib.optionals enableWayland [
    wayland-protocols
    wayland-scanner
  ]);
}
