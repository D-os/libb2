

#include	<module.h>
#include	<termios.h>
#include	<tty/rico.h>
#include	<tty/str.h>
#include	<tty/barrier.h>
#include	<tty/dd.h>
#include	<tty/tty.h>


#define	B_TTY_MODULE_NAME	"bus_managers/tty/v1"


typedef struct tty_module_info	tty_module_info;

struct tty_module_info {
	module_info	mi;
	status_t	(*ttyopen)( struct ttyfile *, struct ddrover *, bool (*)( struct tty *, struct ddrover *, uint)),
			(*ttyclose)( struct ttyfile *, struct ddrover *),
			(*ttyfree)( struct ttyfile *, struct ddrover *),
			(*ttyread)( struct ttyfile *, struct ddrover *, char *, size_t *),
			(*ttywrite)( struct ttyfile *, struct ddrover *, const char *, size_t *),
			(*ttycontrol)( struct ttyfile *, struct ddrover *, ulong, void *, size_t);
	void		(*ttyinit)( struct tty *, bool),
			(*ttyilock)( struct tty *, struct ddrover *, bool),
			(*ttyhwsignal)( struct tty *, struct ddrover *, int, bool),
			(*ttyin)( struct tty *, struct ddrover *, int);
	int		(*ttyout)( struct tty *, struct ddrover *);
	struct ddrover	*(*ddrstart)( struct ddrover *);
	void		(*ddrdone)( struct ddrover *),
			(*ddacquire)( struct ddrover *, struct ddomain *);
};
