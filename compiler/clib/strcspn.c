/*
    (C) 1995-97 AROS - The Amiga Research OS
    $Id$

    Desc: ANSI C function strcspn()
    Lang: english
*/

/*****************************************************************************

    NAME */
#include <string.h>

	size_t strcspn (

/*  SYNOPSIS */
	const char * str,
	const char * reject)

/*  FUNCTION
	Calculates the length of the initial segment of str which consists
	entirely of characters not in reject.

    INPUTS
	str - The string to check.
	reject - Characters which must not be in str.

    RESULT
	Length of the initial segment of str which doesn't contain any
	characters from reject.

    NOTES

    EXAMPLE
	char buffer[64];

	strcpy (buffer, "Hello ");

	// Returns 5
	strcspn (buffer, " ");

	// Returns 0
	strcspn (buffer, "H");

    BUGS

    SEE ALSO

    INTERNALS

    HISTORY
	11.12.1996 digulla created

******************************************************************************/
{
    size_t n = 0; /* Must set this to zero */

    while (*str && !strchr (reject, *str))
    {
	str ++;
	n ++;
    }

    return n;
} /* strcspn */
