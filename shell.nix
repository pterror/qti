with import <nixpkgs> { };
mkShell {
  nativeBuildInputs = [
    cmake
    kdePackages.qtshadertools
    qt6.wrapQtAppsHook
    makeWrapper
    wayland-scanner
  ];
  buildInputs = [
    qt6.qtbase
    qt6.qtdeclarative
    qt6.qtsvg
    # FIXME: make wayland optional
    qt6.qtwayland
    wayland
  ];

  QTWAYLANDSCANNER = "${qt6.qtwayland}/libexec/qtwaylandscanner";

  shellHook = ''
    export CMAKE_BUILD_PARALLEL_LEVEL=$(nproc)

    # Add Qt-related environment variables.
    # https://discourse.nixos.org/t/qt-development-environment-on-a-flake-system/23707/5
    setQtEnvironment=$(mktemp)
    random=$(openssl rand -base64 20 | sed "s/[^a-zA-Z0-9]//g")
    makeShellWrapper "$(type -p sh)" "$setQtEnvironment" "''${qtWrapperArgs[@]}" --argv0 "$random"
    sed "/$random/d" -i "$setQtEnvironment"
    source "$setQtEnvironment"

    # qmlls does not account for the import path and bases its search off qtbase's path.
    # The actual imports come from qtdeclarative. This directs qmlls to the correct imports.
    export QMLLS_BUILD_DIRS=$(pwd)/build:$QML2_IMPORT_PATH
  '';
}
