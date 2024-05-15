{
  inputs.nixpkgs.url = "nixpkgs/nixos-unstable";
  outputs = { self, nixpkgs }:
    let
      forEachSystem = fn: nixpkgs.lib.genAttrs
        nixpkgs.lib.systems.flakeExposed
        (system: fn system nixpkgs.legacyPackages.${system});
    in
    {
      packages = forEachSystem
        (system: pkgs: rec {
          qti = pkgs.callPackage ./default.nix {
            gitRev = self.rev or self.dirtyRev;
          };
          qti-plugin-stdlib = pkgs.callPackage ./nix-modules/qti-plugin-stdlib.nix { };
          qti-plugin-core = pkgs.callPackage ./nix-modules/qti-plugin-core.nix { };
          qti-plugin-filesystem = pkgs.callPackage ./nix-modules/qti-plugin-filesystem.nix { };
          qti-plugin-process = pkgs.callPackage ./nix-modules/qti-plugin-process.nix { };
          qti-plugin-clipboard = pkgs.callPackage ./nix-modules/qti-plugin-clipboard.nix { };
          qti-plugin-screenshot = pkgs.callPackage ./nix-modules/qti-plugin-screenshot.nix { };
          qti-plugin-application-database = pkgs.callPackage ./nix-modules/qti-plugin-application-database.nix { };
          qti-plugin-sql = pkgs.callPackage ./nix-modules/qti-plugin-sql.nix { };
          qti-app-screenshot-editor = pkgs.callPackage ./nix-modules/qti-app-screenshot-editor.nix { };
          default = qti;
          qti-all-plugins = [
            qti-plugin-stdlib
            qti-plugin-core
            qti-plugin-filesystem
            qti-plugin-process
            qti-plugin-clipboard
            qti-plugin-screenshot
            qti-plugin-application-database
            qti-plugin-sql
          ];
          qti-all-apps = [ qti-app-screenshot-editor ];
          qti-all = [ qti ] ++ qti-all-plugins ++ qti-all-apps;
        });

      devShells = forEachSystem (system: pkgs: rec {
        default = import ./shell.nix {
          inherit pkgs;
          inherit (self.packages.${system}) default;
        };
      });
    };
}
