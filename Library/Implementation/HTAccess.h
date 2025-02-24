/*                                                      HTAccess:  Access manager  for libwww
                                      ACCESS MANAGER
                                             
   This module keeps a list of valid protocol (naming scheme) specifiers with associated
   access code. It allows documents to be loaded given various combinations of
   parameters. New access protocols may be registered at any time.
   
   Part of the libwww library .
   
 */
#ifndef HTACCESS_H
#define HTACCESS_H

/*      Definition uses:
*/
#include <HTUtils.h>
#include <HTSTD.h>
#include <HTAnchor.h>
#include <HTFormat.h>


/*      Return codes from load routines:
**
**      These codes may be returned by the protocol modules,
**      and by the HTLoad routines.
**      In general, positive codes are OK and negative ones are bad.
*/

#define HT_NO_DATA -9999        /* return code: OK but no data was loaded */

/* Typically, other app started or forked */

/*

Default Addresses

   These control the home page selection. To mess with these for normal browses is asking
   for user confusion.
   
 */
#define LOGICAL_DEFAULT "WWW_HOME"  /* Defined to be the home page */

#ifndef PERSONAL_DEFAULT
#define PERSONAL_DEFAULT "WWW/default.html"     /* in home directory */
#endif
#ifndef LOCAL_DEFAULT_FILE
#define LOCAL_DEFAULT_FILE "/usr/local/lib/WWW/default.html"
#endif
/*  If one telnets to a www access point,
    it will look in this file for home page */
#ifndef REMOTE_POINTER
#define REMOTE_POINTER  "/etc/www-remote.url"  /* can't be file */
#endif
/* and if that fails it will use this. */
#ifndef REMOTE_ADDRESS
#define REMOTE_ADDRESS  "http://info.cern.ch/remote.html"  /* can't be file */
#endif

/* If run from telnet daemon and no -l specified, use this file:
*/
#ifndef DEFAULT_LOGFILE
#define DEFAULT_LOGFILE "/usr/adm/www-log/www-log"
#endif

/*      If the home page isn't found, use this file:
*/
#ifndef LAST_RESORT
#define LAST_RESORT     "http://info.cern.ch/default.html"
#endif


/*

Flags which may be set to control this module

 */
extern int HTDiag;                      /* Flag: load source as plain text */
extern char* HTClientHost;             /* Name or number of telnetting host */
extern FILE* logfile;                  /* File to output one-liners to */
extern HTBool HTSecure;                   /* Disable security holes? */
extern HTStream* HTOutputStream;        /* For non-interactive, set this */
extern HTFormat HTOutputFormat;         /* To convert on load, set this */



/*

Load a document from relative name

  ON ENTRY,
  
  relative_name           The relative address of the file to be accessed.
                         
  here                    The anchor of the object being searched
                         
  ON EXIT,
  
  returns    HT_TRUE          Success in opening file
                         
  HT_FALSE                      Failure
                         
 */
HTBool HTLoadRelative(
		const char* relative_name, HTParentAnchor* here);


/*

Load a document from absolute name

  ON ENTRY,
  
  addr                    The absolute address of the document to be accessed.
                         
  filter                  if HT_TRUE, treat document as HTML
                         
 */

/*

  ON EXIT,
  
 */

/*

  returns HT_TRUE             Success in opening document
                         
  HT_FALSE                      Failure
                         
 */
HTBool HTLoadAbsolute(const char* addr);


/*

Load a document from absolute name to a stream

  ON ENTRY,
  
  addr                    The absolute address of the document to be accessed.
                         
  filter                  if HT_TRUE, treat document as HTML
                         
  ON EXIT,
  
  returns HT_TRUE             Success in opening document
                         
  HT_FALSE                      Failure
                         
   Note: This is equivalent to HTLoadDocument
   
 */
HTBool HTLoadToStream(const char* addr, HTBool filter, HTStream* sink);


/*

Load if necessary, and select an anchor

  ON ENTRY,
  
  destination                The child or parenet anchor to be loaded.
                         
 */

/*

  ON EXIT,
  
 */

/*

  returns HT_TRUE             Success
                         
  returns HT_FALSE              Failure
                         
 */



HTBool HTLoadAnchor(HTAnchor* destination);


/*

Make a stream for Saving object back

  ON ENTRY,
  
  anchor                  is valid anchor which has previously beeing loaded
                         
  ON EXIT,
  
  returns                 0 if error else a stream to save the object to.
                         
 */


HTStream* HTSaveStream(HTParentAnchor* anchor);


/*

Search

   Performs a search on word given by the user. Adds the search words to the end of the
   current address and attempts to open the new address.
   
  ON ENTRY,
  
  *keywords               space-separated keyword list or similar search list
                         
  here                    The anchor of the object being searched
                         
 */
HTBool HTSearch(const char* keywords, HTParentAnchor* here);


/*

Search Given Indexname

   Performs a keyword search on word given by the user. Adds the keyword to  the end of
   the current address and attempts to open the new address.
   
  ON ENTRY,
  
  *keywords               space-separated keyword list or similar search list
                         
  *indexname              is name of object search is to be done on.
                         
 */
HTBool HTSearchAbsolute(
		const char* keywords, const char* indexname);


/*

Register an access method

 */

typedef struct _HTProtocol {
	char* name;

	int (* load)(
			const char* full_address, HTParentAnchor* anchor,
			HTFormat format_out, HTStream* sink);

	HTStream* (* saveStream)(HTParentAnchor* anchor);

} HTProtocol;

HTBool HTRegisterProtocol(HTProtocol* protocol);


/*

Generate the anchor for the home page

 */

/*

   As it involves file access, this should only be done once when the program first runs.
   This is a default algorithm -- browser don't HAVE to use this.
   
 */
HTParentAnchor* HTHomeAnchor(void);

#endif /* HTACCESS_H */

/*

   end of HTAccess  */
