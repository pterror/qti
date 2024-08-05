{ pkgs }:
(pkgs.callPackage ./app-module.nix { }) {
  pname = "wayland-compositor";
  meta.description = "Wayland compositor app for Qt interpreter";
}
