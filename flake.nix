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
          qti-plugin-core = pkgs.callPackage ./nix-modules/qti-plugin-core.nix { };
          qti-plugin-clipboard = pkgs.callPackage ./nix-modules/qti-plugin-clipboard.nix { };
          qti-plugin-screenshot = pkgs.callPackage ./nix-modules/qti-plugin-screenshot.nix { };
          # not included in `qti-all-apps` as it is a dependency, not an app
          qti-app-stdlib = pkgs.callPackage ./nix-modules/qti-app-stdlib.nix { };
          qti-app-screenshot-editor = pkgs.callPackage ./nix-modules/qti-app-screenshot-editor.nix { };
          default = qti;
          qti-all-plugins = [ qti-plugin-core qti-plugin-clipboard qti-plugin-screenshot ];
          qti-all-apps = [ qti-app-screenshot-editor ];
          qti-all = [ qti ] ++ qti-all-plugins ++ qti-all-apps;
        });

      devShells = forEachSystem (system: pkgs: rec {
        default = import ./shell.nix {
          inherit pkgs;
          inherit (self.packages.${system}) quickshell;
        };
      });
    };
}
