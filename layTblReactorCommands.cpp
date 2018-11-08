#include "StdAfx.h"
#include "StdArx.h"

// extern AcDbObjectId acedGetCurViewportObjectId();

AcDbObjectIdArray g_layerIdArray;
bool g_bWatchOn = true;

bool g_bLayersOff;
bool g_bLayersLocked;
bool g_bLayersFrozen;
bool g_bLayersFrozenVP;
bool g_bLayersFrozenVPNew;


class AsdkDbReactor : public AcDbDatabaseReactor
{
public:
	virtual void objectAppended(const AcDbDatabase* pDb, const AcDbObject * pObj)
	{
		static const AcDbObject * pPrevObj = NULL;

		Acad::ErrorStatus es;
		if (pPrevObj != pObj)
		{
			pPrevObj = pObj;
			if (g_bWatchOn && pObj->isA() == AcDbLayerTableRecord::desc())
			{
				acDocManager->disableDocumentActivation();
				AcDbLayerTableRecord * pLayerRecord = (AcDbLayerTableRecord *)pObj;
				//LPCTSTR pcszLayerName;
				//pLayerRecord->getName(pcszLayerName);
				es = pLayerRecord->upgradeOpen();

				pLayerRecord->setIsOff(g_bLayersOff);
				pLayerRecord->setIsLocked(g_bLayersLocked);
				pLayerRecord->setIsFrozen(g_bLayersFrozen);
				pLayerRecord->setVPDFLT(g_bLayersFrozenVPNew);

				es = pLayerRecord->downgradeOpen();
				g_layerIdArray.append(pLayerRecord->objectId());
				acDocManager->enableDocumentActivation();
			}
		}
	}
};

void clearLayerIdArray(void)
{
	g_layerIdArray.removeAll();
	//while (!g_layerIdArray.isEmpty())
	//{
	//	g_layerIdArray.removeLast();
	//}
}

void UpdateViewPorts(void)
{
	if (!g_bLayersFrozenVP || g_layerIdArray.isEmpty())
		return;

	ads_name ss;
	resbuf *rb = ads_buildlist(RTDXF0, _T("viewport"), 0);
	if (acedSSGet(_T("X"), NULL, NULL, rb, ss) == RTNORM)
	{
		Adesk::Int32 nCount;
		Acad::ErrorStatus es;
		acedSSLength(ss, &nCount);
		AcDbObjectId vpId = acedGetCurViewportObjectId();
		for (int i = 0; i < nCount; i++)
		{
			ads_name entName;
			if (acedSSName(ss, i, entName) == RTNORM)
			{
				AcDbObjectId eId;
				acdbGetObjectId(eId, entName);
				if (vpId != eId)
				{
					AcDbViewport *pViewport;
					es = acdbOpenAcDbEntity((AcDbEntity*&)pViewport, eId, AcDb::kForRead);
					if (es == Acad::eOk)
					{
						if (pViewport->number() != 1)
						{
							es = pViewport->upgradeOpen();
							pViewport->freezeLayersInViewport(g_layerIdArray);
						}
						pViewport->close();
					}
				}
			}
		}
		acedSSFree(ss);
	}
	acutRelRb(rb);
	clearLayerIdArray();
}

class AeditorReactor : public AcEditorReactor
{
public:
	virtual void commandEnded(const TCHAR *pcszCommand)
	{
		UpdateViewPorts();
	}
};


//AsdkDbReactor * gpDbR = NULL;		// pointer to database reactor
//AeditorReactor * gpEdr = NULL;		// pointer to editor reactor
//long gEntCount = 0;					// global entity count


void watchDb(void)
{
	AsdkDbReactor ** pDbr = &DocVars.docData().m_pDbr;
	if (!*pDbr)

	{
		*pDbr = new AsdkDbReactor();
		acdbHostApplicationServices()->workingDatabase()->addReactor(*pDbr);
	}

	AeditorReactor ** pEdr = &DocVars.docData().m_pEdr;
	if (!*pEdr)
	{
		*pEdr = new AeditorReactor();
		acedEditor->addReactor(*pEdr);
	}
	
	acutPrintf(_T("\nlayTblReactor initialized and working in current drawing, command DDLAYTBLREACTOR available"));
}


void clearReactors(void)
{
	clearLayerIdArray();
	AsdkDbReactor ** pDbr = &DocVars.docData().m_pDbr;
	if (acdbHostApplicationServices()->workingDatabase())
	{
		acdbHostApplicationServices()->workingDatabase()->removeReactor(*pDbr);
		delete *pDbr;
		*pDbr = NULL;
	}

	AeditorReactor ** pEdr = &DocVars.docData().m_pEdr;
	if (*pEdr)
	{
		acedEditor->removeReactor(*pEdr);
		delete *pEdr;
		*pEdr = NULL;
	}
}

void rpklayTblReactorOn(void)
{
	g_bWatchOn = true;
}

void rpklayTblReactorOff(void)
{
	g_bWatchOn = false;
}