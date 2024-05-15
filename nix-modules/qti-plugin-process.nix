{ pkgs }:
(pkgs.callPackage ./plugin-module.nix { }) {
  pname = "process";
  src = ../plugin/process;
  meta = {
    homepage = "https://github.com/pterror/qti/tree/master/plugin/process";
    description = "Process plugin for Qt interpreter";
  };
}
