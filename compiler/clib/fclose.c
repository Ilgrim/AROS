/*
    (C) 1995-96 AROS - The Amiga Research OS
    $Id$

    Desc: ANSI C function fclose()
    Lang: english
*/
#include <proto/exec.h>
#include <proto/dos.h>
#include <errno.h>
#include <stdlib.h>
#include "__errno.h"
#include "__stdio.h"


/*****************************************************************************

    NAME */
#include <stdio.h>

	int fclose (

/*  SYNOPSIS */
	FILE * stream)

/*  FUNCTION
	Closes a stream.

    INPUTS
	stream - Stream to close.

    RESULT
	Upon successful completion 0 is returned. Otherwise, EOF is
	returned and the global variable errno is set to indicate the
	error. In either case no further access to the stream is possible.

    NOTES

    EXAMPLE

    BUGS

    SEE ALSO
	fopen(), open(), close()

    INTERNALS

    HISTORY
	17.01.97 digulla created

******************************************************************************/
{
    FILENODE * fn;
    int err;

    fn = FILE2FILENODE (stream);

    Remove ((struct Node *)fn);

    err = Close ((BPTR)fn->File.fh);

    FreeMem (fn, sizeof (FILENODE));

    if (err == EOF)
    {
	errno = IoErr2errno (IoErr ());

	return EOF;
    }

    return 0;
} /* fclose */

