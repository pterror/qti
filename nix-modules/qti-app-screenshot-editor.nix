{ pkgs }:
(pkgs.callPackage ./app-module.nix { }) {
  pname = "screenshot-editor";
  meta.description = "Screenshot editor app for Qt interpreter";
  buildInputs = [
    (pkgs.callPackage ./qti-plugin-core.nix { })
    (pkgs.callPackage ./qti-plugin-screenshot.nix { })
    (pkgs.callPackage ./qti-plugin-clipboard.nix { })
  ];
}
