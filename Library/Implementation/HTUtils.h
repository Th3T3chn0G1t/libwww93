/*
 * Utitlity macros for the W3 code library
 * MACROS FOR GENERAL USE
 * Generates: HTUtils.h
 * See also: the system dependent file "tcp.h"
 */

#ifndef HTUTILS_H
#define HTUTILS_H

#include <HTString.h> /* String utilities */

/*
 * Debug message control.
 */
#ifndef NDEBUG
# define TRACE (WWW_TraceFlag)
# define PROGRESS(str) printf(str)
extern int WWW_TraceFlag;
#else
# define TRACE (0)
# define PROGRESS(str) /* nothing for now */
#endif
#define CTRACE if(TRACE) fprintf

/*
 * ERROR TYPE
 *
 * This is passed back when streams are aborted. It might be nice to have some
 * structure of error messages, numbers, and recursive pointers to reasons.
 * Curently this is a placeholder for something more sophisticated.
 */
typedef void* HTError; /* Unused at present -- best definition? */

/*
 * Booleans
 */
typedef char HTBool;
#define HT_TRUE ((HTBool) 1)
#define HT_FALSE ((HTBool) 0)

#define HT_MIN(a, b) ((a) <= (b) ? (a) : (b))

#define HT_TCP_PORT (80) /* Allocated to http by Jon Postel/ISI 24-Jan-92 */
#define HT_DNP_OBJ (80) /* This one doesn't look busy, but we must check */

/* That one was for decnet */

/* Inline Function WHITE: Is character c white space? */

/* For speed, include all control characters */
#define HT_WHITE(c) (((unsigned char) ((c))) <= 32)

/*
 * Success (>=0) and failure (<0) codes
 */
#define HT_LOADED (29999) /* Instead of a socket */
#define HT_OK (0) /* Generic success*/
#define HT_NO_ACCESS (-10) /* Access not available */
#define HT_FORBIDDEN (-11) /* Access forbidden */

/*
 * Out Of Memory checking for malloc() return:
 */
void HTOOM(const char* file, const char* func);

#endif /* HTUTILS_H */

/* end of utilities */
