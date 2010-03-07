#define SpinVersion	"Spin Version 5.2.4 -- 2 December 2009"
#define PanSource	"ok.11.bug.pr"

#ifdef WIN64
#define ONE_L	((unsigned long) 1)
#define long	long long
#else
#define ONE_L	(1L)
#endif
char *TrailFile = PanSource; /* default */
char *trailfilename;
#if defined(BFS)
#ifndef SAFETY
#define SAFETY
#endif
#ifndef XUSAFE
#define XUSAFE
#endif
#endif
#ifndef uchar
#define uchar	unsigned char
#endif
#ifndef uint
#define uint	unsigned int
#endif
#define DELTA	500
#ifdef MA
	#if NCORE>1 && !defined(SEP_STATE)
	#define SEP_STATE
	#endif
#if MA==1
#undef MA
#define MA	100
#endif
#endif
#ifdef W_XPT
#if W_XPT==1
#undef W_XPT
#define W_XPT 1000000
#endif
#endif
#ifndef NFAIR
#define NFAIR	2	/* must be >= 2 */
#endif
#define HAS_CODE
#define MERGED	1
#if !defined(HAS_LAST) && defined(BCS)
	#define HAS_LAST	1 /* use it, but */
	#ifndef STORE_LAST
		#define NO_LAST	1 /* dont store it */
	#endif
#endif
#if defined(BCS) && defined(BITSTATE)
	#ifndef NO_CTX
		#define STORE_CTX	1
	#endif
#endif
#ifdef NP	/* includes np_ demon */
#define HAS_NP	2
#define VERI	12
#define endclaim	3 /* none */
#endif
typedef struct S_F_MAP {
	char *fnm; int from; int upto;
} S_F_MAP;

#define nstates11	24	/* :init: */
#define endstate11	23
short src_ln11 [] = {
	  0, 355, 355, 356, 357, 358, 358, 358, 
	354, 362, 354, 363, 364, 365, 366, 367, 
	368, 369, 370, 371, 372, 373, 362, 375, 
	  0, };
S_F_MAP src_file11 [] = {
	{ "-", 0, 0 },
	{ "ok.11.bug.pr", 1, 23 },
	{ "-", 24, 25 }
};
uchar reached11 [] = {
	  0,   1,   1,   1,   0,   0,   0,   0, 
	  0,   1,   1,   1,   0,   0,   0,   0, 
	  0,   0,   0,   0,   0,   0,   0,   0, 
	  0, };
uchar *loopstate11;

#define nstates10	14	/* _ZN6Source7computeEv_pnumber_10 */
#define endstate10	13
short src_ln10 [] = {
	  0, 344, 345, 127, 128, 129, 128, 128, 
	126, 130, 130, 347, 341, 348,   0, };
S_F_MAP src_file10 [] = {
	{ "-", 0, 0 },
	{ "ok.11.bug.pr", 1, 13 },
	{ "-", 14, 15 }
};
uchar reached10 [] = {
	  0,   1,   0,   1,   0,   0,   1,   0, 
	  1,   1,   0,   0,   0,   0,   0, };
uchar *loopstate10;

#define nstates9	17	/* _ZN8MyModule7computeEv_pnumber_9 */
#define endstate9	16
short src_ln9 [] = {
	  0, 330, 331, 190, 190, 191, 117, 118, 
	119, 118, 118, 116, 120, 120, 335, 327, 
	336,   0, };
S_F_MAP src_file9 [] = {
	{ "-", 0, 0 },
	{ "ok.11.bug.pr", 1, 16 },
	{ "-", 17, 18 }
};
uchar reached9 [] = {
	  0,   1,   0,   1,   0,   0,   1,   0, 
	  0,   1,   0,   1,   1,   0,   0,   0, 
	  0,   0, };
uchar *loopstate9;

#define nstates8	17	/* _ZN8MyModule7computeEv_pnumber_8 */
#define endstate8	16
short src_ln8 [] = {
	  0, 316, 317, 184, 184, 185, 107, 108, 
	109, 108, 108, 106, 110, 110, 321, 313, 
	322,   0, };
S_F_MAP src_file8 [] = {
	{ "-", 0, 0 },
	{ "ok.11.bug.pr", 1, 16 },
	{ "-", 17, 18 }
};
uchar reached8 [] = {
	  0,   1,   0,   1,   0,   0,   1,   0, 
	  0,   1,   0,   1,   1,   0,   0,   0, 
	  0,   0, };
uchar *loopstate8;

#define nstates7	17	/* _ZN8MyModule7computeEv_pnumber_7 */
#define endstate7	16
short src_ln7 [] = {
	  0, 302, 303, 178, 178, 179,  97,  98, 
	 99,  98,  98,  96, 100, 100, 307, 299, 
	308,   0, };
S_F_MAP src_file7 [] = {
	{ "-", 0, 0 },
	{ "ok.11.bug.pr", 1, 16 },
	{ "-", 17, 18 }
};
uchar reached7 [] = {
	  0,   1,   0,   1,   0,   0,   1,   0, 
	  0,   1,   0,   1,   1,   0,   0,   0, 
	  0,   0, };
uchar *loopstate7;

#define nstates6	17	/* _ZN8MyModule7computeEv_pnumber_6 */
#define endstate6	16
short src_ln6 [] = {
	  0, 288, 289, 172, 172, 173,  87,  88, 
	 89,  88,  88,  86,  90,  90, 293, 285, 
	294,   0, };
S_F_MAP src_file6 [] = {
	{ "-", 0, 0 },
	{ "ok.11.bug.pr", 1, 16 },
	{ "-", 17, 18 }
};
uchar reached6 [] = {
	  0,   1,   0,   1,   0,   0,   1,   0, 
	  0,   1,   0,   1,   1,   0,   0,   0, 
	  0,   0, };
uchar *loopstate6;

#define nstates5	17	/* _ZN8MyModule7computeEv_pnumber_5 */
#define endstate5	16
short src_ln5 [] = {
	  0, 274, 275, 166, 166, 167,  77,  78, 
	 79,  78,  78,  76,  80,  80, 279, 271, 
	280,   0, };
S_F_MAP src_file5 [] = {
	{ "-", 0, 0 },
	{ "ok.11.bug.pr", 1, 16 },
	{ "-", 17, 18 }
};
uchar reached5 [] = {
	  0,   1,   0,   1,   0,   0,   1,   0, 
	  0,   1,   0,   1,   1,   0,   0,   0, 
	  0,   0, };
uchar *loopstate5;

#define nstates4	17	/* _ZN8MyModule7computeEv_pnumber_4 */
#define endstate4	16
short src_ln4 [] = {
	  0, 260, 261, 160, 160, 161,  67,  68, 
	 69,  68,  68,  66,  70,  70, 265, 257, 
	266,   0, };
S_F_MAP src_file4 [] = {
	{ "-", 0, 0 },
	{ "ok.11.bug.pr", 1, 16 },
	{ "-", 17, 18 }
};
uchar reached4 [] = {
	  0,   1,   0,   1,   0,   0,   1,   0, 
	  0,   1,   0,   1,   1,   0,   0,   0, 
	  0,   0, };
uchar *loopstate4;

#define nstates3	17	/* _ZN8MyModule7computeEv_pnumber_3 */
#define endstate3	16
short src_ln3 [] = {
	  0, 246, 247, 154, 154, 155,  57,  58, 
	 59,  58,  58,  56,  60,  60, 251, 243, 
	252,   0, };
S_F_MAP src_file3 [] = {
	{ "-", 0, 0 },
	{ "ok.11.bug.pr", 1, 16 },
	{ "-", 17, 18 }
};
uchar reached3 [] = {
	  0,   1,   0,   1,   0,   0,   1,   0, 
	  0,   1,   0,   1,   1,   0,   0,   0, 
	  0,   0, };
uchar *loopstate3;

#define nstates2	17	/* _ZN8MyModule7computeEv_pnumber_2 */
#define endstate2	16
short src_ln2 [] = {
	  0, 232, 233, 148, 148, 149,  47,  48, 
	 49,  48,  48,  46,  50,  50, 237, 229, 
	238,   0, };
S_F_MAP src_file2 [] = {
	{ "-", 0, 0 },
	{ "ok.11.bug.pr", 1, 16 },
	{ "-", 17, 18 }
};
uchar reached2 [] = {
	  0,   1,   0,   1,   0,   0,   1,   0, 
	  0,   1,   0,   1,   1,   0,   0,   0, 
	  0,   0, };
uchar *loopstate2;

#define nstates1	17	/* _ZN8MyModule7computeEv_pnumber_1 */
#define endstate1	16
short src_ln1 [] = {
	  0, 218, 219, 142, 142, 143,  37,  38, 
	 39,  38,  38,  36,  40,  40, 223, 215, 
	224,   0, };
S_F_MAP src_file1 [] = {
	{ "-", 0, 0 },
	{ "ok.11.bug.pr", 1, 16 },
	{ "-", 17, 18 }
};
uchar reached1 [] = {
	  0,   1,   0,   1,   0,   0,   1,   0, 
	  0,   1,   0,   1,   1,   0,   0,   0, 
	  0,   0, };
uchar *loopstate1;

#define nstates0	17	/* _ZN8MyModule7computeEv_pnumber_0 */
#define endstate0	16
short src_ln0 [] = {
	  0, 203, 204, 136, 136, 137,  37,  38, 
	 39,  38,  38,  36,  40,  40, 208, 200, 
	210,   0, };
S_F_MAP src_file0 [] = {
	{ "-", 0, 0 },
	{ "ok.11.bug.pr", 1, 16 },
	{ "-", 17, 18 }
};
uchar reached0 [] = {
	  0,   1,   0,   1,   0,   0,   1,   0, 
	  0,   1,   0,   1,   1,   0,   0,   0, 
	  0,   0, };
uchar *loopstate0;
struct {
	int tp; short *src;
} src_all[] = {
	{ 11, &src_ln11[0] },
	{ 10, &src_ln10[0] },
	{ 9, &src_ln9[0] },
	{ 8, &src_ln8[0] },
	{ 7, &src_ln7[0] },
	{ 6, &src_ln6[0] },
	{ 5, &src_ln5[0] },
	{ 4, &src_ln4[0] },
	{ 3, &src_ln3[0] },
	{ 2, &src_ln2[0] },
	{ 1, &src_ln1[0] },
	{ 0, &src_ln0[0] },
	{ 0, (short *) 0 }
};
short *frm_st0;
struct {
	char *c; char *t;
} code_lookup[] = {
	{ (char *) 0, "" }
};
#define _T5	95
#define _T2	96
#define T_ID	unsigned char
#define SYNC	0
#define ASYNC	0

#ifndef NCORE
	#ifdef DUAL_CORE
		#define NCORE	2
	#elif QUAD_CORE
		#define NCORE	4
	#else
		#define NCORE	1
	#endif
#endif
char *procname[] = {
   "_ZN8MyModule7computeEv_pnumber_0",
   "_ZN8MyModule7computeEv_pnumber_1",
   "_ZN8MyModule7computeEv_pnumber_2",
   "_ZN8MyModule7computeEv_pnumber_3",
   "_ZN8MyModule7computeEv_pnumber_4",
   "_ZN8MyModule7computeEv_pnumber_5",
   "_ZN8MyModule7computeEv_pnumber_6",
   "_ZN8MyModule7computeEv_pnumber_7",
   "_ZN8MyModule7computeEv_pnumber_8",
   "_ZN8MyModule7computeEv_pnumber_9",
   "_ZN6Source7computeEv_pnumber_10",
   ":init:",
   ":np_:",
};

#define Pinit	((P11 *)this)
typedef struct P11 { /* :init: */
	unsigned _pid : 8;  /* 0..255 */
	unsigned _t   : 5; /* proctype */
	unsigned _p   : 6; /* state    */
	int i;
} P11;
#define Air11	(sizeof(P11) - Offsetof(P11, i) - 1*sizeof(int))
#define P_ZN6Source7computeEv_pnumber_10	((P10 *)this)
typedef struct P10 { /* _ZN6Source7computeEv_pnumber_10 */
	unsigned _pid : 8;  /* 0..255 */
	unsigned _t   : 5; /* proctype */
	unsigned _p   : 6; /* state    */
	int llvm_cbe_alloca_20_point;
} P10;
#define Air10	(sizeof(P10) - Offsetof(P10, llvm_cbe_alloca_20_point) - 1*sizeof(int))
#define P_ZN8MyModule7computeEv_pnumber_9	((P9 *)this)
typedef struct P9 { /* _ZN8MyModule7computeEv_pnumber_9 */
	unsigned _pid : 8;  /* 0..255 */
	unsigned _t   : 5; /* proctype */
	unsigned _p   : 6; /* state    */
	int llvm_cbe_alloca_20_point;
} P9;
#define Air9	(sizeof(P9) - Offsetof(P9, llvm_cbe_alloca_20_point) - 1*sizeof(int))
#define P_ZN8MyModule7computeEv_pnumber_8	((P8 *)this)
typedef struct P8 { /* _ZN8MyModule7computeEv_pnumber_8 */
	unsigned _pid : 8;  /* 0..255 */
	unsigned _t   : 5; /* proctype */
	unsigned _p   : 6; /* state    */
	int llvm_cbe_alloca_20_point;
} P8;
#define Air8	(sizeof(P8) - Offsetof(P8, llvm_cbe_alloca_20_point) - 1*sizeof(int))
#define P_ZN8MyModule7computeEv_pnumber_7	((P7 *)this)
typedef struct P7 { /* _ZN8MyModule7computeEv_pnumber_7 */
	unsigned _pid : 8;  /* 0..255 */
	unsigned _t   : 5; /* proctype */
	unsigned _p   : 6; /* state    */
	int llvm_cbe_alloca_20_point;
} P7;
#define Air7	(sizeof(P7) - Offsetof(P7, llvm_cbe_alloca_20_point) - 1*sizeof(int))
#define P_ZN8MyModule7computeEv_pnumber_6	((P6 *)this)
typedef struct P6 { /* _ZN8MyModule7computeEv_pnumber_6 */
	unsigned _pid : 8;  /* 0..255 */
	unsigned _t   : 5; /* proctype */
	unsigned _p   : 6; /* state    */
	int llvm_cbe_alloca_20_point;
} P6;
#define Air6	(sizeof(P6) - Offsetof(P6, llvm_cbe_alloca_20_point) - 1*sizeof(int))
#define P_ZN8MyModule7computeEv_pnumber_5	((P5 *)this)
typedef struct P5 { /* _ZN8MyModule7computeEv_pnumber_5 */
	unsigned _pid : 8;  /* 0..255 */
	unsigned _t   : 5; /* proctype */
	unsigned _p   : 6; /* state    */
	int llvm_cbe_alloca_20_point;
} P5;
#define Air5	(sizeof(P5) - Offsetof(P5, llvm_cbe_alloca_20_point) - 1*sizeof(int))
#define P_ZN8MyModule7computeEv_pnumber_4	((P4 *)this)
typedef struct P4 { /* _ZN8MyModule7computeEv_pnumber_4 */
	unsigned _pid : 8;  /* 0..255 */
	unsigned _t   : 5; /* proctype */
	unsigned _p   : 6; /* state    */
	int llvm_cbe_alloca_20_point;
} P4;
#define Air4	(sizeof(P4) - Offsetof(P4, llvm_cbe_alloca_20_point) - 1*sizeof(int))
#define P_ZN8MyModule7computeEv_pnumber_3	((P3 *)this)
typedef struct P3 { /* _ZN8MyModule7computeEv_pnumber_3 */
	unsigned _pid : 8;  /* 0..255 */
	unsigned _t   : 5; /* proctype */
	unsigned _p   : 6; /* state    */
	int llvm_cbe_alloca_20_point;
} P3;
#define Air3	(sizeof(P3) - Offsetof(P3, llvm_cbe_alloca_20_point) - 1*sizeof(int))
#define P_ZN8MyModule7computeEv_pnumber_2	((P2 *)this)
typedef struct P2 { /* _ZN8MyModule7computeEv_pnumber_2 */
	unsigned _pid : 8;  /* 0..255 */
	unsigned _t   : 5; /* proctype */
	unsigned _p   : 6; /* state    */
	int llvm_cbe_alloca_20_point;
} P2;
#define Air2	(sizeof(P2) - Offsetof(P2, llvm_cbe_alloca_20_point) - 1*sizeof(int))
#define P_ZN8MyModule7computeEv_pnumber_1	((P1 *)this)
typedef struct P1 { /* _ZN8MyModule7computeEv_pnumber_1 */
	unsigned _pid : 8;  /* 0..255 */
	unsigned _t   : 5; /* proctype */
	unsigned _p   : 6; /* state    */
	int llvm_cbe_alloca_20_point;
} P1;
#define Air1	(sizeof(P1) - Offsetof(P1, llvm_cbe_alloca_20_point) - 1*sizeof(int))
#define P_ZN8MyModule7computeEv_pnumber_0	((P0 *)this)
typedef struct P0 { /* _ZN8MyModule7computeEv_pnumber_0 */
	unsigned _pid : 8;  /* 0..255 */
	unsigned _t   : 5; /* proctype */
	unsigned _p   : 6; /* state    */
	int llvm_cbe_alloca_20_point;
} P0;
#define Air0	(sizeof(P0) - Offsetof(P0, llvm_cbe_alloca_20_point) - 1*sizeof(int))
typedef struct P12 { /* np_ */
	unsigned _pid : 8;  /* 0..255 */
	unsigned _t   : 5; /* proctype */
	unsigned _p   : 6; /* state    */
} P12;
#define Air12	(sizeof(P12) - 3)
#if defined(BFS) && defined(REACH)
#undef REACH
#endif
#ifdef VERI
#define BASE	1
#else
#define BASE	0
#endif
typedef struct Trans {
	short atom;	/* if &2 = atomic trans; if &8 local */
#ifdef HAS_UNLESS
	short escp[HAS_UNLESS];	/* lists the escape states */
	short e_trans;	/* if set, this is an escp-trans */
#endif
	short tpe[2];	/* class of operation (for reduction) */
	short qu[6];	/* for conditional selections: qid's  */
	uchar ty[6];	/* ditto: type's */
#ifdef NIBIS
	short om;	/* completion status of preselects */
#endif
	char *tp;	/* src txt of statement */
	int st;		/* the nextstate */
	int t_id;	/* transition id, unique within proc */
	int forw;	/* index forward transition */
	int back;	/* index return  transition */
	struct Trans *nxt;
} Trans;

#define qptr(x)	(((uchar *)&now)+(int)q_offset[x])
#define pptr(x)	(((uchar *)&now)+(int)proc_offset[x])
extern uchar *Pptr(int);
#define q_sz(x)	(((Q0 *)qptr(x))->Qlen)

#ifndef VECTORSZ
#define VECTORSZ	1024           /* sv   size in bytes */
#endif

#define WS	4 /* word size in bytes */
#ifdef VERBOSE
#ifndef CHECK
#define CHECK
#endif
#ifndef DEBUG
#define DEBUG
#endif
#endif
#ifdef SAFETY
#ifndef NOFAIR
#define NOFAIR
#endif
#endif
#ifdef NOREDUCE
#ifndef XUSAFE
#define XUSAFE
#endif
#if !defined(SAFETY) && !defined(MA)
#define FULLSTACK
#endif
#else
#ifdef BITSTATE
#if defined(SAFETY) && !defined(HASH64)
#define CNTRSTACK
#else
#define FULLSTACK
#endif
#else
#define FULLSTACK
#endif
#endif
#ifdef BITSTATE
#ifndef NOCOMP
#define NOCOMP
#endif
#if !defined(LC) && defined(SC)
#define LC
#endif
#endif
#if defined(COLLAPSE2) || defined(COLLAPSE3) || defined(COLLAPSE4)
/* accept the above for backward compatibility */
#define COLLAPSE
#endif
#ifdef HC
#undef HC
#define HC4
#endif
#ifdef HC0
#define HC	0
#endif
#ifdef HC1
#define HC	1
#endif
#ifdef HC2
#define HC	2
#endif
#ifdef HC3
#define HC	3
#endif
#ifdef HC4
#define HC	4
#endif
#ifdef COLLAPSE
#if NCORE>1 && !defined(SEP_STATE)
unsigned long *ncomps;	/* in shared memory */
#else
unsigned long ncomps[256+2];
#endif
#endif
#define MAXQ   	255
#define MAXPROC	255

typedef struct Stack  {	 /* for queues and processes */
#if VECTORSZ>32000
	int o_delta;
	int o_offset;
	int o_skip;
	int o_delqs;
#else
	short o_delta;
	short o_offset;
	short o_skip;
	short o_delqs;
#endif
	short o_boq;
#ifndef XUSAFE
	char *o_name;
#endif
	char *body;
	struct Stack *nxt;
	struct Stack *lst;
} Stack;

typedef struct Svtack { /* for complete state vector */
#if VECTORSZ>32000
	int o_delta;
	int m_delta;
#else
	short o_delta;	 /* current size of frame */
	short m_delta;	 /* maximum size of frame */
#endif
#if SYNC
	short o_boq;
#endif
#define StackSize	(WS)
	char *body;
	struct Svtack *nxt;
	struct Svtack *lst;
} Svtack;

Trans ***trans;	/* 1 ptr per state per proctype */

struct H_el *Lstate;
int depthfound = -1;	/* loop detection */
#if VECTORSZ>32000
int proc_offset[MAXPROC];
int q_offset[MAXQ];
#else
short proc_offset[MAXPROC];
short q_offset[MAXQ];
#endif
uchar proc_skip[MAXPROC];
uchar q_skip[MAXQ];
unsigned long  vsize;	/* vector size in bytes */
#ifdef SVDUMP
int vprefix=0, svfd;		/* runtime option -pN */
#endif
char *tprefix = "trail";	/* runtime option -tsuffix */
short boq = -1;		/* blocked_on_queue status */
typedef struct State {
	uchar _nr_pr;
	uchar _nr_qs;
	uchar   _a_t;	/* cycle detection */
#ifndef NOFAIR
	uchar   _cnt[NFAIR];	/* counters, weak fairness */
#endif
#ifndef NOVSZ
#if VECTORSZ<65536
	unsigned short _vsz;
#else
	unsigned long  _vsz;
#endif
#endif
#ifdef HAS_LAST
	uchar  _last;	/* pid executed in last step */
#endif
#if defined(BITSTATE) && defined(BCS) && defined(STORE_CTX)
	uchar  _ctx;	/* nr of context switches so far */
#endif
#ifdef EVENT_TRACE
#if nstates_event<256
	uchar _event;
#else
	unsigned short _event;
#endif
#endif
	int T[11];
	int e[11];
	uchar sv[VECTORSZ];
} State;

#define HAS_TRACK	0
/* hidden variable: */	uchar finished[11];
int _; /* a predefined write-only variable */

#define FORWARD_MOVES	"pan.m"
#define REVERSE_MOVES	"pan.b"
#define TRANSITIONS	"pan.t"
#define _NP_	12
uchar reached12[3];  /* np_ */
uchar *loopstate12;  /* np_ */
#define nstates12	3 /* np_ */
#define endstate12	2 /* np_ */

#define start12	0 /* np_ */
#define start11	8
#define start10	12
#define start9	15
#define start8	15
#define start7	15
#define start6	15
#define start5	15
#define start4	15
#define start3	15
#define start2	15
#define start1	15
#define start0	15
#ifdef NP
	#define ACCEPT_LAB	1 /* at least 1 in np_ */
#else
	#define ACCEPT_LAB	0 /* user-defined accept labels */
#endif
#ifdef MEMCNT
	#ifdef MEMLIM
		#warning -DMEMLIM takes precedence over -DMEMCNT
		#undef MEMCNT
	#else
		#if MEMCNT<20
			#warning using minimal value -DMEMCNT=20 (=1MB)
			#define MEMLIM	(1)
			#undef MEMCNT
		#else
			#if MEMCNT==20
				#define MEMLIM	(1)
				#undef MEMCNT
			#else
			 #if MEMCNT>=50
			  #error excessive value for MEMCNT
			 #else
				#define MEMLIM	(1<<(MEMCNT-20))
			 #endif
			#endif
		#endif
	#endif
#endif
#if NCORE>1 && !defined(MEMLIM)
	#define MEMLIM	(2048)	/* need a default, using 2 GB */
#endif
#define PROG_LAB	0 /* progress labels */
uchar *accpstate[13];
uchar *progstate[13];
uchar *loopstate[13];
uchar *reached[13];
uchar *stopstate[13];
uchar *visstate[13];
short *mapstate[13];
#ifdef HAS_CODE
int NrStates[13];
#endif
#define NQS	0
short q_flds[1];
short q_max[1];
typedef struct Q0 {	/* generic q */
	uchar Qlen;	/* q_size */
	uchar _t;
} Q0;

/** function prototypes **/
char *emalloc(unsigned long);
char *Malloc(unsigned long);
int Boundcheck(int, int, int, int, Trans *);
int addqueue(int, int);
/* int atoi(char *); */
/* int abort(void); */
int close(int);
int delproc(int, int);
int endstate(void);
int hstore(char *, int);
#ifdef MA
int gstore(char *, int, uchar);
#endif
int q_cond(short, Trans *);
int q_full(int);
int q_len(int);
int q_zero(int);
int qrecv(int, int, int, int);
int unsend(int);
/* void *sbrk(int); */
void Uerror(char *);
void assert(int, char *, int, int, Trans *);
void c_chandump(int);
void c_globals(void);
void c_locals(int, int);
void checkcycles(void);
void crack(int, int, Trans *, short *);
void d_sfh(const char *, int);
void sfh(const char *, int);
void d_hash(uchar *, int);
void s_hash(uchar *, int);
void r_hash(uchar *, int);
void delq(int);
void do_reach(void);
void pan_exit(int);
void exit(int);
void hinit(void);
void imed(Trans *, int, int, int);
void new_state(void);
void p_restor(int);
void putpeg(int, int);
void putrail(void);
void q_restor(void);
void retrans(int, int, int, short *, uchar *, uchar *);
void settable(void);
void setq_claim(int, int, char *, int, char *);
void sv_restor(void);
void sv_save(void);
void tagtable(int, int, int, short *, uchar *);
void do_dfs(int, int, int, short *, uchar *, uchar *);
void uerror(char *);
void unrecv(int, int, int, int, int);
void usage(FILE *);
void wrap_stats(void);
#if defined(FULLSTACK) && defined(BITSTATE)
int  onstack_now(void);
void onstack_init(void);
void onstack_put(void);
void onstack_zap(void);
#endif
#ifndef XUSAFE
int q_S_check(int, int);
int q_R_check(int, int);
uchar q_claim[MAXQ+1];
char *q_name[MAXQ+1];
char *p_name[MAXPROC+1];
#endif
void qsend(int, int, int);
#define Addproc(x)	addproc(x)
#define LOCAL	1
#define Q_FULL_F	2
#define Q_EMPT_F	3
#define Q_EMPT_T	4
#define Q_FULL_T	5
#define TIMEOUT_F	6
#define GLOBAL	7
#define BAD	8
#define ALPHA_F	9
#define NTRANS	97
#ifdef PEG
long peg[NTRANS];
#endif
