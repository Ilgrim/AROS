/*
    (C) 1995-97 AROS - The Amiga Research OS
    $Id$

    Desc:
    Lang: english
*/

#include <proto/exec.h>
#include <proto/intuition.h>
#include <proto/utility.h>
#include <exec/memory.h>
#include <intuition/classes.h>
#include <aros/asmcall.h>
#include <gadgets/aroslist.h>
#include <gadgets/aroslistview.h>
#include <string.h>

#include "dirlist.h"
#include "asl_intern.h"

#ifndef TURN_OFF_DEBUG
#define DEBUG 1
#endif

#include <aros/debug.h>

#undef AslBase
#define AslBase (GetSDLD(cl)->sd_AslBase)

STATIC IPTR dirlist_set(struct IClass *cl, Object *o, struct opSet *msg)
{
    struct TagItem *tag, *tstate;
    IPTR retval = 0UL;
    struct DirListData *data = (struct DirListData *)INST_DATA(cl, o);
     
    tstate = msg->ops_AttrList;
    while ((tag = NextTagItem(&tstate)) != NULL);
    {
    	IPTR tidata = tag->ti_Data;
    	
    	switch (tag->ti_Tag)
    	{
    	    case AROSA_DirList_Path:
    	    	data->dld_CurPath[0] = 0;
    	    	UpdateFileList(cl, o, msg->ops_GInfo, (STRPTR)tidata, ASLB(AslBase));
    	    	retval = 1UL;
    	    	break;
    	
    	default:
    	    break;

    	}
    }

    return (retval);
}

/*********************
**  DirList::Get()  **
*********************/

STATIC IPTR dirlist_get(struct IClass *cl, Object *o, struct opGet *msg)
{
    IPTR retval = 1UL;
    
    struct DirListData *data = (struct DirListData *)INST_DATA(cl, o);
    
    switch (msg->opg_AttrID)
    {
    case AROSA_DirList_VolumesShown:
    	*(msg->opg_Storage) = ((data->dld_Flags & DLFLG_FILELIST) == 0);
    	break;
    
    default:
    	retval = DoSuperMethodA(cl, o, (Msg)msg);
    	break;
    }
    return (retval);
}

/*********************
**  DirList::New()  **
*********************/

STATIC Object *dirlist_new(struct IClass *cl, Object *o, struct opSet *msg)
{
    struct DirListData *data;
    struct opSet ops;
    struct TagItem tags[] =
    {
    	{AROSA_Listview_MaxColumns,	(IPTR)2},
    	{AROSA_Listview_DisplayHook,	(IPTR)&(GetSDLD(cl)->sd_FileDisplayHook)},
    	{AROSA_Listview_Format,		(IPTR)"P=l,P=l"},
    	{TAG_MORE, 			(IPTR)msg->ops_AttrList}
    };
    
    ops.MethodID	= OM_NEW;
    ops.ops_AttrList	= tags;
    ops.ops_GInfo	= NULL; 

    o = (Object *)DoSuperMethodA(cl, o, (Msg)&ops);
    if (!o)
        return (NULL);

    data = (struct DirListData *)INST_DATA(cl, o);
    
    memset(data, 0, sizeof (struct DirListData));

    /* Create a empty files and volumes list objects */
    data->dld_FileList = NewObject(NULL, AROSLISTCLASS,
    	AROSA_List_ConstructHook, &(GetSDLD(cl)->sd_FileConstructHook),
    	AROSA_List_DestructHook,  &(GetSDLD(cl)->sd_FileDestructHook),
    	TAG_END);
    if (!data->dld_FileList)
    	goto failure;

    data->dld_VolumesList = NewObject(NULL, AROSLISTCLASS,
    	AROSA_List_ConstructHook, &(GetSDLD(cl)->sd_VolConstructHook),
    	AROSA_List_DestructHook,  &(GetSDLD(cl)->sd_VolDestructHook),
    	TAG_END);
    if (!data->dld_VolumesList)
    	goto failure;

    data->dld_Flags |= DLFLG_FILELIST;
    
    SetSuperAttrs(cl, o, AROSA_Listview_List, data->dld_FileList, TAG_END);

    /* Get the volumes list */
    if (!GetVolumes(data->dld_VolumesList, ASLB(AslBase)))
    	goto failure;
    	    	
    dirlist_set(cl, o, msg);
    
    /* Current path set ? If not set a default one */
    if (!data->dld_CurPath)
    {
    	if (!AddToPath("Sys:",
    		&(data->dld_CurPath),
    		&(data->dld_PathBufSize),
    		AslBase))
    	    goto failure;

    	/* Insert into file list */
    }
    if (!GetDir(data->dld_CurPath, data->dld_FileList, ASLB(AslBase)))
	goto failure;
    
    return (o);
     	
failure:
    DisposeObject(o);
    return (NULL);
}

/*************************
**  DirList::Dispose()  **
*************************/

STATIC VOID dirlist_dispose(struct IClass *cl, Object *o, Msg msg)
{
    struct DirListData *data = INST_DATA(cl, o);
    
    if (data->dld_CurPath)
    	FreeMem(data->dld_CurPath, data->dld_PathBufSize);
    	
    if (data->dld_FileList)
    	DisposeObject(data->dld_FileList);

    if (data->dld_VolumesList)
    	DisposeObject(data->dld_VolumesList);
    	
    DoSuperMethodA(cl, o, msg);
    
    return;
}

/*****************************
**  DirList::SingleClick()  **
*****************************/
STATIC VOID dirlist_singleclick(Class *cl, Object *o, struct AROSP_Listview_SingleClick *msg)
{
    struct DirListData *data = INST_DATA(cl, o);
	
    /* Is volumelist currently shown ? If so only a single click should
    ** cause a change of directory.
    */
    

    if (!(data->dld_Flags & DLFLG_FILELIST))
    {
    	struct VolumeInfo *vi;

    	DoMethod(data->dld_VolumesList, AROSM_List_GetEntry, msg->Position, &vi);
		
	data->dld_CurPath[0] = 0;
        UpdateFileList(cl, o, msg->GInfo, vi->vi_Name, ASLB(AslBase));


    } /* if (volume list is shown) */
	
    return;     
}

/*****************************
**  DirList::DoubleClick()  **
*****************************/

STATIC VOID dirlist_doubleclick(Class *cl,
				Object *o,
				struct AROSP_Listview_DoubleClick *msg)
{
    struct DirListData *data = (struct DirListData *)INST_DATA(cl, o);
	    	
    if (data->dld_Flags & DLFLG_FILELIST)
    {
    
    	struct ExAllData *ead;

    	DoMethod(data->dld_FileList, AROSM_List_GetEntry, msg->Position, &ead);

	if (ead->ed_Type > 0)
	{
	
	    UpdateFileList(cl, o, msg->GInfo, ead->ed_Name, ASLB(AslBase));

	} /* if (Clicked entry is a directory) */
    } /* if (filelist is currently shown) */
   
    return;
}

/*****************************
**  DirList::ShowVolumes()  **
*****************************/

VOID dirlist_showvolumes(Class *cl, Object *o, struct AROSP_DirList_ShowVolumes *msg)
{
    struct opSet set_msg;
    
    struct TagItem tags[] =
    {
	{ AROSA_Listview_List, 		NULL},
	{ AROSA_Listview_DisplayHook,   (IPTR)&(GetSDLD(cl)->sd_VolDisplayHook) },
	{ AROSA_Listview_Format, 	(IPTR)"P=l" },
	{TAG_END}
    };
    
    struct DirListData *data = (struct DirListData *)INST_DATA(cl, o);
    
    /* Reuse the GInfo instead of using SetGadgetAttrs() which would
    ** waste time creating a new one
    */
    
    set_msg.MethodID	 = OM_SET;
    set_msg.ops_AttrList = tags;
    set_msg.ops_GInfo	 = msg->GInfo;
    
    tags[0].ti_Data = (IPTR)data->dld_VolumesList;
    
    DoSuperMethodA(cl, o, (Msg)&set_msg);
    data->dld_Flags &= ~DLFLG_FILELIST;
  
    if (msg->GInfo)
    {
    	struct RastPort *rp;

    	/* Relayout the lisview. (It has to calculate new column offsets etc.) */
    	DoMethod(o, GM_LAYOUT, msg->GInfo, TRUE);
    	if ((rp = ObtainGIRPort(msg->GInfo)) != NULL)
    	{
    	    DoMethod(o, GM_RENDER, msg->GInfo, rp, GREDRAW_REDRAW);
    	    
    	    ReleaseGIRPort(rp);
    	}
    }
    
    return;
}

/****************************
**  DirList::ShowParent()  **
****************************/
VOID dirlist_showparent(Class *cl, Object *o, struct AROSP_DirList_ShowParent *msg)
{
    UpdateFileList(cl, o, msg->GInfo, "/", AslBase);

    return;
}

/*****************
**  Dispatcher  **
*****************/

AROS_UFH3(STATIC IPTR, dispatch_dirlistclass,
    AROS_UFHA(Class *,  cl,  A0),
    AROS_UFHA(Object *, o,   A2),
    AROS_UFHA(Msg,      msg, A1)
)
{
    IPTR retval = 0UL;

    switch(msg->MethodID)
    {

    case OM_NEW:
	retval = (IPTR)dirlist_new(cl, o, (struct opSet *)msg);
	break;

    case OM_SET:
    case OM_UPDATE:
	retval = DoSuperMethodA(cl, o, msg);
	retval += (IPTR)dirlist_set(cl, o, (struct opSet *)msg);
	break;

    case OM_GET:
	retval = (IPTR)dirlist_get(cl, o, (struct opGet *)msg);
	break;
	    
    case OM_DISPOSE:
	dirlist_dispose(cl, o, msg);
	break;
	    
    case AROSM_Listview_SingleClick:
	dirlist_singleclick(cl, o, (struct AROSP_Listview_SingleClick *)msg);
	break;
	    
    case AROSM_Listview_DoubleClick:
	dirlist_doubleclick(cl, o, (struct AROSP_Listview_DoubleClick *)msg);
	break;
	
    case AROSM_DirList_ShowVolumes:
	dirlist_showvolumes(cl, o, (struct AROSP_DirList_ShowVolumes *)msg);
	break;

    case AROSM_DirList_ShowParent:
	dirlist_showparent(cl, o, (struct AROSP_DirList_ShowParent *)msg);
	break;

	    
    default:
	retval = DoSuperMethodA(cl, o, msg);
	break;
    } /* switch */
    

    return (retval);
}  /* dispatch_dirlistclass */


#undef AslBase

/****************************************************************************/

/* Initialize our dirlist class. */
struct IClass *InitDirListClass (struct AslBase_intern * AslBase)
{
    struct IClass *cl = NULL;

    /* This is the code to make the dirlistclass...
     */
    if ((cl = MakeClass(NULL, AROSLISTVIEWCLASS, NULL, sizeof(struct DirListData), 0)))
    {
    	struct StaticDirListData *sd;
    	
	cl->cl_Dispatcher.h_Entry    = (APTR)AROS_ASMSYMNAME(dispatch_dirlistclass);
	cl->cl_Dispatcher.h_SubEntry = NULL;
	
	sd = (struct StaticDirListData *)AllocMem(sizeof (struct StaticDirListData),
				MEMF_ANY|MEMF_CLEAR);
	
	if (sd)
	{
	    cl->cl_UserData = (IPTR)sd;
	    sd->sd_AslBase = AslBase;
	    
	    #define EasyHook(h, func, data) 			\
	    	(h)->h_Entry    = (APTR)AROS_ASMSYMNAME(func); \
	    	(h)->h_SubEntry = NULL;				\
	    	(h)->h_Data	= data;
	    	
	    EasyHook(&sd->sd_FileDisplayHook	, FileDisplayHook,  sd->sd_DispHookBuf);
	    EasyHook(&sd->sd_FileConstructHook	, FileConstructHook, AslBase);
	    EasyHook(&sd->sd_FileDestructHook	, FileDestructHook, AslBase);

	    EasyHook(&sd->sd_VolDisplayHook	, VolDisplayHook,  sd->sd_DispHookBuf);
	    EasyHook(&sd->sd_VolConstructHook	, VolConstructHook, AslBase);
	    EasyHook(&sd->sd_VolDestructHook	, VolDestructHook, AslBase);
	    
	    
	}
	else
	{
	    FreeClass(cl);
	    cl = NULL;
	}
    }

    return (cl);
}


VOID CleanupDirListClass(struct IClass *cl, struct AslBase_intern *AslBase)
{
    FreeMem((APTR)cl->cl_UserData, sizeof (struct StaticDirListData));
    
    FreeClass(cl);
    return;
}
