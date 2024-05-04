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
  pname = "qti-clipboard";
  propagatedBuildInputs = [ qtbase ];
  buildInputs = [ ];
  nativeBuildInputs = [ pkg-config ];
}
