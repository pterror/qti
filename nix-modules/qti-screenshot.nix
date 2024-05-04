{ pkgs, enableWayland ? true }:
(pkgs.callPackage ./module.nix {
  enableWayland = enableWayland;
}) {
  pname = "qti-screenshot";
  src = ../plugin/screenshot;
}
