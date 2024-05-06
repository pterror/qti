{ pkgs, enableWayland ? true }:
(pkgs.callPackage ./plugin-module.nix {
  enableWayland = enableWayland;
}) {
  pname = "screenshot";
  src = ../plugin/screenshot;
  meta = {
    homepage = "https://github.com/pterror/qti/tree/master/plugin/screenshot";
    description = "Screenshot plugin for Qt interpreter";
  };
}
