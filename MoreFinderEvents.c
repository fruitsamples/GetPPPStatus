/*	File:		MoreFinderEvents.c	Contains:	Functions to help you build and sending Apple events to the Finder.	Written by: Andy Bachorski		Copyright:	Copyright � 1996-1999 by Apple Computer, Inc., All Rights Reserved.				You may incorporate this Apple sample source code into your program(s) without				restriction. This Apple sample source code has been provided "AS IS" and the				responsibility for its operation is yours. You are not permitted to redistribute				this Apple sample source code as "Apple sample source code" after having made				changes. If you're going to re-distribute the source, we require that you make				it clear in the source that the code was descended from Apple sample source				code, but that you've made changes.	Change History (most recent first):				7/22/1999	Karl Groethe	Updated for Metrowerks Codewarror Pro 2.1				*///	Constant used to #undef PASCAL when not compiling a library#define	COMPILING_MORE_FINDER_EVENTS	true//	System includes#include <Aliases.h>#include <AppleScript.h>#include <Components.h>#include <Folders.h>#include <Icons.h>#include <OSA.h>//	C standard library includes#include <string.h>//	MoreFinderEvents includes#include "FinderRegistry.h"#include "AEHelpers.h"//	Export symbols in this header for shared libraries#pragma export on#include "MoreFinderEvents.h"#pragma export off#define	COMPILING_MORE_FINDER_EVENTS	trueconst	Boolean		flagNotSet = -1;static	Boolean		hasAppleEvents = flagNotSet;static	Boolean		finderCallsAEProcess = flagNotSet;static	Boolean		finderIsOSLCompliant = flagNotSet;// *****************************************************************************pascal OSErr MFESetSelectionToNone( const AEIdleUPP idleProcUPP ){	OSErr		anErr = noErr;		AppleEvent	theEvent = { typeNull, nil };		anErr = AEHMakeAppleEventSignatureTarget( kFinderFileType, kFinderCreatorType,											  kAECoreSuite, kAESetData, &theEvent );	if ( anErr == noErr )	{		AEDesc 		propertyObject = { typeNull, nil };		AEDesc		containerObj = { typeNull, nil };	// start with the null (application) container					anErr = AEHMakePropertyObject( pSelection, &containerObj, &propertyObject );		if ( anErr == noErr )		{			anErr = AEPutParamDesc( &theEvent, keyDirectObject, &propertyObject );			AEDisposeDesc( &propertyObject );			if ( anErr == noErr )			{				AEDescList		emptyList = { typeNull, nil };								anErr = AECreateList( nil, 0, false, &emptyList );				if ( anErr == noErr )				{					anErr = AEPutParamDesc( &theEvent, keyAEData, &emptyList );					if ( anErr == noErr )					{						AppleEvent	theReply = { typeNull, nil };						AESendMode	sendMode;												if ( idleProcUPP == nil )							sendMode = kAENoReply;						else							sendMode = kAEWaitReply;						anErr = AESend( &theEvent, &theReply, sendMode, kAENormalPriority,										kNoTimeOut, idleProcUPP, nil );												AEDisposeDesc( &theEvent );												if ( anErr == noErr && idleProcUPP != nil )						{							anErr =  AEHGetHandlerError( &theReply );														AEDisposeDesc( &theReply );						}					}				}			}		}	}		return anErr;	} // MFESetSelectionToNone// *****************************************************************************pascal OSErr MFEChangeFolderViewNewSuite( const FSSpecPtr fssPtr,										  const long viewStyle,										  const AEIdleUPP idleProcUPP ){	OSErr		anErr = noErr;		AppleEvent	theEvent = {typeNull, nil};		anErr = AEHMakeAppleEventSignatureTarget( kFinderFileType, kFinderCreatorType,											  kAECoreSuite, kAESetData, &theEvent );	if ( anErr == noErr )	{		AEDesc 		folderObject = {typeNull, nil};		AEDesc		containerObj = { typeNull, nil };	// start with the null (application) container				anErr = AEHMakeAliasObjectFromFSSpec( fssPtr, &containerObj, &folderObject );		if ( anErr == noErr )		{			AEDesc 		propertyObject = {typeNull, nil};						anErr = AEHMakePropertyObject( pView, &folderObject, &propertyObject );	// disposes of folderObject for us			if ( anErr == noErr )			{				anErr = AEPutParamDesc( &theEvent, keyDirectObject, &propertyObject );				AEDisposeDesc( &propertyObject );				if ( anErr == noErr )				{					anErr = AEPutParamPtr( &theEvent, keyAEData,										   typeLongInteger, &viewStyle, sizeof(viewStyle) );					if ( anErr == noErr )					{						AppleEvent	theReply = {typeNull, nil};						AESendMode	sendMode;												if ( idleProcUPP == nil )							sendMode = kAENoReply;						else							sendMode = kAEWaitReply;						anErr = AESend( &theEvent, &theReply, sendMode, kAENormalPriority,										kNoTimeOut, idleProcUPP, nil );												AEDisposeDesc( &theEvent );												if ( anErr == noErr && idleProcUPP != nil )						{							anErr =  AEHGetHandlerError( &theReply );														AEDisposeDesc( &theReply );						}					}				}			}		}	}		return anErr;	} // MFEChangeFolderView// *****************************************************************************pascal OSErr MFEChangeFolderViewOldSuite( const FSSpecPtr fssPtr,										  const long viewStyle,										  const AEIdleUPP idleProcUPP ){	OSErr		anErr = noErr;		AppleEvent	theEvent = {typeNull, nil};		anErr = AEHMakeAppleEventSignatureTarget( kFinderFileType, kFinderCreatorType,											  kAEFinderEvents, kAEChangeView, &theEvent );	if ( anErr == noErr )	{		AEDesc	aliasObject = { typeNull, nil };	// start with the null (application) container				anErr = AEHMakeAliasDescFromFSSpec( fssPtr, &aliasObject );				if ( anErr == noErr )		{			anErr = AEPutParamDesc( &theEvent, keyDirectObject, &aliasObject );			AEDisposeDesc( &aliasObject );						if ( anErr == noErr )			{				anErr = AEPutParamPtr( &theEvent, keyMiscellaneous,									   typeLongInteger, &viewStyle, sizeof( viewStyle ) );				if ( anErr == noErr )				{					AppleEvent	theReply = {typeNull, nil};					AESendMode	sendMode;										if ( idleProcUPP == nil )						sendMode = kAENoReply;					else						sendMode = kAEWaitReply;					anErr = AESend( &theEvent, &theReply, sendMode, kAENormalPriority,									kNoTimeOut, idleProcUPP, nil );										AEDisposeDesc( &theEvent );										if ( anErr == noErr && idleProcUPP != nil )					{						anErr =  AEHGetHandlerError( &theReply );												AEDisposeDesc( &theReply );					}				}			}		}	}		return anErr;	} // MFEChangeFolderViewOldSuite// *****************************************************************************pascal OSErr MFEChangeFolderView( const FSSpecPtr fssPtr,								  const long viewStyle,								  const AEIdleUPP idleProcUPP ){	OSErr	anErr = noErr;		if ( FinderCallsAEProcess() )	{		if ( FinderIsOSLCompliant() )		{			anErr = MFEChangeFolderViewNewSuite( fssPtr, viewStyle, idleProcUPP );		}		else		{			anErr = MFEChangeFolderViewOldSuite( fssPtr, viewStyle, idleProcUPP );		}	}	else	{		anErr = errAEEventNotHandled;	}		return anErr;}//end MFEChangeFolderView// *****************************************************************************pascal OSErr MFEAddCustomIconToItem( const FSSpecPtr fssPtr,									 const Handle theIconSuite,									 const IconSelectorValue iconSelector,									 const AEIdleUPP idleProcUPP ){	OSErr		anErr = noErr;		AppleEvent	theEvent = {typeNull, nil};		anErr = AEHMakeAppleEventSignatureTarget( kFinderFileType, kFinderCreatorType,											  kAECoreSuite, kAESetData, &theEvent );	if ( anErr == noErr )	{		AEDesc 		itemObject = {typeNull, nil};		AEDesc		containerObj = { typeNull, nil };		// start with the null (application) container				anErr = AEHMakeAliasObjectFromFSSpec( fssPtr, &containerObj, &itemObject );				if ( anErr == noErr )		{			AEDesc 		propertyObject = {typeNull, nil};						anErr = AEHMakePropertyObject( pIconBitmap, &itemObject, &propertyObject );	// disposes of itemObject for us						if ( anErr == noErr )			{				anErr = AEPutParamDesc( &theEvent, keyDirectObject, &propertyObject );				AEDisposeDesc( &propertyObject );								if ( anErr == noErr )				{					AEDescList	iconFamilyRec = { typeNull, nil };										anErr = AEHMakeIconFamilyRecord( theIconSuite, iconSelector, &iconFamilyRec );					if ( anErr == noErr )					{						anErr = AEPutParamDesc( &theEvent, keyAEData, &iconFamilyRec );												if ( anErr == noErr )						{							AppleEvent	theReply = {typeNull, nil};							AESendMode	sendMode;														if ( idleProcUPP == nil )								sendMode = kAENoReply;							else								sendMode = kAEWaitReply;							anErr = AESend( &theEvent, &theReply, sendMode, kAENormalPriority,											kNoTimeOut, idleProcUPP, nil );														AEDisposeDesc( &theEvent );														if ( anErr == noErr )							{								anErr =  AEHGetHandlerError( &theReply );																AEDisposeDesc( &theReply );							}						}					}				}			}		}	}		return anErr;	} // MFEAddCustomIconToItem// *****************************************************************************pascal OSErr MFEGetItemIcon( const FSSpecPtr fssPtr,							 const AEIdleUPP idleProcUPP,								   Handle	 *theIconSuite ){	OSErr	anErr = noErr;		AppleEvent	theEvent = {typeNull, nil};		anErr = AEHMakeAppleEventSignatureTarget( kFinderFileType, kFinderCreatorType,											  kAECoreSuite, kAEGetData, &theEvent );	if ( anErr == noErr )	{		AEDesc 		itemObject = {typeNull, nil};		AEDesc		containerObj = { typeNull, nil };		// start with the null (application) container				anErr = AEHMakeAliasObjectFromFSSpec( fssPtr, &containerObj, &itemObject );				if ( anErr == noErr )		{			AEDesc 		propertyObject = {typeNull, nil};						anErr = AEHMakePropertyObject( pIconBitmap, &itemObject, &propertyObject );	// disposes of itemObject for us						if ( anErr == noErr )			{				anErr = AEPutParamDesc( &theEvent, keyDirectObject, &propertyObject );				AEDisposeDesc( &propertyObject );								if ( anErr == noErr )				{					AppleEvent	theReply = {typeNull, nil};					AESendMode	sendMode;										if ( idleProcUPP == nil )						sendMode = kAENoReply;					else						sendMode = kAEWaitReply;					anErr = AESend( &theEvent, &theReply, sendMode, kAENormalPriority,									kNoTimeOut, idleProcUPP, nil );										AEDisposeDesc( &theEvent );										if ( anErr == noErr )					{						anErr =  AEHGetHandlerError( &theReply );												if ( anErr == noErr )						{							AEDesc	iconFamilyRec = { typeNull, nil };														anErr = AEGetParamDesc( &theReply, keyDirectObject, typeWildCard, &iconFamilyRec );							AEDisposeDesc( &theReply );														if ( anErr == noErr )							{								anErr = AEHMakeIconSuite( &iconFamilyRec, theIconSuite );							}							AEDisposeDesc( &iconFamilyRec );						}					}				}			}		}	}		return anErr;	} // MFEGetItemIcon// *****************************************************************************pascal OSErr MFEGetEveryItemOnDesktop( const AEIdleUPP idleProcUPP,											 AEDescList *objectList ){	OSErr		anErr = noErr;		AppleEvent	theEvent = {typeNull, nil};		objectList->descriptorType = typeNull;	objectList->dataHandle = nil;		anErr = AEHMakeAppleEventSignatureTarget( kFinderFileType, kFinderCreatorType,											  kAECoreSuite, kAEGetData, &theEvent );	if ( anErr == noErr )	{				AEDesc		containerObj = { typeNull, nil };		// start with the null (application) container		AEDesc 		propertyObject = {typeNull, nil};				anErr = AEHMakePropertyObject( kDesktopFolderType, &containerObj, &propertyObject );				if ( anErr == noErr )		{			DescType	selectAll = kAEAll;			AEDesc		allObjectsDesc = { typeNull, nil };						anErr = AEHMakeSelectionObject( selectAll, &propertyObject, &allObjectsDesc );			if ( anErr == noErr )			{				anErr = AEPutParamDesc( &theEvent, keyDirectObject, &allObjectsDesc );				AEDisposeDesc( &allObjectsDesc );				if ( anErr == noErr )				{					AppleEvent	theReply = {typeNull, nil};					AESendMode	sendMode;										if ( idleProcUPP == nil )						sendMode = kAENoReply;					else						sendMode = kAEWaitReply;					anErr = AESend( &theEvent, &theReply, sendMode, kAENormalPriority,									kNoTimeOut, idleProcUPP, nil );										AEDisposeDesc( &theEvent );										if ( anErr == noErr )					{						anErr =  AEHGetHandlerError( &theReply );												if ( !anErr && theReply.descriptorType != typeNull )						{							anErr = AEGetParamDesc( &theReply, keyDirectObject, typeAEList, objectList );						}						AEDisposeDesc( &theReply );					}				}			}		}	}		return anErr;	} // MFEGetEveryItemOnDesktop// *****************************************************************************pascal OSErr MFEUpdateItemFSS( const FSSpecPtr fssPtr ){	OSErr			anErr = noErr;	AliasHandle		aliasHandle;		anErr = NewAlias( nil, fssPtr, &aliasHandle);	if ( anErr == noErr )	{		anErr = MFEUpdateItemAlias( aliasHandle );	}		DisposeHandle( (Handle)aliasHandle );		return anErr;} // MFEChangeFolderView// *****************************************************************************pascal OSErr MFEUpdateItemAlias( const AliasHandle aliasHandle ){	OSErr		anErr = noErr;		AppleEvent	theEvent = {typeNull, nil};		anErr = AEHMakeAppleEventSignatureTarget( kFinderFileType, kFinderCreatorType,											  kAEFinderSuite, kAEUpdate, &theEvent );	if ( anErr == noErr )	{		AEDesc 		aliasDesc = {typeNull, nil};				anErr = AEHMakeAliasDesc( aliasHandle, &aliasDesc );		if ( anErr == noErr )		{			anErr = AEPutParamDesc( &theEvent, keyDirectObject, &aliasDesc );			AEDisposeDesc( &aliasDesc );			if ( anErr == noErr )			{				AppleEvent	theReply = {typeNull, nil};								anErr = AESend( &theEvent, &theReply, kAENoReply, kAENormalPriority,								kAEDefaultTimeout, nil, nil );				AEDisposeDesc( &theEvent );								if ( anErr == noErr )				{					anErr =  AEHGetHandlerError( &theReply );										AEDisposeDesc( &theReply );				}			}		}	}		return anErr;	} // MFEChangeFolderView// *****************************************************************************pascal OSErr MFEOpenFile( const AEIdleUPP idleProcUPP,								FSSpec *fssPtr ){	OSErr		anErr = noErr;		AppleEvent	theEvent = { typeNull, nil };		anErr = AEHMakeAppleEventSignatureTarget( kFinderFileType, kFinderCreatorType,											  kAECoreSuite, kAEOpen, &theEvent );	if ( anErr == noErr )	{		anErr = AEPutParamPtr( &theEvent, keyDirectObject, typeFSS, fssPtr, sizeof( FSSpec ) );				if ( anErr == noErr )		{				AppleEvent	theReply = {typeNull, nil};			AESendMode	sendMode;						if ( idleProcUPP == nil )				sendMode = kAENoReply;			else				sendMode = kAEWaitReply;			anErr = AESend( &theEvent, &theReply, sendMode, kAENormalPriority,							kNoTimeOut, idleProcUPP, nil );						AEDisposeDesc( &theEvent );						if ( anErr == noErr )			{						anErr =  AEHGetHandlerError( &theReply );				AEDisposeDesc( &theReply );			}		}	}		return anErr;}//end MFELaunchApplication