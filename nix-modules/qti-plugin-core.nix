{ pkgs }:
(pkgs.callPackage ./plugin-module.nix { }) {
  pname = "core";
  src = ../plugin/core;
  meta = {
    homepage = "https://github.com/pterror/qti/tree/master/plugin/core";
    description = "Core plugin for Qt interpreter";
  };
}
