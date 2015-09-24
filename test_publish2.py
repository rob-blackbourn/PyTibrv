import sys
sys.path.append(r"..\Debug")
sys.path.append(".")

import tibrv
tibrv.environment.open()
t = tibrv.transport("8650", ";239.255.210.1", "qsvr02.foo.com:7500")

subject='TEST.LIST1'
msg = {'MSG_TYPE':'LIST', 'COUNT':4,'ITEM_1':'the', 'ITEM_2':'quick', 'ITEM_3':'brown', 'ITEM_4':'fox'}
t.send(subject, msg)
print subject, msg

subject='TEST.LIST2'
#msg = {'MSG_TYPE':'LIST', 'COUNT':4,'ITEM_1':'FOO.CDSCRV.LATEST.VOD LN', 'ITEM_2':'FOO.CDSCRV.LATEST.BAY GR', 'ITEM_3':'FOO.CDSCRV.LATEST.SBRY LN', 'ITEM_4':'FOO.CDSCRV.LATEST.TSCO LN'}
msg = {'MSG_TYPE':'LIST', 'COUNT':5,'ITEM_1':'FOO.CDSCRV.LATEST.BAY GR', 'ITEM_2':'FOO.CDSCRV.LATEST.VOD LN', 'ITEM_3':'FOO.CDSCRV.LATEST.SBRY LN', 'ITEM_4':'FOO.CDSCRV.LATEST.TSCO LN', 'ITEM_5':'FOO.CDSCRV.LATEST.TIT IM'}
t.send(subject, msg)
print subject, msg

subject='TEST.GRID2'
#msg = {'MSG_TYPE':'GRID', 'ROWS':2, 'COLS':2,'ITEM_1_1':'21-May-2007', 'ITEM_1_2':51.2, 'ITEM_2_1':'21-Mar-2007', 'ITEM_2_2':'55.7'}
msg = {'MSG_TYPE':'GRID', 'ROWS':2, 'COLS':2,'ITEM_1_1':(2007, 5, 21), 'ITEM_1_2':51.2, 'ITEM_2_1':(2007, 5, 22), 'ITEM_2_2':55.7}
t.send(subject, msg)
print subject, msg


print "press return to exit"
sys.__stdin__.readline()