/*
    (C) 1995-97 AROS - The Amiga Research OS
    $Id$

    Desc: Sets the prompt for the current CLI.
    Lang: english
*/
#include <proto/exec.h>
#include <proto/dos.h>
#include <dos/dos.h>
#include <dos/dosextens.h>
#include "dos_intern.h"

/*****************************************************************************

    NAME */
#include <proto/dos.h>

	AROS_LH1(BOOL, SetPrompt,

/*  SYNOPSIS */
	AROS_LHA(STRPTR, name, D1),

/*  LOCATION */
	struct DosLibrary *, DOSBase, 97, Dos)

/*  FUNCTION
	Sets the prompt in the current CLI structure. If the name doesn't
	fit the old name is kept and a failure is returned. If the current
	process doesn't have a CLI structure this function does nothing.

    INPUTS
	name - The prompt to be set.

    RESULT
	!=0 on success, 0 on failure.

    NOTES

    EXAMPLE

    BUGS
	Never copies more than 255 bytes.

    SEE ALSO
	GetPrompt()

    INTERNALS

    HISTORY
	27-11-96    digulla automatically created from
			    dos_lib.fd and clib/dos_protos.h

*****************************************************************************/
{
    AROS_LIBFUNC_INIT
    AROS_LIBBASE_EXT_DECL(struct DosLibrary *,DOSBase)
    struct CommandLineInterface *cli = NULL;
    STRPTR s;
    ULONG namelen;

    if ((cli = Cli()) == NULL)
        return DOSFALSE;

    s = name;
    while(*s++)
        ;
    namelen = s - name - 1;

    if (namelen > 255)
        return DOSFALSE;

    s = AROS_BSTR_ADDR(cli->cli_Prompt);

    AROS_BSTR_setstrlen(cli->cli_Prompt, namelen);
    CopyMem((APTR)name, s, namelen);

    return DOSTRUE;
    AROS_LIBFUNC_EXIT
} /* SetPrompt */
