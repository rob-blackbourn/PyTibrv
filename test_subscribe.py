import sys
sys.path.append(r"..\Debug")
sys.path.append(".")

import tibrv
tibrv.environment.open()
t = tibrv.transport("9650", "pc1226", "pc1226.foo.com:9765")

def cb(send_subject, reply_subject, msg):
	print send_subject, msg
	
subject="TEST.FOO.BAR"
l = tibrv.listener(tibrv.environment.defaultqueue, cb, t, subject, None)

while True:
	print "dispatching"
	try:
		tibrv.environment.defaultqueue.timeddispatch(1)
	except tibrv.TIMEOUT, e:
		print e

print "press return to exit"
sys.__stdin__.readline()