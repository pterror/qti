{ pkgs }:
(pkgs.callPackage ./app-module.nix { isStdlib = true; }) {
  pname = "stdlib";
  meta.description = "QML app stdlib for Qt interpreter";
}
