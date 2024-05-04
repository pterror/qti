{ pkgs }:
(pkgs.callPackage ./module.nix { }) {
  pname = "qti-clipboard";
  src = ../plugin/clipboard;
  meta = {
    homepage = "https://github.com/pterror/qti/tree/master/plugin/clipboard";
    description = "Clipboard plugin for Qt interpreter";
  };
}
