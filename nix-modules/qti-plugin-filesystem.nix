{ pkgs }:
(pkgs.callPackage ./plugin-module.nix { }) {
	pname = "filesystem";
	src = ../plugin/filesystem;
	meta = {
		homepage = "https://github.com/pterror/qti/tree/master/plugin/filesystem";
		description = "Filesystem plugin for Qt interpreter";
	};
}
