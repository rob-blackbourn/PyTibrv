#include "stdafx.h"

// If you have to ask you're not old enough
extern "C" { FILE _iob[3] = {__iob_func()[0], __iob_func()[1], __iob_func()[2]}; }