/*
 * DDEML library
 *
 * Copyright 1997 Alexandre Julliard
 * Copyright 1997 Len White
 */

/* Only empty stubs for now */

#include <stdio.h>
#include "ddeml.h"
#include "debug.h"

/* FIXME: What are these values? */
#define DMLERR_NO_ERROR		0

static LONG     DDE_current_handle;


/******************************************************************************
 *            DdeInitialize16   (DDEML.2)
 */
UINT16 WINAPI DdeInitialize16( LPDWORD pidInst, PFNCALLBACK16 pfnCallback,
                               DWORD afCmd, DWORD ulRes)
{
    return (UINT16)DdeInitialize32A(pidInst,(PFNCALLBACK32)pfnCallback,
                                    afCmd, ulRes);
}


/******************************************************************************
 *            DdeInitialize32A   (USER32.106)
 */
UINT32 WINAPI DdeInitialize32A( LPDWORD pidInst, PFNCALLBACK32 pfnCallback,
                                DWORD afCmd, DWORD ulRes )
{
    return DdeInitialize32W(pidInst,pfnCallback,afCmd,ulRes);
}


/******************************************************************************
 * DdeInitialize32W [USER32.107]
 * Registers an application with the DDEML
 *
 * PARAMS
 *    pidInst     [I] Pointer to instance identifier
 *    pfnCallback [I] Pointer to callback function
 *    afCmd       [I] Set of command and filter flags
 *    ulRes       [I] Reserved
 *
 * RETURNS
 *    Success: DMLERR_NO_ERROR
 *    Failure: DMLERR_DLL_USAGE, DMLERR_INVALIDPARAMETER, DMLERR_SYS_ERROR
 */
UINT32 WINAPI DdeInitialize32W( LPDWORD pidInst, PFNCALLBACK32 pfnCallback,
                                DWORD afCmd, DWORD ulRes )
{
    FIXME(ddeml, "(%p,%p,%ld,%ld): stub\n",pidInst,pfnCallback,afCmd,ulRes);
    return DMLERR_NO_ERROR;
}


/*****************************************************************
 *            DdeUninitialize16   (DDEML.3)
 */
BOOL16 WINAPI DdeUninitialize16( DWORD idInst )
{
    return (BOOL16)DdeUninitialize32( idInst );
}


/*****************************************************************
 * DdeUninitialize32 [USER32.119]  Frees DDEML resources
 *
 * PARAMS
 *    idInst [I] Instance identifier
 *
 * RETURNS
 *    Success: TRUE
 *    Failure: FALSE
 */
BOOL32 WINAPI DdeUninitialize32( DWORD idInst )
{
    FIXME(ddeml, "(%ld): stub\n", idInst);
    return TRUE;
}


/*****************************************************************
 * DdeConnectList16 [DDEML.4]
 */
HCONVLIST WINAPI DdeConnectList16( DWORD idInst, HSZ hszService, HSZ hszTopic,
                 HCONVLIST hConvList, LPCONVCONTEXT16 pCC )
{
    return DdeConnectList32(idInst, hszService, hszTopic, hConvList, 
                            (LPCONVCONTEXT32)pCC);
}


/******************************************************************************
 * DdeConnectList32 [USER32.93]  Establishes conversation with DDE servers
 *
 * PARAMS
 *    idInst     [I] Instance identifier
 *    hszService [I] Handle to service name string
 *    hszTopic   [I] Handle to topic name string
 *    hConvList  [I] Handle to conversation list
 *    pCC        [I] Pointer to structure with context data
 *
 * RETURNS
 *    Success: Handle to new conversation list
 *    Failure: 0
 */
HCONVLIST WINAPI DdeConnectList32( DWORD idInst, HSZ hszService, HSZ hszTopic,
                 HCONVLIST hConvList, LPCONVCONTEXT32 pCC )
{
    FIXME(ddeml, "(%ld,%ld,%ld,%ld,%p): stub\n", idInst, hszService, hszTopic,
          hConvList,pCC);
    return 1;
}


/*****************************************************************
 * DdeQueryNextServer16 [DDEML.5]
 */
HCONV WINAPI DdeQueryNextServer16( HCONVLIST hConvList, HCONV hConvPrev )
{
    return DdeQueryNextServer32(hConvList, hConvPrev);
}


/*****************************************************************
 * DdeQueryNextServer32 [USER32.112]
 */
HCONV WINAPI DdeQueryNextServer32( HCONVLIST hConvList, HCONV hConvPrev )
{
    FIXME(ddeml, "(%ld,%ld): stub\n",hConvList,hConvPrev);
    return 0;
}


/*****************************************************************
 *            DdeDisconnectList (DDEML.6)
 */
BOOL16 WINAPI DdeDisconnectList16( HCONVLIST hConvList )
{
    return (BOOL16)DdeDisconnectList32(hConvList);
}


/******************************************************************************
 * DdeDisconnectList32 [USER32.98]  Destroys list and terminates conversations
 *
 * RETURNS
 *    Success: TRUE
 *    Failure: FALSE
 */
BOOL32 WINAPI DdeDisconnectList32(
    HCONVLIST hConvList) /* [in] Handle to conversation list */
{
    FIXME(ddeml, "(%ld): stub\n", hConvList);
    return TRUE;
}


/*****************************************************************
 *            DdeConnect16   (DDEML.7)
 */
HCONV WINAPI DdeConnect16( DWORD idInst, HSZ hszService, HSZ hszTopic,
                           LPCONVCONTEXT16 pCC )
{
    FIXME( ddeml, "empty stub\n" );
    return 0;
}


/*****************************************************************
 *            DdeConnect32   (USER32.92)
 */
HCONV WINAPI DdeConnect32( DWORD idInst, HSZ hszService, HSZ hszTopic,
                           LPCONVCONTEXT32 pCC )
{
    FIXME( ddeml, "(...): stub\n");
    return 0;
}


/*****************************************************************
 *            DdeDisconnect16   (DDEML.8)
 */
BOOL16 WINAPI DdeDisconnect16( HCONV hConv )
{
    return (BOOL16)DdeDisconnect32( hConv );
}

/*****************************************************************
 *            DdeSetUserHandle (DDEML.10)
 */
BOOL16 WINAPI DdeSetUserHandle( HCONV hConv, DWORD id, DWORD hUser )
{
    FIXME( ddeml, "empty stub\n" );
    return 0;
}

/*****************************************************************
 *            DdeCreateDataHandle (DDEML.14)
 */
HDDEDATA WINAPI DdeCreateDataHandle( DWORD idInst, LPBYTE pSrc, DWORD cb, DWORD cbOff, HSZ hszItem, UINT16 wFmt, UINT16 afCmd )
{
    FIXME( ddeml, "empty stub\n" );
    return 0;
}

/*****************************************************************
 *            DdeDisconnect32   (USER32.97)
 */
BOOL32 WINAPI DdeDisconnect32( HCONV hConv )
{
    FIXME( ddeml, "empty stub\n" );
    return 0;
}


/*****************************************************************
 *            DdeReconnect   (DDEML.37) (USER32.115)
 */
HCONV WINAPI DdeReconnect( HCONV hConv )
{
    FIXME( ddeml, "empty stub\n" );
    return 0;
}


/*****************************************************************
 *            DdeCreateStringHandle16   (DDEML.21)
 */
HSZ WINAPI DdeCreateStringHandle16( DWORD idInst, LPCSTR str, INT16 codepage )
{
    return DdeCreateStringHandle32A( idInst, str, codepage );
}


/*****************************************************************
 * DdeCreateStringHandle32A [USER32.95]
 */
HSZ WINAPI DdeCreateStringHandle32A( DWORD idInst, LPCSTR psz, INT32 codepage )
{
    FIXME(ddeml, "(...): stub\n" );
    DDE_current_handle++;
    return DDE_current_handle;
}


/******************************************************************************
 * DdeCreateStringHandle32W [USER32.96]  Creates handle to identify string
 *
 * RETURNS
 *    Success: String handle
 *    Failure: 0
 */
HSZ WINAPI DdeCreateStringHandle32W(
    DWORD idInst,   /* [in] Instance identifier */
    LPCWSTR psz,    /* [in] Pointer to string */
    INT32 codepage) /* [in] Code page identifier */
{
    FIXME(ddeml, "(%ld,%s,%d): stub\n",idInst,debugstr_w(psz),codepage);
    DDE_current_handle++;
    return DDE_current_handle;
}


/*****************************************************************
 *            DdeFreeStringHandle16   (DDEML.22)
 */
BOOL16 WINAPI DdeFreeStringHandle16( DWORD idInst, HSZ hsz )
{
    return (BOOL32)DdeFreeStringHandle32( idInst, hsz );
}


/*****************************************************************
 *            DdeFreeStringHandle32   (USER32.101)
 */
BOOL32 WINAPI DdeFreeStringHandle32( DWORD idInst, HSZ hsz )
{
    FIXME( ddeml, "(%d,%d): stub\n",idInst, hsz );
    return TRUE;
}


/*****************************************************************
 *            DdeFreeDataHandle16   (DDEML.19)
 */
BOOL16 WINAPI DdeFreeDataHandle16( HDDEDATA hData )
{
    return (BOOL32)DdeFreeDataHandle32( hData );
}


/*****************************************************************
 *            DdeFreeDataHandle32   (USER32.100)
 */
BOOL32 WINAPI DdeFreeDataHandle32( HDDEDATA hData )
{
    FIXME( ddeml, "empty stub\n" );
    return TRUE;
}


/*****************************************************************
 *            DdeKeepStringHandle16   (DDEML.24)
 */
BOOL16 WINAPI DdeKeepStringHandle16( DWORD idInst, HSZ hsz )
{
    return (BOOL32)DdeKeepStringHandle32( idInst, hsz );
}


/*****************************************************************
 *            DdeKeepStringHandle32  (USER32.108)
 */
BOOL32 WINAPI DdeKeepStringHandle32( DWORD idInst, HSZ hsz )
{
    FIXME( ddeml, "empty stub\n" );
    return TRUE;
}


/*****************************************************************
 *            DdeClientTransaction16  (DDEML.11)
 */
HDDEDATA WINAPI DdeClientTransaction16( LPVOID pData, DWORD cbData,
                                        HCONV hConv, HSZ hszItem, UINT16 wFmt,
                                        UINT16 wType, DWORD dwTimeout,
                                        LPDWORD pdwResult )
{
    return DdeClientTransaction32( (LPBYTE)pData, cbData, hConv, hszItem,
                                   wFmt, wType, dwTimeout, pdwResult );
}


/*****************************************************************
 *            DdeClientTransaction32  (USER32.90)
 */
HDDEDATA WINAPI DdeClientTransaction32( LPBYTE pData, DWORD cbData,
                                        HCONV hConv, HSZ hszItem, UINT32 wFmt,
                                        UINT32 wType, DWORD dwTimeout,
                                        LPDWORD pdwResult )
{
    FIXME( ddeml, "empty stub\n" );
    return 0;
}

/*****************************************************************
 *            DdeAbandonTransaction (DDEML.12)
 */
BOOL16 WINAPI DdeAbandonTransaction( DWORD idInst, HCONV hConv, 
                                     DWORD idTransaction )
{
    FIXME( ddeml, "empty stub\n" );
    return 0;
}


/*****************************************************************
 * DdePostAdvise16 [DDEML.13]
 */
BOOL16 WINAPI DdePostAdvise16( DWORD idInst, HSZ hszTopic, HSZ hszItem )
{
    return (BOOL16)DdePostAdvise32(idInst, hszTopic, hszItem);
}


/******************************************************************************
 * DdePostAdvise32 [USER32.110]  Send transaction to DDE callback function.
 *
 * RETURNS
 *    Success: TRUE
 *    Failure: FALSE
 */
BOOL32 WINAPI DdePostAdvise32(
    DWORD idInst, /* [in] Instance identifier */
    HSZ hszTopic, /* [in] Handle to topic name string */
    HSZ hszItem)  /* [in] Handle to item name string */
{
    FIXME(ddeml, "(%ld,%ld,%ld): stub\n",idInst,hszTopic,hszItem);
    return TRUE;
}


/*****************************************************************
 *            DdeAddData (DDEML.15)
 */
HDDEDATA WINAPI DdeAddData( HDDEDATA hData, LPBYTE pSrc, DWORD cb,
                            DWORD cbOff )
{
    FIXME( ddeml, "empty stub\n" );
    return 0;
}


/******************************************************************************
 * DdeGetData32 [USER32.102]  Copies data from DDE object ot local buffer
 *
 * RETURNS
 *    Size of memory object associated with handle
 */
DWORD WINAPI DdeGetData32(
    HDDEDATA hData, /* [in] Handle to DDE object */
    LPBYTE pDst,    /* [in] Pointer to destination buffer */
    DWORD cbMax,    /* [in] Amount of data to copy */
    DWORD cbOff)    /* [in] Offset to beginning of data */
{
    FIXME(ddeml, "(%ld,%p,%ld,%ld): stub\n",hData,pDst,cbMax,cbOff);
    return cbMax;
}


/*****************************************************************
 * DdeGetData16 [DDEML.16]
 */
DWORD WINAPI DdeGetData16(
    HDDEDATA hData,
    LPBYTE pDst,
    DWORD cbMax, 
    DWORD cbOff)
{
    return DdeGetData32(hData, pDst, cbMax, cbOff);
}


/*****************************************************************
 *            DdeAccessData (DDEML.17)
 */
LPBYTE WINAPI DdeAccessData( HDDEDATA hData, LPDWORD pcbDataSize )
{
     FIXME( ddeml, "empty stub\n" );
     return 0;
}

/*****************************************************************
 *            DdeUnaccessData (DDEML.18)
 */
BOOL16 WINAPI DdeUnaccessData( HDDEDATA hData )
{
     FIXME( ddeml, "empty stub\n" );
     return 0;
}

/*****************************************************************
 *            DdeEnableCallback (DDEML.26)
 */
BOOL16 WINAPI DdeEnableCallback( DWORD idInst, HCONV hConv, UINT16 wCmd )
{
     FIXME( ddeml, "empty stub\n" );
     return 0;
}

/*****************************************************************
 *            DdeNameService16  (DDEML.27)
 */
HDDEDATA WINAPI DdeNameService16( DWORD idInst, HSZ hsz1, HSZ hsz2,
                                  UINT16 afCmd )
{
    return DdeNameService32( idInst, hsz1, hsz2, afCmd );
}


/******************************************************************************
 * DdeNameService32 [USER32.109]  {Un}registers service name of DDE server
 *
 * PARAMS
 *    idInst [I] Instance identifier
 *    hsz1   [I] Handle to service name string
 *    hsz2   [I] Reserved
 *    afCmd  [I] Service name flags
 *
 * RETURNS
 *    Success: Non-zero
 *    Failure: 0
 */
HDDEDATA WINAPI DdeNameService32( DWORD idInst, HSZ hsz1, HSZ hsz2,
                UINT32 afCmd )
{
    FIXME(ddeml, "(%ld,%ld,%ld,%d): stub\n",idInst,hsz1,hsz2,afCmd);
    return 1;
}


/*****************************************************************
 *            DdeGetLastError16  (DDEML.20)
 */
UINT16 WINAPI DdeGetLastError16( DWORD idInst )
{
    return (UINT16)DdeGetLastError32( idInst );
}


/******************************************************************************
 * DdeGetLastError32 [USER32.103]  Gets most recent error code
 *
 * PARAMS
 *    idInst [I] Instance identifier
 *
 * RETURNS
 *    Last error code
 */
UINT32 WINAPI DdeGetLastError32( DWORD idInst )
{
    FIXME(ddeml, "(%ld): stub\n",idInst);
    return 0;
}


/*****************************************************************
 *            DdeCmpStringHandles (DDEML.36)
 */
int WINAPI DdeCmpStringHandles( HSZ hsz1, HSZ hsz2 )
{
     FIXME( ddeml, "empty stub\n" );
     return 0;
}

