{ pkgs }:
(pkgs.callPackage ./plugin-module.nix { }) {
  pname = "sql";
  src = ../plugin/sql;
  meta = {
    homepage = "https://github.com/pterror/qti/tree/master/plugin/sql";
    description = "SQL plugin for Qt interpreter";
  };
}
