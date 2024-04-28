#!/usr/bin/env python3
import sys
import subprocess

mode = "launcher"
try:
	import gdb
	mode = "gdb"
except:
	pass

def convert_qstring(val):
	d = val["d"]
	data = d.reinterpret_cast(gdb.lookup_type("char").pointer()) + d["offset"]
	data_len = d["size"] * gdb.lookup_type("unsigned short").sizeof
	return data.string("utf-16", "replace", data_len)

def on_stop(event):
	frame = gdb.newest_frame()
	if not frame.name().startswith("QQmlAbstractBinding::printBindingLoopError"): return
	print("=== Binding loop detected ===")
	i = 0
	while True:
		frame = frame.older()
		if frame == None or not frame.is_valid(): break
		if frame.name().startswith("QQmlBinding::update"):
			current_binding = frame.read_register("rax")
			eval_string = f"(*(QQmlBinding*)({current_binding})).expressionIdentifier()"
			ident = convert_qstring(gdb.parse_and_eval(eval_string))
			print(f"#{i} - {ident}")
			i += 1
	print()
	gdb.execute("continue")

if mode == "gdb":
	gdb.events.stop.connect(on_stop)
	breakpoint = gdb.Breakpoint("QQmlAbstractBinding::printBindingLoopError")
	gdb.execute("run")
else:
	args = sys.argv
	selfName = args.pop(0)
	x = ["gdb", "--command", selfName, "--args"] + args
	subprocess.call(x)
