/*			Generic Communication Code		HTTCP.c
**			==========================
**
**	This code is in common between client and server sides.
**
**	16 Jan 92  TBL	Fix strtol() undefined on CMU Mach.
**	25 Jun 92  JFG  Added DECNET option through TCP socket emulation.
*/

#include <HTUtils.h>
#include <HTSTD.h>        /* Defines SHORT_NAMES if necessary */

/*	Module-Wide variables
*/

static char* hostname = 0;        /* The name of this host */


/*	VARIABLES
*/

/* struct sockaddr_in HTHostAddress; */    /* The internet address of the host */

/* Valid after call to HTHostName() */

/*	Encode INET status (as in sys/errno.h)			  inet_status()
**	------------------
**
** On entry,
**	where		gives a description of what caused the error
**	global errno	gives the error number in the unix way.
**
** On return,
**	returns		a negative status in the unix way.
*/

/*	Report Internet Error
**	---------------------
*/
int HTInetStatus(char* where) {
	CTRACE(
			stderr,
			"TCP: Error %d in `errno' after call to %s() failed.\n\t%s\n",
			errno, where, strerror(errno));

	return -errno;
}


/*
 *  Parse a cardinal value				       parse_cardinal()
 *	----------------------
 *
 * On entry,
 *	*pp	    points to first character to be interpreted, terminated by
 *		    non 0:9 character.
 *	*pstatus    points to status already valid
 *	maxvalue    gives the largest allowable value.
 *
 * On exit,
 *	*pp	    points to first unread character
 *	*pstatus    points to status updated iff bad
 */

unsigned HTCardinal(int* pstatus, char** pp, unsigned max_value) {
	int n;

	if((**pp < '0') || (**pp > '9')) { /* Null string is error */
		*pstatus = -3; /* No number where one expeceted */
		return 0;
	}

	n = 0;
	while((**pp >= '0') && (**pp <= '9')) n = n * 10 + *((*pp)++) - '0';

	if(n > (int) max_value) {
		*pstatus = -4; /* Cardinal outside range */
		return 0;
	}

	return n;
}


#ifndef DECNET /* Function only used below for a trace message */

/*	Produce a string for an Internet address
 *	----------------------------------------
 *
 * On exit,
 *	returns	a pointer to a static string which must be copied if
 *		it is to be kept.
 */

const char* HTInetString(struct sockaddr_in* sin) {
	static char string[16];
	sprintf(
			string, "%d.%d.%d.%d",
			(int) *((unsigned char*) (&sin->sin_addr) + 0),
			(int) *((unsigned char*) (&sin->sin_addr) + 1),
			(int) *((unsigned char*) (&sin->sin_addr) + 2),
			(int) *((unsigned char*) (&sin->sin_addr) + 3));
	return string;
}

#endif /* Decnet */


/*	Parse a network node address and port
**	-------------------------------------
**
** On entry,
**	str	points to a string with a node name or number,
**		with optional trailing colon and port number.
**	sin	points to the binary internet or decnet address field.
**
** On exit,
**	*sin	is filled in. If no port is specified in str, that
**		field is left unchanged in *sin.
*/
int HTParseInet(struct sockaddr_in* sin, const char* str) {
	char* port;
	char host[256];
	struct hostent* phost;    /* Pointer to host - See netdb.h */
	strcpy(host, str);        /* Take a copy we can mutilate */



/*	Parse port number if present
*/
	if((port = strchr(host, ':'))) {
		*port++ = 0;        /* Chop off port */
		if(port[0] >= '0' && port[0] <= '9') {

#ifdef unix
			sin->sin_port = htons(atol(port));
#else /* VMS */
#ifdef DECNET
			sin->sdn_objnum = (unsigned char) (strtol(port, (char**)0 , 10));
#else
			sin->sin_port = htons((u_short) strtol(port, (char**) 0, 10));
#endif /* Decnet */
#endif /* Unix vs. VMS */

		}
		else {

#ifdef SUPPRESS        /* 1. crashes!?!. 2. Not recommended */
			struct servent * serv = getservbyname(port, (char*)0);
			if (serv) sin->sin_port = serv->s_port;
			else if (TRACE) fprintf(stderr, "TCP: Unknown service %s\n", port);
#endif
		}
	}

#ifdef DECNET
	/* read Decnet node name. @@ Should know about DECnet addresses, but it's
	   probably worth waiting until the Phase transition from IV to V. */

	sin->sdn_nam.n_len = HT_MIN(DN_MAXNAML, strlen(host));  /* <=6 in phase 4 */
	strncpy (sin->sdn_nam.n_name, host, sin->sdn_nam.n_len + 1);

	if (TRACE) fprintf(stderr,
	"DECnet: Parsed address as object number %d on host %.6s...\n",
			  sin->sdn_objnum, host);

#else  /* parse Internet host */

/*	Parse host number if present.
*/
	if(*host >= '0' && *host <= '9') {   /* Numeric node address: */
		sin->sin_addr.s_addr = inet_addr(host); /* See arpa/inet.h */

	}
	else {            /* Alphanumeric node name: */
#ifdef MVS    /* Oustanding problem with crash in MVS gethostbyname */
		if(TRACE)fprintf(stderr, "HTTCP: Calling gethostbyname(%s)\n", host);
#endif
		phost = gethostbyname(host);    /* See netdb.h */
#ifdef MVS
		if(TRACE)fprintf(stderr, "HTTCP: gethostbyname() returned %d\n", phost);
#endif
		if(!phost) {
			if(TRACE) {
				fprintf(
						stderr,
						"HTTPAccess: Can't find internet node name `%s'.\n",
						host);
			}
			return -1;  /* Fail? */
		}
		memcpy(&sin->sin_addr, phost->h_addr, phost->h_length);
	}

	if(TRACE) {
		fprintf(
				stderr,
				"TCP: Parsed address as port %d, IP address %d.%d.%d.%d\n",
				(int) ntohs(sin->sin_port),
				(int) *((unsigned char*) (&sin->sin_addr) + 0),
				(int) *((unsigned char*) (&sin->sin_addr) + 1),
				(int) *((unsigned char*) (&sin->sin_addr) + 2),
				(int) *((unsigned char*) (&sin->sin_addr) + 3));
	}

#endif  /* Internet vs. Decnet */

	return 0;    /* OK */
}


/*	Derive the name of the host on which we are
**	-------------------------------------------
**
*/
static void get_host_details(void)

#ifndef MAXHOSTNAMELEN
#define MAXHOSTNAMELEN 64        /* Arbitrary limit */
#endif

{
	char name[MAXHOSTNAMELEN + 1];    /* The name of this host */
#ifdef NEED_HOST_ADDRESS        /* no -- needs name server! */
	struct hostent * phost;		/* Pointer to host -- See netdb.h */
#endif
	int namelength = sizeof(name);

	if(hostname) return;        /* Already done */
	gethostname(name, namelength);    /* Without domain */
	CTRACE(stderr, "TCP: Local host name is %s\n", name);
	StrAllocCopy(hostname, name);

#ifndef DECNET  /* Decnet ain't got no damn name server 8#OO */
#ifdef NEED_HOST_ADDRESS        /* no -- needs name server! */
	phost=gethostbyname(name);		/* See netdb.h */
	if (!phost) {
	if (TRACE) fprintf(stderr,
		"TCP: Can't find my own internet node address for `%s'!!\n",
		name);
	return;  /* Fail! */
	}
	StrAllocCopy(hostname, phost->h_name);
	memcpy(&HTHostAddress, &phost->h_addr, phost->h_length);
	if (TRACE) fprintf(stderr, "     Name server says that I am `%s' = %s\n",
		hostname, HTInetString(&HTHostAddress));
#endif

#endif /* not Decnet */
}

const char* HTHostName(void) {
	get_host_details();
	return hostname;
}

