{ pkgs }:
(pkgs.callPackage ./app-module.nix { }) {
  pname = "screenshot-editor";
  meta.description = "Screenshot editor app for Qt interpreter";
}
