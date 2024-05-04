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
          qti-core = pkgs.callPackage ./nix-modules/qti-core.nix { };
          qti-clipboard = pkgs.callPackage ./nix-modules/qti-clipboard.nix { };
          qti-screenshot = pkgs.callPackage ./nix-modules/qti-screenshot.nix { };
          default = qti;
          all = [ qti qti-core qti-clipboard qti-screenshot ];
        });

      devShells = forEachSystem (system: pkgs: rec {
        default = import ./shell.nix {
          inherit pkgs;
          inherit (self.packages.${system}) quickshell;
        };
      });
    };
}
