

struct ddomain {
	struct ddrover	*r;
	bool		bg,
			locked;
};

struct ddentry {
	struct ddomain	*d;
	uint		n;
};

struct ddrover {
	struct ddrover	*nextd,
			*nexte;
	bool		bg;
	uint		nbg,
			nfg;
	cpu_status	ps;
	sem_id		sd,
			se;
	barrier		b;
	struct ddentry	e[10];
};

#define	ddbackground( d)	((d)->bg = TRUE)

bool		ddinit( );
void		dduninit( );
struct ddrover	*ddrstart( struct ddrover *);
void		ddacquire( struct ddrover *, struct ddomain *),
		ddrelease( struct ddrover *, struct ddomain *),
		ddrdone( struct ddrover *),
		ddrhousekeep( ),
		ddwakeup( void *);
bool		ddsleep( struct ddrover *, void *),
		ddsleeptimeout( struct ddrover *, void *, bigtime_t);
bool		ddsnooze( struct ddrover *, bigtime_t);
