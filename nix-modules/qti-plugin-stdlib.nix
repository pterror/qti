{ pkgs, enableWayland ? true }:
(pkgs.callPackage ./plugin-module.nix {
  enableWayland = enableWayland;
}) {
  pname = "stdlib";
  src = ../plugin/stdlib;
  meta = {
    homepage = "https://github.com/pterror/qti/tree/master/plugin/stdlib";
    description = "Stdlib plugin for Qt interpreter";
  };
}
