#!/usr/bin/env sh
ACTION=$1
shift
case $ACTION in
	clean)
		cd $(dirname "$0")
		rm -rf .cache build plugin/*/.cache plugin/*/build
		cd -;;
	build)
		cd $(dirname "$0")
		mkdir -p build
		cmake -B build -DCMAKE_BUILD_TYPE=RelWithDebInfo -DCMAKE_EXPORT_COMPILE_COMMANDS=ON && cmake --build build
		cd -;;
	run|debug|massif)
		ROOT=$(dirname $(realpath $0))
		for DIR in $(ls -d plugin/*/build/qml_modules/)
		do
			QML2_IMPORT_PATH="$QML2_IMPORT_PATH:$ROOT/$DIR"
		done
		if [ "$ACTION" = "debug" ]
		then
			cd $(dirname "$0")
	  	QML2_IMPORT_PATH="$QML2_IMPORT_PATH" ./gdb_debug.py ./build/qti --path "$@"
			cd -
		elif [ "$ACTION" = "massif" ]
		then
			cd $(dirname "$0")
	  	QML2_IMPORT_PATH="$QML2_IMPORT_PATH" valgrind --tool=massif -- ./build/qti --path "$@"
			cd -
		else
			cd $(dirname "$0")
	  	QML2_IMPORT_PATH="$QML2_IMPORT_PATH" ./build/qti --path "$@"
			cd -
		fi;;
esac
