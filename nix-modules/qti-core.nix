{ pkgs }:
(pkgs.callPackage ./module.nix { }) {
  pname = "qti-core";
  src = ../plugin/core;
}
