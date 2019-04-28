

typedef struct {
	spinlock	l;
	bool		held;
} barrier;

#define	binit( b)	((b)->held = TRUE)

void	bwait( barrier *),
	brelease( barrier *);
