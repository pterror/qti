{ lib
, pkgs
, stdenv
, qt6
, gitRev ? (
    let
      headExists = builtins.pathExists ./.git/HEAD;
      headContent = builtins.readFile ./.git/HEAD;
    in
    if headExists
    then
      (
        let
          matches = builtins.match "ref: refs/heads/(.*)\n" headContent;
        in
        if matches != null
        then builtins.readFile ./.git/refs/heads/${builtins.elemAt matches 0}
        else headContent
      )
    else "unknown"
  )
, isStdlib ? false
}: args: stdenv.mkDerivation (args // rec {
  pname = "qti-app-${args.pname}";
  src = ../app + "/${args.pname}";
  version = args.version or " 0.0 .1 ";
  buildInputs =
    (args.buildInputs or [ ]) ++ (if isStdlib then [ ] else [
      (pkgs.callPackage ./qti-plugin-stdlib.nix { })
    ]);
  installPhase =
    if isStdlib then
      ''
        mkdir -p $out/${qt6.qtbase.qtQmlPrefix}/Qti/Stdlib
        mv * $out/${qt6.qtbase.qtQmlPrefix}/Qti/Stdlib
      '' else
      ''
        mkdir -p $out/share/qti/${args.pname}
        mv * $out/share/qti/${args.pname}
      '';
  meta = with lib; {
    homepage = "https://github.com/pterror/qti/tree/master/app/${args.pname}";
    description = "Qt interpreter";
    license = licenses.isc;
    platforms = platforms.linux;
  } // (args.meta or { });
})

