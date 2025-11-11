{ lib
, pkgs
, stdenv
, keepDebugInfo
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
, svgSupport ? true # you almost always want this
}: stdenv.mkDerivation rec {
  pname = "qti${lib.optionalString debug "-debug"}";
  version = "0.0.1";
  src = ./.;
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
  ++ (lib.optionals enableWayland [ qt6.qtwayland wayland ])
  ++ (lib.optionals svgSupport [ qt6.qtsvg ]);
  configurePhase =
    let
      cmakeBuildType = if debug then "Debug" else "RelWithDebInfo";
    in
    ''
      cmakeBuildType=${cmakeBuildType} # qt6 setup hook resets this for some godforsaken reason
      cmakeConfigurePhase
    '';
  cmakeFlags = [
    "-DPLUGINS=OFF"
    "-DGIT_REVISION=${gitRev}"
  ];
  buildPhase = "ninjaBuildPhase";
  enableParallelBuilding = true;
  dontStrip = true;
  meta = with lib; {
    homepage = "https://github.com/pterror/qti";
    description = "Qt interpreter";
    license = licenses.isc;
    platforms = platforms.linux;
  };
}
