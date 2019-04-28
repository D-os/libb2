

struct tty {
	uint		nopen,
			flags;
	struct ddomain	dd,
			ddi;
	pid_t		pgid;
	struct termios	t;
	uint		iactivity;
	bool		ibusy;
	struct str	istr,
			rstr,
			ostr;
	struct winsize	wsize;
	bool		(*service)( struct tty *, struct ddrover *, uint);
};
struct ttyfile {
	struct tty	*tty;
	uint		flags;
	bigtime_t	vtime;
};


/* tty.flags
 */
#define	TTYCARRIER	(1 << 0)
#define	TTYWRITABLE	(1 << 1)
#define	TTYWRITING	(1 << 2)
#define	TTYREADING	(1 << 3)
#define	TTYOSTOPPED	(1 << 4)
#define	TTYEXCLUSIVE	(1 << 5)
#define	TTYHWDCD	(1 << 6)
#define	TTYHWCTS	(1 << 7)
#define	TTYHWDSR	(1 << 8)
#define	TTYHWRI		(1 << 9)
#define	TTYFLOWFORCED	(1 << 10)

/* device commands
 */
#define	TTYENABLE	0
#define	TTYDISABLE	1
#define	TTYSETMODES	2
#define	TTYOSTART	3
#define	TTYOSYNC	4
#define	TTYISTOP	5
#define	TTYIRESUME	6
#define	TTYSETBREAK	7
#define	TTYCLRBREAK	8
#define	TTYSETDTR	9
#define	TTYCLRDTR	10
#define	TTYGETSIGNALS	11

#define	ttyforceflow( t)	((t)->flags |= TTYFLOWFORCED)

status_t
	ttyopen( struct ttyfile *, struct ddrover *, bool (*)( struct tty *, struct ddrover *, uint)),
	ttyclose( struct ttyfile *, struct ddrover *),
	ttyfree( struct ttyfile *, struct ddrover *),
	ttyread( struct ttyfile *, struct ddrover *, char *, size_t *),
	ttywrite( struct ttyfile *, struct ddrover *, const char *, size_t *),
	ttycontrol( struct ttyfile *, struct ddrover *, ulong, void *, size_t);
void	ttyinit( struct tty *, bool),
	ttyilock( struct tty *, struct ddrover *, bool),
	ttyhwsignal( struct tty *, struct ddrover *, int, bool),
	ttyin( struct tty *, struct ddrover *, int);
int	ttyout( struct tty *, struct ddrover *);
