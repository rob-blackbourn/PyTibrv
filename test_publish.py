import sys
sys.path.append(r"..\Debug")
sys.path.append(".")

import tibrv
tibrv.environment.open()

t = tibrv.transport("0568", "vldntstapp07", "vldntstapp07.foo.com:7500")

import time
subject="TEST.FOO.BAR"
n = 1
while True:
	msg = {"COUNT":n}
	t.send(subject, msg)
	print subject, msg
	time.sleep(5)
	n = n + 1

print "press return to exit"
sys.__stdin__.readline()