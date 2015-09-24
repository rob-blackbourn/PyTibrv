import sys
sys.path.append(r"..\Debug")
sys.path.append(".")

import tibrv
tibrv.environment.open()

#t = tibrv.transport("9650", "pc1226", "pc1226.cqsm.com:9765")
#t = tibrv.transport("8650", ";239.255.210.1", "qsvr02.cqsm.com:7500")
t = tibrv.transport("0568", "vldntstapp07", "vldntstapp07.cqsm.com:7500")

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