{
  inputs.nixpkgs.url = "nixpkgs/nixos-unstable";
  outputs = { self, nixpkgs }: let
    forEachSystem = fn: nixpkgs.lib.genAttrs
      nixpkgs.lib.systems.flakeExposed
      (system: fn system nixpkgs.legacyPackages.${system});
  in {
    packages = forEachSystem (system: pkgs: rec {
      qti = pkgs.callPackage ./default.nix {
        gitRev = self.rev or self.dirtyRev;
      };
      default = qti;
    });

    devShells = forEachSystem (system: pkgs: rec {
      default = import ./shell.nix {
        inherit pkgs;
        inherit (self.packages.${system}) quickshell;
      };
    });
  };
}
