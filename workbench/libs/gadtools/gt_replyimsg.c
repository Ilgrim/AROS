/*
    (C) 1997 AROS - The Amiga Research OS
    $Id$

    Desc:
    Lang: english
*/
#include <proto/exec.h>
#include <exec/ports.h>
#include "gadtools_intern.h"

/*********************************************************************

    NAME */
#include <proto/gadtools.h>
#include <intuition/intuition.h>

        AROS_LH1(void, GT_ReplyIMsg,

/*  SYNOPSIS */
	AROS_LHA(struct IntuiMessage *, imsg, A1),

/*  LOCATION */
	struct Library *, GadToolsBase, 13, GadTools)

/*  FUNCTION
        Replies a message gotten via GT_GetIMsg().

    INPUTS
        imsg - The message to reply.

    RESULT

    NOTES

    EXAMPLE

    BUGS

    SEE ALSO
        GT_GetIMsg(), exec.library/ReplyMsg()

    INTERNALS

    HISTORY

***************************************************************************/
{
    AROS_LIBFUNC_INIT
    AROS_LIBBASE_EXT_DECL(struct GadToolsBase *,GadToolsBase)

    ReplyMsg((struct Message *)imsg);

    AROS_LIBFUNC_EXIT
} /* GT_ReplyIMsg */
