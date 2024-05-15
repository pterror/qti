{ pkgs }:
(pkgs.callPackage ./app-module.nix { }) {
  pname = "itch";
  meta.description = "Itch app for Qt interpreter";
  buildInputs = [
    (pkgs.callPackage ./qti-plugin-core.nix { })
    (pkgs.callPackage ./qti-plugin-sql.nix { })
    (pkgs.callPackage ./qti-plugin-filesystem.nix { })
    (pkgs.callPackage ./qti-plugin-process.nix { })
  ];
}
