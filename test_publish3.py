import sys
sys.path.append(r"..\Debug")
sys.path.append(".")

import tibrv
tibrv.environment.open()
t = tibrv.transport("8650", "pc391", "pc391.foo.com:7500")

subject = "DATAEXP.VOD LN"

message = {
	"MSG_SRC"				: "GENPUB",
	"MSG_TYPE"				: "DATAEXP",
	"EQUITY_TICKER"			: "VOD LN",
	"EQUITY_NAME"			: "VODAFONE GROUP PLC",
	"ISSUER_TICKER"			: "VOD LN",
	"ISSUER_NAME"			: "Vodafone Group PLC",
	"LENDERS"				: 29 ,
	"AVAILABLE"				: 37420270302.427795 ,
	"ON_LOAN"				: 10705819494.514700,
	"USAGE_RATIO"			: 0.286097 ,
	"BORROWERS"				: 8,
	"BORROWERS_FROM_INV"	: 6,
	"BORROWERS_FROM_MKT"	: 1,
	"ON_LOAN_FROM_INV"		: 1142873046.081000,
	"ON_LOAN_FROM_MKT"		: 9562946448.433701,
	"USAGE_RATIO_FROM_INV"	: 0.030542,
	"PCT_MKT_CAP"			: 0.062755,
	"LAST_UPDATE"			: (2007,06,18,18,02,00)}

t.send(subject, message)


print "press return to exit"
sys.__stdin__.readline()