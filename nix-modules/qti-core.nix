{ pkgs }:
(pkgs.callPackage ./module.nix { }) {
  pname = "qti-core";
  src = ../plugin/core;
  meta = {
    homepage = "https://github.com/pterror/qti/tree/master/plugin/core";
    description = "Core plugin for Qt interpreter";
  };
}
