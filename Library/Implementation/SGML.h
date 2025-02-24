/*                                                SGML parse and stream definition for libwww
                               SGML AND STRUCTURED STREAMS
                                             
   The SGML parser is a state machine. It is called for every character
   
   of the input stream. The DTD data structure contains pointers
   
   to functions which are called to implement the actual effect of the
   
   text read. When these functions are called, the attribute structures pointed to by the
   DTD are valid, and the function is passed a pointer to the curent tag structure, and an
   "element stack" which represents the state of nesting within SGML elements.
   
   The following aspects are from Dan Connolly's suggestions:  Binary search, Strcutured
   object scheme basically, SGML content enum type.
   
   (c) Copyright CERN 1991 - See Copyright.html
   
 */
#ifndef SGML_H
#define SGML_H

#include <HTUtils.h>
#include <HTStream.h>

/*

SGML content types

 */
typedef enum _SGMLContent {
	SGML_EMPTY,    /* no content */
	SGML_LITERAL, /* character data. Recognised excat close tag only. literal
                    Old www server compatibility only! Not SGML */
	SGML_CDATA,    /* character data. recognize </ only */
	SGML_RCDATA,   /* replaceable character data. recognize </ and &ref; */
	SGML_MIXED,    /* elements and parsed character data. recognize all markup */
	SGML_ELEMENT   /* any data found will be returned as an error*/
} SGMLContent;


typedef struct {
	char* name;           /* The (constant) name of the attribute */
	/* Could put type info in here */
} attr;


/*              A tag structure describes an SGML element.
**              -----------------------------------------
**
**
**      name            is the string which comes after the tag opener "<".
**
**      attributes      points to a zero-terminated array
**                      of attribute names.
**
**      literal        determines how the SGML engine parses the charaters
**                      within the element. If set, tag openers are ignored
**                      except for that which opens a matching closing tag.
**
*/
typedef struct _tag HTTag;
struct _tag {
	char* name;                   /* The name of the tag */
	attr* attributes;             /* The list of acceptable attributes */
	int number_of_attributes;   /* Number of possible attributes */
	SGMLContent contents;               /* End only on end tag @@ */
};


/*              DTD Information
**              ---------------
**
** Not the whole DTD, but all this parser usues of it.
*/
typedef struct {
	HTTag* tags;           /* Must be in strcmp order by name */
	int number_of_tags;
	const char** entity_names;   /* Must be in strcmp order by name */
	int number_of_entities;
} SGML_dtd;


/*      SGML context passed to parsers
*/
typedef struct _HTSGMLContext* HTSGMLContext;   /* Hidden */


/*__________________________________________________________________________
*/
/*              Structured Object definition
**
**      A structured object is something which can reasonably be
**      represented in SGML. I'll rephrase that. A structured
**      object is am ordered tree-structured arrangement of data
**      which is representable as text.
**
**      The SGML parer outputs to a Structured object.
**      A Structured object can output its contents
**      to another Structured Object.
**      It's a kind of typed stream. The architecure
**      is largely Dan Conolly's.
**      Elements and entities are passed to the sob by number, implying
**      a knowledge of the DTD.
**      Knowledge of the SGML syntax is not here, though.
**
**      Superclass: HTStream
*/


/*      The creation methods will vary on the type of Structured Object.
**      Maybe the callerData is enough info to pass along.
*/

typedef struct _HTStructured HTStructured;

typedef void (*HTStructuredFree)(HTStructured*);
typedef void (*HTStructuredAbort)(HTStructured*, HTError);
typedef void (*HTStructuredPutCharacter)(HTStructured*, char);
typedef void (*HTStructuredPutString)(HTStructured*, const char*);
typedef void (*HTStructuredWrite)(HTStructured*, const char*, unsigned);
typedef void (*HTStructuredStartElement)(
		HTStructured*, int, const HTBool*, const char**);

typedef void (*HTStructuredEndElement)(HTStructured*, int);
typedef void (*HTStructuredPutEntity)(HTStructured*, int);

typedef struct _HTStructuredClass {
	char* name; /* Just for diagnostics */

	HTStructuredFree free;
	HTStructuredAbort abort;
	HTStructuredPutCharacter put_character;
	HTStructuredPutString put_string;
	HTStructuredWrite write;
	HTStructuredStartElement start_element;
	HTStructuredEndElement end_element;
	HTStructuredPutEntity put_entity;
} HTStructuredClass;


/*      Create an SGML parser
**
** On entry,
**      dtd             must point to a DTD structure as defined above
**      callbacks       must point to user routines.
**      callData        is returned in callbacks transparently.
** On exit,
**              The default tag starter has been processed.
*/


HTStream* SGML_new(
		const SGML_dtd* dtd, HTStructured* target);

extern const HTStreamClass SGMLParser;


#endif  /* SGML_H */




/*

    */
