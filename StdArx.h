#pragma once

#include "acdb.h"
#include "rxregsvc.h"
#include "dbapserv.h"
#include "aced.h"
#include "acdocman.h"
#include "dbhandle.h"
#include "acaplmgr.h"
#include "dbents.h"
#include "dbmain.h"


void watchDb(void);
void clearReactors(void);
AcadApp::ErrorStatus getAppInfo(AcadAppInfo & info);
AcadApp::ErrorStatus RegisterArxApp(void);
AcadApp::ErrorStatus UnregisterArxApp();
AcadApp::ErrorStatus UnregisterArxApp(AcadAppInfo & info);
bool WriteLayerStateSettings(void);
void clearReactors(void);

extern AcApDataManager<CDocData> DocVars;