{ lib
, pkgs
, keepDebugInfo
, buildStdenv ? pkgs.clang17Stdenv
, cmake
, ninja
, qt6
, wayland
, wayland-protocols
, gitRev ? (
    let
      headExists = builtins.pathExists ./.git/HEAD;
      headContent = builtins.readFile ./.git/HEAD;
    in
    if headExists
    then
      (
        let
          matches = builtins.match "ref: refs/heads/(.*)\n" headContent;
        in
        if matches != null
        then builtins.readFile ./.git/refs/heads/${builtins.elemAt matches 0}
        else headContent
      )
    else "unknown"
  )
, debug ? false
, enableWayland ? true
}: args: buildStdenv.mkDerivation (args // rec {
  pname = "${args.pname}${lib.optionalString debug "-debug"}";
  version = args.version or "0.0.1";
  nativeBuildInputs = with pkgs; [
    cmake
    ninja
    qt6.wrapQtAppsHook
  ] ++ (lib.optionals enableWayland [
    pkg-config
    wayland-protocols
    wayland-scanner
  ]);
  buildInputs = with pkgs; [
    qt6.qtbase
    qt6.qtdeclarative
  ]
  ++ (lib.optionals enableWayland [ qt6.qtwayland wayland ]);

  QTWAYLANDSCANNER = lib.optionalString enableWayland "${qt6.qtwayland}/libexec/qtwaylandscanner";

  configurePhase =
    let
      cmakeBuildType = if debug then "Debug" else "RelWithDebInfo";
    in
    ''
      cmakeBuildType=${cmakeBuildType} # qt6 setup hook resets this for some godforsaken reason
      cmakeConfigurePhase
    '';

  qtPluginPrefix = "lib/qt-6/plugins";

  cmakeFlags = [
    "-DINSTALL_PLUGINSDIR=${qtPluginPrefix}"
    "-DGIT_REVISION=${gitRev}"
  ] ++ lib.optional (!enableWayland) "-DWAYLAND=OFF";

  buildPhase = "ninjaBuildPhase";
  enableParallelBuilding = true;
  dontStrip = true;

  meta = with lib; {
    homepage = "https://github.com/pterror/qti";
    description = "Qt interpreter";
    license = licenses.isc;
    platforms = platforms.linux;
  } // (args.meta or { });
})
