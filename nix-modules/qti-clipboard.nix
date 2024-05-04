{ pkgs }:
(pkgs.callPackage ./module.nix { }) {
  pname = "qti-clipboard";
  src = ../plugin/clipboard;
}
