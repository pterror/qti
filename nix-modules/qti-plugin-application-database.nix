{ pkgs, enableWayland ? true }:
(pkgs.callPackage ./plugin-module.nix {
  enableWayland = enableWayland;
}) {
  pname = "application-database";
  src = ../plugin/application-database;
  meta = {
    homepage = "https://github.com/pterror/qti/tree/master/plugin/application-database";
    description = "Application Database plugin for Qt interpreter";
  };
}
