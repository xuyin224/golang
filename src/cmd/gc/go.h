// Copyright 2009 The Go Authors. All rights reserved.
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file.

/*
*/

#include	<u.h>
#include	<libc.h>
#include	<bio.h>
#include	"compat.h"

#ifndef	EXTERN
#define	EXTERN	extern
#endif
enum
{
	NHUNK		= 50000,
	BUFSIZ		= 8192,
	NSYMB		= 500,
	NHASH		= 1024,
	STRINGSZ	= 200,
	YYMAXDEPTH	= 500,
	MAXALIGN	= 7,
	UINF		= 100,
	HISTSZ		= 10,

	PRIME1		= 3,
	PRIME2		= 10007,
	PRIME3		= 10009,
	PRIME4		= 10037,
	PRIME5		= 10039,
	PRIME6		= 10061,
	PRIME7		= 10067,
	PRIME8		= 10079,
	PRIME9		= 10091,
	PRIME10		= 10093,

	AUNK		= 100,

	// these values are known by runtime
	AMEM		= 0,
	ANOEQ,
	ASTRING,
	AINTER,
	ANILINTER,

	BADWIDTH	= -1000000000
};

/*
 * note this is the representation
 * of the compilers string literals,
 * it is not the runtime representation
 */
typedef	struct	Strlit	Strlit;
struct	Strlit
{
	int32	len;
	char	s[3];	// variable
};

/*
 * note this is the runtime representation
 * of hashmap iterator. it is probably
 * insafe to use it this way, but it puts
 * all the changes in one place.
 * only flag is referenced from go.
 * actual placement does not matter as long
 * as the size is >= actual size.
 */
typedef	struct	Hiter	Hiter;
struct	Hiter
{
	uchar	data[8];		// return val from next
	int32	elemsize;		// size of elements in table */
	int32	changes;		// number of changes observed last time */
	int32	i;			// stack pointer in subtable_state */
	uchar	last[8];		// last hash value returned */
	uchar	h[8];			// the hash table */
	struct
	{
		uchar	sub[8];		// pointer into subtable */
		uchar	start[8];	// pointer into start of subtable */
		uchar	end[8];		// pointer into end of subtable */
		uchar	pad[8];
	} sub[4];
};

enum
{
	Mpscale	= 29,		// safely smaller than bits in a long
	Mpprec	= 16,		// Mpscale*Mpprec is max number of bits
	Mpnorm	= Mpprec - 1,	// significant words in a normalized float
	Mpbase	= 1L << Mpscale,
	Mpsign	= Mpbase >> 1,
	Mpmask	= Mpbase - 1,
	Mpdebug	= 0,
};

typedef	struct	Mpint	Mpint;
struct	Mpint
{
	long	a[Mpprec];
	uchar	neg;
	uchar	ovf;
};

typedef	struct	Mpflt	Mpflt;
struct	Mpflt
{
	Mpint	val;
	short	exp;
};

typedef	struct	Val	Val;
struct	Val
{
	short	ctype;
	union
	{
		short	reg;		// OREGISTER
		short	bval;		// bool value CTBOOL
		Mpint*	xval;		// int CTINT
		Mpflt*	fval;		// float CTFLT
		Strlit*	sval;		// string CTSTR
	} u;
};

typedef	struct	Sym	Sym;
typedef	struct	Node	Node;
typedef	struct	Type	Type;
typedef	struct	Dcl	Dcl;

struct	Type
{
	uchar	etype;
	uchar	chan;
	uchar	recur;		// to detect loops
	uchar	trecur;		// to detect loops
	uchar	printed;
	uchar	embedded;	// TFIELD embedded type
	uchar	siggen;
	uchar	funarg;
	uchar	copyany;
	uchar	local;		// created in this file

	// TFUNCT
	uchar	thistuple;
	uchar	outtuple;
	uchar	intuple;
	uchar	outnamed;

	Type*	method;

	Sym*	sym;
	int32	vargen;		// unique name for OTYPE/ONAME

	Node*	nname;
	vlong	argwid;

	// most nodes
	Type*	type;
	vlong	width;		// offset in TFIELD, width in all others

	// TFIELD
	Type*	down;		// also used in TMAP
	Strlit*	note;		// literal string annotation

	// TARRAY
	int32	bound;		// negative is dynamic array
};
#define	T	((Type*)0)

struct	Node
{
	uchar	op;
	uchar	ullman;		// sethi/ullman number
	uchar	addable;	// type of addressability - 0 is not addressable
	uchar	trecur;		// to detect loops
	uchar	etype;		// op for OASOP, etype for OTYPE, exclam for export
	uchar	class;		// PPARAM, PAUTO, PEXTERN, etc
	uchar	method;		// OCALLMETH name
	uchar	iota;		// OLITERAL made from iota
	uchar	embedded;	// ODCLFIELD embedded type
	uchar	colas;		// OAS resulting from :=
	uchar	diag;		// already printed error about this
	uchar	noescape;	// ONAME never move to heap
	uchar	funcdepth;

	// most nodes
	Node*	left;
	Node*	right;
	Type*	type;

	// for-body
	Node*	ninit;
	Node*	ntest;
	Node*	nincr;
	Node*	nbody;

	// if-body
	Node*	nelse;

	// cases
	Node*	ncase;

	// func
	Node*	nname;
	Node*	enter;
	Node*	exit;
	Node*	cvars;	// closure params
	Dcl*	dcl;	// outer autodcl

	// OLITERAL/OREGISTER
	Val	val;

	// ONAME func param with PHEAP
	Node*	heapaddr;	// temp holding heap address of param
	Node*	stackparam;	// OPARAM node referring to stack copy of param
	Node*	alloc;	// allocation call

	// ONAME closure param with PPARAMREF
	Node*	outer;	// outer PPARAMREF in nested closure
	Node*	closure;	// ONAME/PHEAP <-> ONAME/PPARAMREF

	Sym*	psym;		// import
	Sym*	sym;		// various
	int32	vargen;		// unique name for OTYPE/ONAME
	int32	lineno;
	vlong	xoffset;
};
#define	N	((Node*)0)

struct	Sym
{
	ushort	block;		// blocknumber to catch redeclaration

	uchar	undef;		// a diagnostic has been generated
	uchar	export;		// marked as export
	uchar	exported;	// exported
	uchar	imported;	// imported
	uchar	sym;		// huffman encoding in object file
	uchar	uniq;		// imbedded field name first found
	uchar	siggen;		// signature generated

	char*	package;	// package name
	char*	name;		// variable name
	Node*	oname;		// ONAME node if a var
	Type*	otype;		// TYPE node if a type
	Node*	oconst;		// OLITERAL node if a const
	char*	opack;		// package reference if lexical == LPACK
	vlong	offset;		// stack location if automatic
	int32	lexical;
	int32	vargen;		// unique variable number
	int32	lastlineno;	// last declaration for diagnostic
	Sym*	link;
};
#define	S	((Sym*)0)

struct	Dcl
{
	uchar	op;
	ushort	block;
	int32	lineno;

	Sym*	dsym;		// for printing only
	Node*	dnode;		// oname
	Type*	dtype;		// otype

	Dcl*	forw;
	Dcl*	back;		// sentinel has pointer to last
};
#define	D	((Dcl*)0)

typedef	struct	Iter	Iter;
struct	Iter
{
	int	done;
	Type*	tfunc;
	Type*	t;
	Node**	an;
	Node*	n;
};

typedef	struct	Hist	Hist;
struct	Hist
{
	Hist*	link;
	char*	name;
	int32	line;
	int32	offset;
};
#define	H	((Hist*)0)

enum
{
	OXXX,

	OTYPE, OCONST, OVAR, OIMPORT,

	ONAME, ONONAME, ODCL,
	ODOT, ODOTPTR, ODOTMETH, ODOTINTER,
	ODCLFUNC, ODCLFIELD, ODCLARG,
	OLIST, OCMP, OPTR, OARRAY, ORANGE,
	ORETURN, OFOR, OIF, OSWITCH, ODEFER,
	OAS, OASOP, OCASE, OXCASE, OFALL, OXFALL,
	OGOTO, OPROC, OMAKE, ONEW, OEMPTY, OSELECT,
	OLEN, OCAP, OPANIC, OPANICN, OPRINT, OPRINTN, OTYPEOF,
	OCLOSE, OCLOSED,

	OOROR,
	OANDAND,
	OEQ, ONE, OLT, OLE, OGE, OGT,
	OADD, OSUB, OOR, OXOR,
	OMUL, ODIV, OMOD, OLSH, ORSH, OAND, OANDNOT,
	OINC, ODEC,	// placeholders - not used
	OFUNC,
	OLABEL,
	OBREAK,
	OCONTINUE,
	OADDR,
	OIND,
	OCALL, OCALLMETH, OCALLINTER,
	OINDEX, OSLICE,
	ONOT, OCOM, OPLUS, OMINUS, OSEND, ORECV,
	OLITERAL, OREGISTER, OINDREG,
	OKEY, OPARAM,
	OCOMPOS, OCOMPSLICE, OCOMPMAP,
	OCONV,
	ODOTTYPE, OTYPESW,
	OBAD,

	OEXTEND,	// 6g internal

	OEND,
};
enum
{
	Txxx,			// 0

	TINT8,	TUINT8,		// 1
	TINT16,	TUINT16,
	TINT32,	TUINT32,
	TINT64,	TUINT64,
	TINT, TUINT, TUINTPTR,

	TFLOAT32,		// 12
	TFLOAT64,
	TFLOAT80,
	TFLOAT,

	TBOOL,			// 16

	TPTR32, TPTR64,		// 17

	TDDD,			// 19
	TFUNC,
	TARRAY,
	T_old_DARRAY,
	TSTRUCT,		// 23
	TCHAN,
	TMAP,
	TINTER,			// 26
	TFORW,
	TFIELD,
	TANY,
	TSTRING,
	TFORWSTRUCT,
	TFORWINTER,

	// pseudo-types for literals
	TIDEAL,
	TNIL,

	NTYPE,
};
enum
{
	CTxxx,

	CTINT,
	CTFLT,
	CTSTR,
	CTBOOL,
	CTNIL,
};

enum
{
	/* types of channel */
	Cxxx,
	Crecv = 1<<0,
	Csend = 1<<1,
	Cboth = Crecv | Csend,
};

enum
{
	Pxxx,

	PEXTERN,	// declaration context
	PAUTO,
	PPARAM,
	PPARAMOUT,
	PPARAMREF,	// param passed by reference
	PFUNC,

	PHEAP = 1<<7,
};

enum
{
	Exxx,
	Eyyy,
	Etop,		// evaluated at statement level
	Elv,		// evaluated in lvalue context
	Erv,		// evaluated in rvalue context
};

#define	BITS	5
#define	NVAR	(BITS*sizeof(uint32)*8)

typedef	struct	Bits	Bits;
struct	Bits
{
	uint32	b[BITS];
};

EXTERN	Bits	zbits;

typedef	struct	Var	Var;
struct	Var
{
	vlong	offset;
	Sym*	sym;
	int	width;
	char	name;
	char	etype;
};

EXTERN	Var	var[NVAR];

typedef	struct	Typedef	Typedef;
struct	Typedef
{
	char*	name;
	int	etype;
	int	sameas;
};

extern	Typedef	typedefs[];

typedef	struct	Sig	Sig;
struct Sig
{
	char*	name;
	Sym*	sym;
	uint32	hash;
	int32	perm;
	int32	offset;
	Sig*	link;
};

typedef	struct	Io	Io;
struct	Io
{
	char*	infile;
	Biobuf*	bin;
	int32	ilineno;
	int	peekc;
	int	peekc1;	// second peekc for ...
	char*	cp;	// used for content when bin==nil
};

typedef	struct	Dlist	Dlist;
struct	Dlist
{
	Type*	field;
};

typedef	struct	Idir	Idir;
struct Idir
{
	Idir*	link;
	char*	dir;
};

/*
 * note this is the runtime representation
 * of the compilers arrays.
 *
 * typedef	struct
 * {				// must not move anything
 * 	uchar	array[8];	// pointer to data
 * 	uchar	nel[4];		// number of elements
 * 	uchar	cap[4];		// allocated number of elements
 * } Array;
 */
EXTERN	int	Array_array;	// runtime offsetof(Array,array) - same for String
EXTERN	int	Array_nel;	// runtime offsetof(Array,nel) - same for String
EXTERN	int	Array_cap;	// runtime offsetof(Array,cap)
EXTERN	int	sizeof_Array;	// runtime sizeof(Array)


/*
 * note this is the runtime representation
 * of the compilers strings.
 *
 * typedef	struct
 * {				// must not move anything
 * 	uchar	array[8];	// pointer to data
 * 	uchar	nel[4];		// number of elements
 * } String;
 */
EXTERN	int	sizeof_String;	// runtime sizeof(String)

EXTERN	Dlist	dotlist[10];	// size is max depth of embeddeds

EXTERN	Io	curio;
EXTERN	Io	pushedio;
EXTERN	int32	lineno;
EXTERN	int32	prevlineno;
EXTERN	char*	pathname;
EXTERN	Hist*	hist;
EXTERN	Hist*	ehist;

EXTERN	char*	infile;
EXTERN	char*	outfile;
EXTERN	char*	package;
EXTERN	Biobuf*	bout;
EXTERN	int	nerrors;
EXTERN	char	namebuf[NSYMB];
EXTERN	char	lexbuf[NSYMB];
EXTERN	char	debug[256];
EXTERN	Sym*	hash[NHASH];
EXTERN	Sym*	dclstack;
EXTERN	Sym*	b0stack;
EXTERN	Sym*	pkgmyname;	// my name for package
EXTERN	Sym*	pkgimportname;	// package name from imported package
EXTERN	int	tptr;		// either TPTR32 or TPTR64
extern	char*	sysimport;
extern	char*	unsafeimport;
EXTERN	char*	filename;	// name to uniqify names
EXTERN	Idir*	idirs;

EXTERN	Type*	types[NTYPE];
EXTERN	uchar	simtype[NTYPE];
EXTERN	uchar	isptr[NTYPE];
EXTERN	uchar	isint[NTYPE];
EXTERN	uchar	isfloat[NTYPE];
EXTERN	uchar	issigned[NTYPE];
EXTERN	uchar	issimple[NTYPE];
EXTERN	uchar	okforeq[NTYPE];
EXTERN	uchar	okforadd[NTYPE];
EXTERN	uchar	okforand[NTYPE];
EXTERN	Type*	idealstring;

EXTERN	Mpint*	minintval[NTYPE];
EXTERN	Mpint*	maxintval[NTYPE];
EXTERN	Mpflt*	minfltval[NTYPE];
EXTERN	Mpflt*	maxfltval[NTYPE];

EXTERN	Dcl*	autodcl;
EXTERN	Dcl*	paramdcl;
EXTERN	Dcl*	externdcl;
EXTERN	Dcl*	exportlist;
EXTERN	Dcl*	signatlist;
EXTERN	Dcl*	typelist;
EXTERN	int	dclcontext;		// PEXTERN/PAUTO
EXTERN	int	importflag;
EXTERN	int	inimportsys;
EXTERN	int	initflag;		// compiling the init fn
EXTERN	int	statuniqgen;		// name generator for static temps

EXTERN	uint32	iota;
EXTERN	Node*	lastconst;
EXTERN	Type*	lasttype;
EXTERN	int32	vargen;
EXTERN	int32	exportgen;
EXTERN	int32	maxarg;
EXTERN	int32	stksize;		// stack size for current frame
EXTERN	int32	initstksize;		// stack size for init function
EXTERN	ushort	blockgen;		// max block number
EXTERN	ushort	block;			// current block number
EXTERN	int	hasdefer;		// flag that curfn has defer statetment

EXTERN	int	maxround;
EXTERN	int	widthptr;

EXTERN	Node*	retnil;
EXTERN	Node*	fskel;

EXTERN	Node*	addtop;
EXTERN	Node*	typeswvar;

EXTERN	char*	context;
EXTERN	char*	pkgcontext;
extern	int	thechar;
extern	char*	thestring;
EXTERN	char*	hunk;
EXTERN	int32	nhunk;
EXTERN	int32	thunk;

EXTERN	int	exporting;

EXTERN	int	funcdepth;

EXTERN	Node*	funclit;

/*
 *	y.tab.c
 */
int	yyparse(void);

/*
 *	lex.c
 */
void	setfilename(char*);
void	addidir(char*);
void	importfile(Val*);
void	cannedimports(char*, char*);
void	unimportfile();
int32	yylex(void);
void	typeinit(int lex);
void	lexinit(void);
char*	lexname(int);
int32	getr(void);
int	getnsc(void);
int	escchar(int, int*, vlong*);
int	getc(void);
void	ungetc(int);
void	mkpackage(char*);

/*
 *	mparith1.c
 */
int	mpcmpfixflt(Mpint *a, Mpflt *b);
int	mpcmpfltfix(Mpflt *a, Mpint *b);
int	mpcmpfixfix(Mpint *a, Mpint *b);
int	mpcmpfixc(Mpint *b, vlong c);
int	mpcmpfltflt(Mpflt *a, Mpflt *b);
int	mpcmpfltc(Mpflt *b, double c);
void	mpsubfixfix(Mpint *a, Mpint *b);
void	mpsubfltflt(Mpflt *a, Mpflt *b);
void	mpaddcfix(Mpint *a, vlong c);
void	mpaddcflt(Mpflt *a, double c);
void	mpmulcfix(Mpint *a, vlong c);
void	mpmulcflt(Mpflt *a, double c);
void	mpdivfixfix(Mpint *a, Mpint *b);
void	mpmodfixfix(Mpint *a, Mpint *b);
void	mpatofix(Mpint *a, char *s);
void	mpatoflt(Mpflt *a, char *s);
int	mpmovefltfix(Mpint *a, Mpflt *b);
void	mpmovefixflt(Mpflt *a, Mpint *b);
int	Bconv(Fmt*);

/*
 *	mparith2.c
 */
void	mpmovefixfix(Mpint *a, Mpint *b);
void	mpmovecfix(Mpint *a, vlong v);
int	mptestfix(Mpint *a);
void	mpaddfixfix(Mpint *a, Mpint *b);
void	mpmulfixfix(Mpint *a, Mpint *b);
void	mpmulfract(Mpint *a, Mpint *b);
void	mpdivmodfixfix(Mpint *q, Mpint *r, Mpint *n, Mpint *d);
void	mpdivfract(Mpint *a, Mpint *b);
void	mpnegfix(Mpint *a);
void	mpandfixfix(Mpint *a, Mpint *b);
void	mpandnotfixfix(Mpint *a, Mpint *b);
void	mplshfixfix(Mpint *a, Mpint *b);
void	mporfixfix(Mpint *a, Mpint *b);
void	mprshfixfix(Mpint *a, Mpint *b);
void	mpxorfixfix(Mpint *a, Mpint *b);
void	mpcomfix(Mpint *a);
vlong	mpgetfix(Mpint *a);
void	mpshiftfix(Mpint *a, int s);

/*
 *	mparith3.c
 */
int	sigfig(Mpflt *a);
void	mpnorm(Mpflt *a);
void	mpmovefltflt(Mpflt *a, Mpflt *b);
void	mpmovecflt(Mpflt *a, double f);
int	mptestflt(Mpflt *a);
void	mpaddfltflt(Mpflt *a, Mpflt *b);
void	mpmulfltflt(Mpflt *a, Mpflt *b);
void	mpdivfltflt(Mpflt *a, Mpflt *b);
void	mpnegflt(Mpflt *a);
double	mpgetflt(Mpflt *a);
int	Fconv(Fmt*);

/*
 *	subr.c
 */
void*	mal(int32);
void*	remal(void*, int32, int32);
void	errorexit(void);
uint32	stringhash(char*);
Sym*	lookup(char*);
Sym*	pkglookup(char*, char*);
Sym*	opkglookup(char*, char*);
void	importdot(Sym*);
void	yyerror(char*, ...);
void	warn(char*, ...);
void	fatal(char*, ...);
void	linehist(char*, int32, int);
int32	setlineno(Node*);
Node*	nod(int, Node*, Node*);
Node*	nodlit(Val);
Node*	list(Node*, Node*);
Type*	typ(int);
Dcl*	dcl(void);
int	algtype(Type*);
Node*	rev(Node*);
Node*	unrev(Node*);
Node*	appendr(Node*, Node*);
void	dodump(Node*, int);
void	dump(char*, Node*);
Type*	aindex(Node*, Type*);
int	isnil(Node*);
int	isptrto(Type*, int);
int	istype(Type*, int);
int	isfixedarray(Type*);
int	isslice(Type*);
int	isinter(Type*);
int	isnilinter(Type*);
int	isddd(Type*);
Type*	maptype(Type*, Type*);
Type*	methtype(Type*);
Sym*	signame(Type*);
int	eqtype(Type*, Type*);
int	cvttype(Type*, Type*);
int	eqtypenoname(Type*, Type*);
void	argtype(Node*, Type*);
int	eqargs(Type*, Type*);
uint32	typehash(Type*, int, int);
void	frame(int);
Node*	dobad(void);
Node*	nodintconst(int64);
void	nodconst(Node*, Type*, int64);
Node*	nodnil(void);
Node*	nodbool(int);
void	ullmancalc(Node*);
void	badtype(int, Type*, Type*);
Type*	ptrto(Type*);
Node*	cleanidlist(Node*);
Node*	syslook(char*, int);
Node*	treecopy(Node*);
int	isselect(Node*);
void	tempname(Node*, Type*);
Node*	staticname(Type*);
int	iscomposite(Type*);
Node*	callnew(Type*);

Type**	getthis(Type*);
Type**	getoutarg(Type*);
Type**	getinarg(Type*);

Type*	getthisx(Type*);
Type*	getoutargx(Type*);
Type*	getinargx(Type*);

Node*	listfirst(Iter*, Node**);
Node*	listnext(Iter*);
Type*	structfirst(Iter*, Type**);
Type*	structnext(Iter*);
Type*	funcfirst(Iter*, Type*);
Type*	funcnext(Iter*);

int	brcom(int);
int	brrev(int);
void	setmaxarg(Type*);
Sig*	lsort(Sig*, int(*)(Sig*, Sig*));
int	dotoffset(Node*, int*, Node**);
void	tempname(Node*, Type*);

int	Econv(Fmt*);
int	Jconv(Fmt*);
int	Lconv(Fmt*);
int	Oconv(Fmt*);
int	Sconv(Fmt*);
int	Tconv(Fmt*);
int	Nconv(Fmt*);
int	Wconv(Fmt*);
int	Zconv(Fmt*);

int	lookdot0(Sym*, Type*, Type**);
Type*	lookdot1(Sym*, Type*, Type*);
int	adddot1(Sym*, Type*, int, Type**);
Node*	adddot(Node*);
void	expandmeth(Sym*, Type*);
void	genwrapper(Type*, Type*, Sym*);

/*
 *	dcl.c
 */
void	dodclvar(Node*, Type*);
Type*	dodcltype(Type*);
void	updatetype(Type*, Type*);
void	dodclconst(Node*, Node*);
void	defaultlit(Node*, Type*);
void	defaultlit2(Node*, Node*);
int	listcount(Node*);
int	structcount(Type*);
void	addmethod(Node*, Type*, int);
Node*	methodname(Node*, Type*);
Sym*	methodsym(Sym*, Type*);
Type*	functype(Node*, Node*, Node*);
char*	thistypenam(Node*);
void	funcnam(Type*, char*);
Node*	renameinit(Node*);
void	funchdr(Node*);
void	funcargs(Type*);
void	funcbody(Node*);
Type*	dostruct(Node*, int);
Type**	stotype(Node*, int, Type**);
Type*	sortinter(Type*);
void	markdcl(void);
void	popdcl(void);
void	poptodcl(void);
void	dumpdcl(char*);
void	markdclstack(void);
void	testdclstack(void);
Sym*	pushdcl(Sym*);
void	addvar(Node*, Type*, int);
void	addtyp(Type*, int);
void	addconst(Node*, Node*, int);
Node*	fakethis(void);
int	isifacemethod(Type*);
Node*	newname(Sym*);
Node*	oldname(Sym*);
Type*	newtype(Sym*);
Type*	oldtype(Sym*);
void	fninit(Node*);
Node*	nametoanondcl(Node*);
Node*	nametodcl(Node*, Type*);
Node*	anondcl(Type*);
void	checkarglist(Node*);
void	checkwidth(Type*);
void	defercheckwidth(void);
void	resumecheckwidth(void);
Node*	embedded(Sym*);
Node*	variter(Node*, Type*, Node*);
void	constiter(Node*, Type*, Node*);

void	funclit0(Type*);
Node*	funclit1(Type*, Node*);
Node*	unsafenmagic(Node*, Node*);

/*
 * sinit.c
 */

Node*	initfix(Node*);

/*
 *	export.c
 */
void	renameimports(void);
void	autoexport(Sym*);
int	exportname(char*);
void	exportsym(Sym*);
void	packagesym(Sym*);
void	dumpe(Sym*);
void	dumpexport(void);
void	dumpexporttype(Sym*);
void	dumpexportvar(Sym*);
void	dumpexportconst(Sym*);
void	doimportv1(Node*, Node*);
void	doimportc1(Node*, Val*);
void	doimportc2(Node*, Node*, Val*);
void	doimport1(Node*, Node*, Node*);
void	doimport2(Node*, Val*, Node*);
void	doimport3(Node*, Node*);
void	doimport4(Node*, Node*);
void	doimport5(Node*, Val*);
void	doimport6(Node*, Node*);
void	doimport7(Node*, Node*);
void	doimport8(Node*, Val*, Node*);
void	doimport9(Sym*, Node*);
void	importconst(Node *ss, Type *t, Node *v);
void	importmethod(Sym *s, Type *t);
void	importtype(Node *ss, Type *t);
void	importvar(Node *ss, Type *t, int ctxt);
void	checkimports(void);
Type*	pkgtype(char*, char*);

/*
 *	walk.c
 */
void	addtotop(Node*);
void	gettype(Node*, Node*);
void	walk(Node*);
void	walkstate(Node*);
void	walktype(Node*, int);
void	walkconv(Node*);
void	walkas(Node*);
void	walkbool(Node*);
void	walkswitch(Node*);
void	walkselect(Node*);
void	walkdot(Node*);
Node*	ascompatee(int, Node**, Node**);
Node*	ascompatet(int, Node**, Type**, int);
Node*	ascompatte(int, Type**, Node**, int);
int	ascompat(Type*, Type*);
Node*	prcompat(Node*, int);
Node*	nodpanic(int32);
Node*	newcompat(Node*);
Node*	makecompat(Node*);
Node*	stringop(Node*, int);
Type*	fixmap(Type*);
Node*	mapop(Node*, int);
Type*	fixchan(Type*);
Node*	chanop(Node*, int);
Node*	arrayop(Node*, int);
Node*	ifacecvt(Type*, Node*, int);
Node*	ifaceop(Node*);
int	ifaceas(Type*, Type*, int);
int	ifaceas1(Type*, Type*, int);
void	ifacecheck(Type*, Type*, int, int);
void	runifacechecks(void);
Node*	convas(Node*);
void	arrayconv(Type*, Node*);
Node*	colas(Node*, Node*);
Node*	dorange(Node*);
Node*	reorder1(Node*);
Node*	reorder2(Node*);
Node*	reorder3(Node*);
Node*	reorder4(Node*);
Node*	structlit(Node*, Node*);
Node*	arraylit(Node*, Node*);
Node*	maplit(Node*, Node*);
Node*	selectas(Node*, Node*);
Node*	old2new(Node*, Type*);
void	addrescapes(Node*);
void	heapmoves(void);

/*
 *	const.c
 */
void	convlit1(Node*, Type*, int);
void	convlit(Node*, Type*);
void	evconst(Node*);
int	cmpslit(Node *l, Node *r);
int	smallintconst(Node*);
long	nonnegconst(Node*);
int	consttype(Node*);
int	isconst(Node*, int);

/*
 *	align.c
 */
uint32	rnd(uint32, uint32);
void	dowidth(Type*);
int	argsize(Type*);

/*
 *	bits.c
 */
Bits	bor(Bits, Bits);
Bits	band(Bits, Bits);
Bits	bnot(Bits);
int	bany(Bits*);
int	bnum(Bits);
Bits	blsh(uint);
int	beq(Bits, Bits);
int	bset(Bits, uint);
int	Qconv(Fmt *fp);
int	bitno(int32);

/*
 *	gen.c
 */
typedef	struct	Prog	Prog;
#define	P	((Prog*)0)

typedef	struct	Label Label;
struct	Label
{
	uchar	op;		// OGOTO/OLABEL
	Sym*	sym;
	Prog*	label;		// pointer to code
	Prog*	breakpc;	// pointer to code
	Prog*	continpc;	// pointer to code
	Label*	link;
};
#define	L	((Label*)0)

EXTERN	Label*	labellist;
EXTERN	Label*	findlab(Sym*);

typedef	struct	Plist	Plist;
struct	Plist
{
	Node*	name;
	Dcl*	locals;
	Prog*	firstpc;
	int	recur;
	Plist*	link;
};

EXTERN	Plist*	plist;
EXTERN	Plist*	plast;

EXTERN	Prog*	continpc;
EXTERN	Prog*	breakpc;
EXTERN	Prog*	pc;
EXTERN	Prog*	firstpc;

void	allocparams(void);
void	cgen_as(Node *nl, Node *nr);
void	cgen_callmeth(Node *n, int proc);
void	cgen_dcl(Node *n);
void	cgen_proc(Node *n, int proc);
void	checklabels(void);
Label*	findlab(Sym *s);
void	gen(Node *n);
void	newlab(int op, Sym *s);
Node*	sysfunc(char *name);
Plist*	newplist(void);

/*
 *	obj.c
 */
void	Bputdot(Biobuf *b);
void	dumpglobls(void);
void	dumpobj(void);
void	ieeedtod(uint64 *ieee, double native);
void	outhist(Biobuf *b);

/*
 *	arch-specific gen.c/gsubr.c/obj.c
 */
void	betypeinit(void);
vlong	convvtox(vlong, int);
void	compile(Node*);
void	proglist(void);
int	optopop(int);
void	dumpobj(void);
void	dowidth(Type*);
void	argspace(int32);
Node*	nodarg(Type*, int);
Type*	deep(Type*);
Type*	shallow(Type*);
Prog*	gjmp(Prog*);
void	patch(Prog*, Prog*);
void	bgen(Node *n, int true, Prog *to);
void	cgen_asop(Node *n);
void	cgen_call(Node *n, int proc);
void	cgen_callinter(Node *n, Node *res, int proc);
void	cgen_ret(Node *n);
int	isfat(Type*);
void	clearfat(Node *n);
void	cgen(Node*, Node*);
void	gused(Node*);
void	dumpsignatures(void);
void	dumpfuncs(void);
void	dumpdata(void);
void	ggloblnod(Node *nam, int32 width);
void	ggloblsym(Sym *s, int32 width, int dupok);
void	zfile(Biobuf *b, char *p, int n);
void	zhist(Biobuf *b, int line, vlong offset);
void	zname(Biobuf *b, Sym *s, int t);
void	nopout(Prog*);
int	dstringptr(Sym *s, int off, char *str);
int	dsymptr(Sym *s, int off, Sym *x);
int	duint16(Sym *s, int off, uint32 v);
int	duint32(Sym *s, int off, uint32 v);
int	duintptr(Sym *s, int off, uint32 v);
int	duintxx(Sym *s, int off, uint64 v, int wid);
void	genembedtramp(Type*, Sig*);
int	gen_as_init(Node*, Node*);

