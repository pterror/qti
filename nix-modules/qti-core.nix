{ qtModule
, qtbase
, libwebp
, jasper
, libmng
, zlib
, pkg-config
, lib
, stdenv
}:

qtModule {
  pname = "qti-core";
  propagatedBuildInputs = [ qtbase ];
  buildInputs = [ ];
  nativeBuildInputs = [ pkg-config ];
}
