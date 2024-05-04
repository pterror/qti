{ pkgs, enableWayland ? true }:
(pkgs.callPackage ./module.nix {
  enableWayland = enableWayland;
}) {
  pname = "qti-screenshot";
  src = ../plugin/screenshot;
  meta = {
    homepage = "https://github.com/pterror/qti/tree/master/plugin/screenshot";
    description = "Screenshot plugin for Qt interpreter";
  };
}
