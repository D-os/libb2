

#define	LATENCY_INSTRUMENTATION	0


#if LATENCY_INSTRUMENTATION

cpu_status	idisable( );
void		istart( ),
		idone( ),
		irestore( cpu_status),
		idump( );

#else

#define	idisable( )	disable_interrupts( )
#define	irestore( ps)	restore_interrupts( ps)
#define	istart( )
#define	idone( )
#define	idump( )

#endif
