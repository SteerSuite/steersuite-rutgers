/*CCCCCCCCCCCCCCCCCCCCCCCC MIDACO C/C++ HEADER CCCCCCCCCCCCCCCCCCCCCCCCC
C                           
C     _|      _|  _|_|_|  _|_|_|      _|_|      _|_|_|    _|_|    
C     _|_|  _|_|    _|    _|    _|  _|    _|  _|        _|    _|  
C     _|  _|  _|    _|    _|    _|  _|_|_|_|  _|        _|    _|  
C     _|      _|    _|    _|    _|  _|    _|  _|        _|    _|  
C     _|      _|  _|_|_|  _|_|_|    _|    _|    _|_|_|    _|_|  
C
C                                          Version 4.0 (limited)
C                                                           
C    MIDACO - Mixed Integer Distributed Ant Colony Optimization
C    ----------------------------------------------------------
C
C    MIDACO solves the general Mixed Integer Non-Linear Program (MINLP):
C
C
C       Minimize     F(X)           where X(1,...N-NI)   is *CONTINUOUS*
C                                   and   X(N-NI+1,...N) is *DISCRETE* 
C
C       Subject to:  G_j(X)  =  0   (j=1,...ME)     Equality Constraints
C                    G_j(X) >=  0   (j=ME+1,...M) Inequality Constraints
C
C       And bounds:  XL <= X <= XU
C
C
C    MIDACO is a (heuristic) global optimization solver that stochastically 
C    approximates a solution 'X' to the above displayed MINLP problem. MIDACO
C    is based on an extended Ant Colony Optimization framework (see [1]) in 
C    combination with the Oracle Penalty Method (see [2]) for constraints 'G(X)'.
C
C    In case of mixed integer problems, the continuous variables are stored 
C    first in 'X(1,...N-NI)', while the discrete (also called integer or 
C    combinatorial) variables are stored last in 'X(N-NI+1,...N)'.
C    As an example consider:
C
C       X = (  0.1234,  5.6789,  1.0000,  2.0000,  3.0000)   
C
C       where 'N' = 5 and 'NI' = 3
C
C    Note that all 'X' is of type double precision. Equality and inequality 
C    constraints are handled in a similar way. The vector 'G' stores at first 
C    the 'ME' equality constraints and behind those, the remaining 'M-ME' 
C    inequality constraints are stored. 
C
C    MIDACO is a derivative free black box solver and does not require the 
C    relaxation of integer variables (this means, integer variables are 
C    treated as categorical variables). MIDACO does not require any user 
C    specified parameter tuning as it can run completely on 'Autopilot'
C    (all parameter set equal to zero). Optionally, the user can adjust 
C    the MIDACO performance by setting some parameters explained below.
C
C
C    List of MIDACO subroutine arguments:
C    ------------------------------------
C
C    P  :   (Parallelization Factor)
C            If no parallelization is desired, set P = 1.
C
C    N  :   Number of optimization variables in total (continuous and integer ones). 
C           'N' is the dimension of the iterate 'X' with X = (X_1,...,X_N).
C
C    NI :   Number of integer optimization variables. 'NI' <= 'N'.
C           Integer (discrete) variables must be stored at the end of 'X'.
C     
C    M  :   Number of constraints in total (equality and inequality ones).
C           'P*M' is the dimension of a constraint vector 'G' with G = (G_1,...,G_M).
C
C    ME :   Number of equality constraints. 'ME' <= 'M'.
C           Equality constraints are stored in the beginning of 'G'. 
C           Inequality constraints are stored in the end of 'G'.
c
C    X(P*N) :   Array containing the iterate 'X'. For P=1 (no parallelization)
C               'X' stores only one iterate and has length 'N'. For P>1 
C               'X' contains several iterates, which are stored one after
C               another.
C
C    F(P)   :   Array containing the objective function value 'F' corresponding
C               to the iterates 'X'. For P=1 (no parallelization), 'F' is a single
C               value. For P>1 F stores several values, corresponding to 'X' one 
C               after another.
C
C    G(P*M) :   Array containing the constraint values 'G'.For P=1 (no parallelization) 
C               'G' has length 'M'. For P>1 'G' has length 'P*M' and stores the vectors
C               of constraints, corresponding to 'X' one after another.
C
C     XL(N) :   Array containing the lower bounds for the iterates 'X'.
C               Note that for integer dimensions (i > N-NI) the bounds should also be 
C               discrete, e.g. X(i) = 1.0000.
C
C     XU(N) :   Array containing the upper bounds for the iterates 'X'.
C               Note that for integer dimensions (i > N-NI) the bounds should also be 
C               discrete, e.g. X(i) = 1.0000.
C
C    IFLAG :    Information flag used by MIDACO. Initially MIDACO must be called with IFLAG=0.
C               If MIDACO works correctly, IFLAG flags lower than 0 are used for internal 
C               communication. If MIDACO stops (either by submitting ISTOP=1 or automatically
C               by the FSTOP or AUTOSTOP parameter), an IFLAG value between 1 and 9 is returned 
C               as final message. If MIDACO detects at start-up some critical problem setup, a 
C               ***WARNING*** message is returned by IFLAG as value between 10 and 99. If
C               MIDACO detects an ***MIDACO INPUT ERROR***, an IFLAG value between 100 and 999 
C               is returned and MIDACO stops. The individual IFLAG flags are as follows:
C
C               Final Messages:
C               ---------------
C               IFLAG = 1 : Feasible solution,   MIDACO was stopped by MAXEVAL or MAXTIME
C               IFLAG = 2 : Infeasible solution, MIDACO was stopped by MAXEVAL or MAXTIME
C               IFLAG = 3 : Feasible solution,   MIDACO stopped automatically by AUTOSTOP
C               IFLAG = 4 : Infeasible solution, MIDACO stopped automatically by AUTOSTOP
C               IFLAG = 5 : Feasible solution,   MIDACO stopped automatically by FSTOP
C       
C               WARNING - Flags:
C               ----------------
C               IFLAG = 51 : Some X(i)  is greater/lower than +/- 1.0D+8 (try to avoid huge values!)
C               IFLAG = 52 : Some XL(i) is greater/lower than +/- 1.0D+8 (try to avoid huge values!)
C               IFLAG = 53 : Some XU(i) is greater/lower than +/- 1.0D+8 (try to avoid huge values!)
C
C               IFLAG = 61 : Some X(i)  should be discrete (e.g. 1.0), but is continuous (e.g. 1.234)
C               IFLAG = 62 : Some XL(i) should be discrete (e.g. 1.0), but is continuous (e.g. 1.234)
C               IFLAG = 63 : Some XU(i) should be discrete (e.g. 1.0), but is continuous (e.g. 1.234)
C
C               IFLAG = 71 : Some XL(i) = XU(i) (fixed variable)
C
C               IFLAG = 81 : F(X) has value NaN for starting point X
C               IFLAG = 82 : Some G(X) has value NaN for starting point X 
C
C               IFLAG = 91 : FSTOP is greater/lower than +/- 1.0D+8
C               IFLAG = 92 : ORACLE is greater/lower than +/- 1.0D+8
C
C               ERROR - Flags:
C               --------------
C               IFLAG = 101 :   P   <= 0   or   P  > 1.0D+6
C               IFLAG = 102 :   N   <= 0   or   N  > 1.0D+6
C               IFLAG = 103 :   NI  <  0
C               IFLAG = 104 :   NI  >  N
C               IFLAG = 105 :   M   <  0   or   M  > 1.0D+6
C               IFLAG = 106 :   ME  <  0
C               IFLAG = 107 :   ME  >  M
C
C               IFLAG = 201 :   some X(i)  has type NaN
C               IFLAG = 202 :   some XL(i) has type NaN
C               IFLAG = 203 :   some XU(i) has type NaN
C               IFLAG = 204 :   some X(i) < XL(i)
C               IFLAG = 205 :   some X(i) > XU(i)
C               IFLAG = 206 :   some XL(i) > XU(i)
C           
C               IFLAG = 301 :   PARAM(1) < 0   or   PARAM(1) > 1.0D+6
C               IFLAG = 302 :   PARAM(2) < 0   or   PARAM(2) > 1.0D+12
C               IFLAG = 303 :   PARAM(3) greater/lower than +/- 1.0D+12
C               IFLAG = 304 :   PARAM(4) < 0   or   PARAM(4) > 1.0D+6
C               IFLAG = 305 :   PARAM(5) greater/lower than +/- 1.0D+12
C               IFLAG = 306 :   |PARAM(6)| < 1   or   PARAM(6) > 1.0D+12
C               IFLAG = 307 :   PARAM(7) < 0   or   PARAM(7) > 1.0D+8
C               IFLAG = 308 :   PARAM(8) < 0   or   PARAM(8) > 100
C               IFLAG = 309 :   PARAM(7) < PARAM(8)
C               IFLAG = 310 :   PARAM(7) > 0 but PARAM(8) = 0
C               IFLAG = 311 :   PARAM(8) > 0 but PARAM(7) = 0
C               IFLAG = 312 :   PARAM(9) < 0   or   PARAM(9) > 1000
C               IFLAG = 313 :   Some PARAM(i) has type NaN
C
C               IFLAG = 401 :   ISTOP < 0 or ISTOP > 1
C
C               IFLAG = 501 :   Double precision work space size LRW is too small.               
C                               ---> RW must be at least of size LRW = 200*N+2*M+1000
C
C               IFLAG = 601 :   Integer work space size LIW is too small.
C                               ---> IW must be at least of size LIW = 2*N+P+1000
C
C               IFLAG = 701 :   Input check failed! MIDACO must be called initially with IFLAG = 0
C
C               IFLAG = 801 :   P > PMAX (user must increase PMAX in the MIDACO source code) 
C               IFLAG = 802 :   P*M+1 > PXM (user must increase PXM in the MIDACO source code)
C
C               IFLAG = 900 :   Invalid or corrupted LICENSE-KEY
C
C               IFLAG = 999 :   N > 4. The free test version is limited up to 4 variables.
C
C    ISTOP :   Communication flag to stop MIDACO. If MIDACO is called with 
C              ISTOP = 1, MIDACO returns the best found solution in 'X' with 
C              corresponding 'F' and 'G'. As long as MIDACO should continue 
C              its search, ISTOP must be equal to 0.
C
C    PARAM() :  Array containing 9 parameters that can be selected by the user to adjust MIDACO. 
C               (See the user manual for a more detailed description of individual parameters) 
C
C     PARAM(1) :  [ACCURACY] Accuracy for constraint violation (measured as the L1-Norm over G(X)).
C                 If PARAM(1) is set to 0, MIDACO uses a default accuracy of 0.001. If the user
C                 desires a more precise accuracy, set PARAM(1) = 0.000001 for example. If the
C                 user desires a less precise accuracy, set PARAM(1) = 0.05 for example. 
C
C     PARAM(2) :  [RANDOM-SEED] Random seed used for MIDACO's internal pseudo random number
C                 generator. Value must be a positive discrete value, e.g. PARAM(2) = 1,2,3,... 
C                 The default random seed for MIDACO is zero.
C
C     PARAM(3) :  [FSTOP] Stopping criteria for MIDACO. MIDACO will stop, if a feasible
c                 solution 'X' with F(X) <= FSTOP is found. FSTOP is disabled, when FSTOP = 0. 
C                 In case the user wishes to use zero as FSTOP, a tiny value (e.g. 0.000001) 
C                 should be used instead.
C
C     PARAM(4) :  [AUTOSTOP] Automatic stopping criteria within MIDACO. AUTOSTOP defines the
C                 number of successive internal MIDACO restarts, after which no progress was
C                 made on the objective function. AUTOSTOP must be a (discrete) value >= 0.
C                 Some (very rough) examples for AUTOSTOP values are: 
C                 AUTOSTOP = 1 (lowest chance of global optimality, but fastest runtime)
C                 AUTOSTOP = 10 (medium chance of global optimality)
C                 AUTOSTOP = 50 (high chance of global optimality)
C                 AUTOSTOP = 500 (very high chance of global optimality, but very long runtime)
C                 AUTOSTOP = 0 disables this stopping criteria.
C
C     PARAM(5) :  [ORACLE] This parameter affects only constrained problems. If PARAM(4)=0.0,
C                 MIDACO will use its internal oracle strategy. If PARAM(4) is not equal to 0.0, 
C                 MIDACO will use PARAM(5) as initial oracle for its in-build oracle penalty 
C                 function. In case the user wishes to use zero as ORACLE, a tiny value 
C                 (e.g. 0.000001) should be used as ORACLE. Note: For most problems it is 
C                 better to overestimate an oracle, instead of underestimating it.
C
C     PARAM(6) :  [FOCUS] This parameter focuses the MIDACO search process around the
C                 current best solution (which might be the starting point).
C
C     PARAM(7) :  [ANTS] Number of ants (stochastically sampled iterates) used by MIDACO 
C                 in every generation. ANTS must be greater or equal to KERNEL.
C
C     PARAM(8) :  [KERNEL] Solution archive size of MIDACO. KERNEL must be lower or equal to ANTS.
C
C     PARAM(9) :  [CHARACTER] This parameter allows the user to use highly customized MIDACO
C                 internal parameter settings for specific problems. This parameter is currently
C                 only available as a service. In case you are interested in this service, please
C                 contact info@midaco-solver.com.
C
C
C    RW(LRW) :  Real workarray (Type: double precision) of length 'LRW'
C    LRW :      Length of 'RW'. 'LRW' must be greater or equal to  200*N+2*M+1000
C
C    IW(LIW) :  Integer workarray (Type: long integer) of length 'LIW'
C    LIW :      Length of 'IW'. 'LIW' must be greater or equal to  2*N+P+1000
C
C    KEY :  License-Key for MIDACO. Note that any licensed copy of MIDACO comes with an 
C           individual 'KEY' determining the license owner and its affiliation.  
C
C
C    References:
C    -----------
C
C    [1] Schlueter, M., Egea, J. A., Banga, J. R.: 
C        "Extended ant colony optimization for non-convex mixed integer nonlinear programming", 
C        Computers & Operations Research (Elsevier), Vol. 36 , Issue 7, Page 2217-2229, 2009.
C
C    [2] Schlueter M., Gerdts M.: "The oracle penalty method",
C        Journal of Global Optimization (Springer), Vol. 47(2),pp 293-325, 2010.
C
C
C    Author (C) :   Dr. Martin Schlueter
C                   Information Initiative Center,
C                   Division of Large Scale Computing Systems,
C                   Hokkaido University, JAPAN.
C
C    Email :        info@midaco-solver.com
C    URL :          http://www.midaco-solver.com
C       
CCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCC
CCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCC
CCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCC

      MIDACO( P, N, NI, M, ME, X, F, G, XL, XU, 
              IFLAG, ISTOP, PARAM, RW, LRW, IW, LIW, KEY)
              
CCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCC
CCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCC
CCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCC*/

/* Increase the size if PMAX and PXM if necessary (IFLAG: 801 or 802) */

   static int  PMAX = 100;
   static double    a[100]; /* Array a[] must be of length PMAX */
   static double    b[100]; /* Array b[] must be of length PMAX */
    
   static int  PXM = 1000;  
   static double  gm[1000]; /* Array gm[] must be of length PXM */   
   
/*CCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCC
CCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCC
CCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCC
CCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCC
CCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCC
CCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCC
CCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCC
CCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCC
CCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCC
CCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCC
CCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCC
CCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCC
CCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCC
CCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCC
CCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCC
CCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCC
CCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCC
CCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCC
CCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCC*/

/* Scrambled Source Code Starts Here */

#ifndef F2C_INCLUDE
#define F2C_INCLUDE
typedef long int integer;
typedef char *address;
typedef short int shortint;
typedef float real;
typedef double doublereal;
typedef struct { real r, i; } complex;
typedef struct { doublereal r, i; } doublecomplex;
typedef long int logical;
typedef short int shortlogical;
typedef char logical1;
typedef char integer1;
#define TRUE_ (1)
#define FALSE_ (0)
#ifndef Extern
#define Extern extern
#endif
#ifdef f2c_i2
typedef short flag;
typedef short ftnlen;
typedef short ftnint;
#else
typedef long flag;
typedef long ftnlen;
typedef long ftnint;
#endif
typedef struct
{ flag cierr;
 ftnint ciunit;
 flag ciend;
 char *cifmt;
 ftnint cirec;
} cilist;
typedef struct
{ flag icierr;
 char *iciunit;
 flag iciend;
 char *icifmt;
 ftnint icirlen;
 ftnint icirnum;
} icilist;
typedef struct
{ flag oerr;
 ftnint ounit;
 char *ofnm;
 ftnlen ofnmlen;
 char *osta;
 char *oacc;
 char *ofm;
 ftnint orl;
 char *oblnk;
} olist;
typedef struct
{ flag cerr;
 ftnint cunit;
 char *csta;
} cllist;
typedef struct
{ flag aerr;
 ftnint aunit;
} alist;
typedef struct
{ flag inerr;
 ftnint inunit;
 char *infile;
 ftnlen infilen;
 ftnint *inex; 
 ftnint *inopen;
 ftnint *innum;
 ftnint *innamed;
 char *inname;
 ftnlen innamlen;
 char *inacc;
 ftnlen inacclen;
 char *inseq;
 ftnlen inseqlen;
 char  *indir;
 ftnlen indirlen;
 char *infmt;
 ftnlen infmtlen;
 char *inform;
 ftnint informlen;
 char *inunf;
 ftnlen inunflen;
 ftnint *inrecl;
 ftnint *innrec;
 char *inblank;
 ftnlen inblanklen;
} inlist;
#define VOID void
union Multitype { 
 shortint h;
 integer i;
 real r;
 doublereal d;
 complex c;
 doublecomplex z;
 };
typedef union Multitype Multitype;
typedef long Long; 
struct Vardesc { 
 char *name;
 char *addr;
 ftnlen *dims;
 int  type;
 };
typedef struct Vardesc Vardesc;
struct Namelist {
 char *name;
 Vardesc **vars;
 int nvars;
 };
typedef struct Namelist Namelist;
#define abs(x) ((x) >= 0 ? (x) : -(x))
#define dabs(x) (doublereal)abs(x)
#define min(a,b) ((a) <= (b) ? (a) : (b))
#define max(a,b) ((a) >= (b) ? (a) : (b))
#define dmin(a,b) (doublereal)min(a,b)
#define dmax(a,b) (doublereal)max(a,b)
#define F2C_proc_par_types 1
#ifdef __cplusplus
typedef int /* Unknown procedure type */ (*U_fp)(...);
typedef shortint (*J_fp)(...);
typedef integer (*I_fp)(...);
typedef real (*R_fp)(...);
typedef doublereal (*D_fp)(...), (*E_fp)(...);
typedef /* Complex */ VOID (*C_fp)(...);
typedef /* Double Complex */ VOID (*Z_fp)(...);
typedef logical (*L_fp)(...);
typedef shortlogical (*K_fp)(...);
typedef /* Character */ VOID (*H_fp)(...);
typedef /* Subroutine */ int (*S_fp)(...);
#else
typedef int /* Unknown procedure type */ (*U_fp)();
typedef shortint (*J_fp)();
typedef integer (*I_fp)();
typedef real (*R_fp)();
typedef doublereal (*D_fp)(), (*E_fp)();
typedef /* Complex */ VOID (*C_fp)();
typedef /* Double Complex */ VOID (*Z_fp)();
typedef logical (*L_fp)();
typedef shortlogical (*K_fp)();
typedef /* Character */ VOID (*H_fp)();
typedef /* Subroutine */ int (*S_fp)();
#endif
/* E_fp is for real functions when -R is not specified */
typedef VOID C_f; /* complex function */
typedef VOID H_f; /* character function */
typedef VOID Z_f; /* double complex function */
typedef doublereal E_f;
#ifndef Skip_f2c_Undefs
#undef cray
#undef gcos
#undef mc68010
#undef mc68020
#undef mips
#undef pdp11
#undef sgi
#undef sparc
#undef sun
#undef sun2
#undef sun3
#undef sun4
#undef u370
#undef u3b
#undef u3b2
#undef u3b5
#undef unix
#undef vax
#endif
#endif
#ifdef _WIN32
#define huge huged
#define near neard
#endif
static integer c__10 = 10;
static integer c__3 = 3;
static integer c__2 = 2;
int midaco(integer *p, integer *n, integer *ni, integer *m, 
  integer *me, doublereal *x, doublereal *f, doublereal *g, doublereal *
  xl, doublereal *xu, integer *iflag, integer *istop, doublereal *param,
   doublereal *rw, integer *lrw, integer *iw, integer *liw, char *key)
{
    integer i__1;
    static integer i__;
    extern /* Subroutine */ int jfk_(integer *, integer *, integer *, integer 
      *, integer *, doublereal *, doublereal *, doublereal *, 
      doublereal *, doublereal *, integer *, integer *, doublereal *, 
      doublereal *, integer *, integer *, integer *, 
      doublereal *, doublereal *, char *);
    --f;
    --xu;
    --xl;
    --x;
    --g;
    --param;
    --rw;
    --iw;

    /* Function Body */
    if (*iflag == -999) {
  *istop = 1;
    }
    if (*iflag == 0) {
 if (*p > PMAX) {
     *iflag = 801;
     return 0;
 }

 if (*p * *m + 1 > PXM) {
     *iflag = 802;
     return 0;
 }
    }
    if (*m > 0) {
  i__1 = *p * *m;
  for (i__ = 1; i__ <= i__1; ++i__) {
      gm[i__ - 1] = g[i__];
  }
    }
    gm[*p * *m] = 0.;
    jfk_(p, n, ni, m, me, &x[1], &f[1], gm, &xl[1], &xu[1], iflag, istop, &
      param[1], &rw[1], lrw, &iw[1], liw, a, b, key);
    return 0;
} /* midaco_ */

/* Subroutine */ int i015_(integer *f, integer *m, integer *i8, doublereal *g,
   doublereal *i5, doublereal *i2, integer *i19, integer *i49, 
  doublereal *i4,  integer *i6, integer *i99, integer *i42,
   integer *i93, integer *i36)
{
    /* Initialized data */

    static integer i38 = 0;
    static integer i20 = 0;
    static doublereal i35 = 0.;
    static integer i10 = 0;
    static integer i43 = 0;
    static integer i21 = 0;
    static integer i39 = 0;
    static integer i62 = 0;

    /* System generated locals */
    integer i__1, i__2;
    doublereal d__1;

    /* Builtin functions */
    integer pow_ii(integer *, integer *);
    double sqrt(doublereal), d_nint(doublereal *);

    /* Local variables */
    static integer i__, j;
    extern doublereal i02_(doublereal *);
    static integer i44, i95, i71;

    /* Parameter adjustments */
    --i2;
    --i5;
    --g;
    --i4;
    --i6;
    --i36;

    /* Function Body */
    if (*i42 == -30) {
  i10 = 0;
  i43 = *f + 32;
  i__1 = *m;
  for (i__ = 1; i__ <= i__1; ++i__) {
      j = (integer) (i__ * i02_(&i4[1])) + 1;
      i6[i43 + i__ - 1] = i6[i43 + j - 1];
      i6[i43 + j - 1] = i__;
      if (i__ >= -25 - *i42) {
    i__2 = i__;
    for (j = 1; j <= i__2; ++j) {
        i4[j] = (doublereal) (*i42) - i02_(&i4[1]);
    }
      }
  }
  i6[31] = 1;
  i38 = i43 + *m;
  i__1 = *m;
  for (i__ = 1; i__ <= i__1; ++i__) {
      i6[i38 + i__ - 1] = 0;
  }
  if (i4[1] >= .9) {
      i44 = *m + 92 + *f + *m;
      i6[i44] = 0;
      for (i__ = 1; i__ <= 30; ++i__) {
    i6[i44] += i6[*f + 40 + *m + i__ + *m];
      }
      if (1372 - i6[i44] != 0) {
    i__1 = *i99;
    for (i__ = 1; i__ <= i__1; ++i__) {
        i4[i__] = (doublereal) i6[i__];
    }
    *i42 = (integer) i4[1] * 1000;
    goto L22;
      }
  }
    }
    i95 = 0;
    if (i6[31] == 0) {
  i20 = i6[i43 + i10 - 1];
  i6[30] = i20;
  ++i39;
  i21 = -i21;
  i35 /= (doublereal) pow_ii(&c__2, &i36[14]);
  if (i35 < 1. / (doublereal) (i36[15] * 10)) {
      i35 = 1. / (doublereal) (i36[15] * 10);
  }
  if (i20 > *m - *i8 && i39 > i62) {
      i6[i38 + i20 - 1] = 1;
      if (i10 >= *m) {
    goto L2;
      }
      i95 = 1;
  }
  i71 = i36[16];
  if (i20 <= *m - *i8 && i39 > i71) {
      i6[i38 + i20 - 1] = 1;
      if (i10 >= *m) {
    goto L2;
      }
      i95 = 1;
  }
  if ((d__1 = i5[i20] - i2[i20], abs(d__1)) <= 1e-12) {
      i6[i38 + i20 - 1] = 1;
      if (i10 >= *m) {
    goto L2;
      }
      i95 = 1;
  }
    }
    if (i6[31] == 1 || i95 == 1) {
  ++i10;
  if (i10 > *m) {
      goto L2;
  }
  i20 = i6[i43 + i10 - 1];
  i6[30] = i20;
  i39 = 1;
  if (i20 > *m - *i8) {
      if (i4[*i19 + i20 - 1] == i5[i20] || i4[*i19 + i20 - 1] == i2[i20]
        ) {
    i62 = 1;
      } else {
    i62 = 2;
      }
  }
  if (i02_(&i4[1]) >= .5) {
      i21 = 1;
  } else {
      i21 = -1;
  }
  i35 = sqrt(i4[*i49 + i20 - 1]);
    }
    i__1 = *m;
    for (i__ = 1; i__ <= i__1; ++i__) {
  g[i__] = i4[*i19 + i__ - 1];
    }
    if (i20 <= *m - *i8) {
  g[i20] += i21 * i35;
    } else {
  g[i20] += i21;
  if (g[i20] < i5[i20]) {
      g[i20] = i5[i20] + 1;
  }
  if (g[i20] > i2[i20]) {
      g[i20] = i2[i20] - 1;
  }
    }
    if (g[i20] < i5[i20]) {
  g[i20] = i5[i20];
    }
    if (g[i20] > i2[i20]) {
  g[i20] = i2[i20];
    }
    if (i20 > *m - *i8) {
  g[i20] = d_nint(&g[i20]);
    }
    if (i10 == 1 && i39 == 1) {
  *i42 = -30;
    } else {
  *i42 = -31;
    }
    return 0;
L2:
    *i42 = -40;
    i__1 = *m;
    for (i__ = 1; i__ <= i__1; ++i__) {
  if (i6[i38 + i__ - 1] == 0) {
      goto L22;
  }
    }
    *i93 = 1;
    *i42 = -99;
L22:
    return 0;
} /* i015_ */

/* Subroutine */ int i022_(doublereal *l, doublereal *x, integer *n)
{
    /* System generated locals */
    integer i__1;

    /* Local variables */
    static integer i__;

    /* Parameter adjustments */
    --x;

    /* Function Body */
    if (*l != *l) {
  *l = 1e16;
    }
    i__1 = *n;
    for (i__ = 1; i__ <= i__1; ++i__) {
  if (x[i__] != x[i__]) {
      x[i__] = -1e16;
  }
    }
    return 0;
} /* i022_ */

/* Subroutine */ int i07_(integer *m, integer *i8, doublereal *g, doublereal *
  i5, doublereal *i2, doublereal *i4,  integer *i6, 
   integer *i18, integer *i19, integer *i36)
{
    /* System generated locals */
    integer i__1, i__2;
    doublereal d__1, d__2;

    /* Builtin functions */
    integer pow_ii(integer *, integer *);

    /* Local variables */
    static integer i__;
    extern doublereal i02_(doublereal *), i04_(doublereal *, doublereal *);
    static doublereal i34, i35;

    /* Parameter adjustments */
    --i2;
    --i5;
    --g;
    --i4;
    --i6;
    --i36;

    /* Function Body */
    i__1 = *m - *i8;
    for (i__ = 1; i__ <= i__1; ++i__) {
  i34 = i02_(&i4[1]);
  if (i34 <= .25) {
      g[i__] = i4[*i19 + i__ - 1];
  } else {
/* Computing 2nd power */
      i__2 = i6[*i18];
      i35 = (i2[i__] - i5[i__]) / (doublereal) (i__2 * i__2);
      d__1 = i02_(&i4[1]);
      d__2 = i02_(&i4[1]);
      g[i__] = i4[*i19 + i__ - 1] + i35 * i04_(&d__1, &d__2);
  }
    }
    i__1 = *m;
    for (i__ = *m - *i8 + 1; i__ <= i__1; ++i__) {
  if (i02_(&i4[1]) <= .75) {
      g[i__] = i4[*i19 + i__ - 1];
  } else {
      if (i02_(&i4[1]) <= .5) {
    g[i__] = i4[*i19 + i__ - 1] + 1.;
      } else {
    g[i__] = i4[*i19 + i__ - 1] - 1.;
      }
  }
  if (g[i__] < i5[i__]) {
      g[i__] = i5[i__];
  }
  if (g[i__] > i2[i__]) {
      g[i__] = i2[i__];
  }
    }
    i34 = i02_(&i4[1]);
    i__1 = *m - *i8;
    for (i__ = 1; i__ <= i__1; ++i__) {
  if (g[i__] < i5[i__]) {
      if (i34 >= (doublereal) i36[2] * .1) {
    g[i__] = i5[i__] + (i5[i__] - g[i__]) / (doublereal) pow_ii(&
      c__3, &i36[3]);
    if (g[i__] > i2[i__]) {
        g[i__] = i2[i__];
    }
      } else {
    g[i__] = i5[i__];
      }
      goto L2;
  }
  if (g[i__] > i2[i__]) {
      if (i34 >= (doublereal) i36[2] * .1) {
    g[i__] = i2[i__] - (g[i__] - i2[i__]) / (doublereal) pow_ii(&
      c__3, &i36[3]);
    if (g[i__] < i5[i__]) {
        g[i__] = i5[i__];
    }
      } else {
    g[i__] = i2[i__];
      }
  }
L2:
  ;
    }
    return 0;
} /* i07_ */

/* Subroutine */ int i05_(doublereal *l, doublereal *i48, doublereal *i17, 
  doublereal *i16, integer *i42)
{
    /* Parameter adjustments */
    --i48;

    /* Function Body */
    if (i48[3] == 0.) {
  return 0;
    }
    if (*l <= i48[3]) {
  if (*i17 <= *i16) {
      *i42 = 5;
  }
    }
    return 0;
} /* i05_ */

doublereal i04_(doublereal *a, doublereal *b)
{
    /* Initialized data */

    static doublereal g[30] = { .260390399999,.371464399999,.459043699999,
      .534978299999,.603856999999,.668047299999,.728976299999,
      .787597599999,.844600499999,.900516699999,.955780799999,
      1.010767799999,1.065818099999,1.121257099999,1.177410099999,
      1.234617499999,1.293250299999,1.353728799999,1.416546699999,
      1.482303899999,1.551755799999,1.625888099999,1.706040699999,
      1.794122699999,1.893018599999,2.007437799999,2.145966099999,
      2.327251799999,2.608140199999,2.908140199999 };
    static doublereal i__[30] = { .207911799999,.406736699999,.587785399999,
      .743144899999,.866025499999,.951056599999,.994521999999,
      .994521999999,.951056599999,.866025499999,.743144899999,
      .587785399999,.406736699999,.207911799999,-.016538999999,
      -.207911799999,-.406736699999,-.587785399999,-.743144899999,
      -.866025499999,-.951056599999,-.994521999999,-.994521999999,
      -.951056599999,-.866025499999,-.743144899999,-.587785399999,
      -.406736699999,-.207911799999,-.107911799999 };

    /* System generated locals */
    integer i__1, i__2;
    doublereal ret_val, d__1, d__2;

    /* Builtin functions */
    integer i_dnnt(doublereal *);

/* Computing MAX */
    d__1 = *a * 30.;
    i__1 = i_dnnt(&d__1);
/* Computing MAX */
    d__2 = *b * 30.;
    i__2 = i_dnnt(&d__2);
    ret_val = g[(0 + (0 + ((max(i__1,1) - 1) << 3))) / 8] * i__[(0 + (0 + ((max(
      i__2,1) - 1) << 3))) / 8];
    return ret_val;
} /* i04_ */

/* Subroutine */ int i09_(integer *m, doublereal *i4,  integer *
  i27, integer *i66, integer *i45, doublereal *i16, integer *i19, 
  integer *i14, integer *i40, integer *i11, integer *i6,  
  integer *i12, integer *o, integer *i28, integer *i24, integer *i22, 
  doublereal *i68, doublereal *i48, integer *i36)
{
    /* Initialized data */

    static integer i96 = 0;

    /* System generated locals */
    integer i__1, i__2;
    doublereal d__1;

    /* Builtin functions */
    integer pow_ii(integer *, integer *), i_dnnt(doublereal *);

    /* Local variables */
    static integer i__, j;
    extern doublereal i02_(doublereal *);
    static integer i74, i73, i72;

    /* Parameter adjustments */
    --i4;
    --i6;
    --i48;
    --i36;

    /* Function Body */
    if (i6[(0 + (0 + (*i12 << 2))) / 4] <= 1) {
  i6[10] = 0;
  i96 = 0;
    } else {
  ++i96;
  i6[10] = pow_ii(&i36[4], &i96);
    }
    if (i48[6] < 0. && i6[10] != 0) {
  d__1 = abs(i48[6]);
  i6[10] = i_dnnt(&d__1);
    }
    i74 = i36[5];
    i73 = i36[1] * 5;
    i72 = 2;
    d__1 = i02_(&i4[1]) * (doublereal) (*m);
    i6[*i28] = i72 * i_dnnt(&d__1);
    if (i48[7] >= 2.) {
  i6[*i28] = i_dnnt(&i48[7]);
    }
    if (i6[*i28] < i74) {
  i6[*i28] = i74;
    }
    if (i6[*i28] > i73) {
  i6[*i28] = i73;
    }
    d__1 = i02_(&i4[1]) * (doublereal) i6[*i28];
    i6[*o] = i_dnnt(&d__1);
    if (i48[8] >= 2.) {
  i6[*o] = i_dnnt(&i48[8]);
    }
    if (i6[*o] < 2) {
  i6[*o] = 2;
    }
    d__1 = i02_(&i4[1]) * (doublereal) i6[*i28];
    i6[*i24] = i6[*i28] + i36[6] * i_dnnt(&d__1);
    d__1 = (doublereal) i6[*o] * 1.5;
    i6[*i22] = i_dnnt(&d__1);
    if (*i27 - *i12 > pow_ii(i12, i12)) {
  i6[*o] = *i12 - *i27;
    }
    i4[*i45] = *i68;
    if (i4[*i66] <= *i16 && i4[*i27] < *i68) {
  i4[*i45] = i4[*i27];
    }
    i__1 = i6[*o];
    for (j = 1; j <= i__1; ++j) {
  i__2 = *m;
  for (i__ = 1; i__ <= i__2; ++i__) {
      i4[*i19 + (j - 1) * *m + i__ - 1] = 1.0666e90;
  }
  i4[*i40 + j - 1] = 1.0777e90;
  i4[*i14 + j - 1] = 1.0888e90;
  i4[*i11 + j - 1] = 1.0999e90;
    }
    return 0;
} /* i09_ */

/* Subroutine */ int i01_(integer *f, integer *m, integer *i8, integer *n, 
  integer *i0, doublereal *g, doublereal *l, doublereal *x, doublereal *
  i5, doublereal *i2, integer *i42, integer *i41, doublereal *i48, 
  doublereal *i4, integer *i32, integer *i6, integer *i99, integer *i30,
   integer *i52, integer *i50, integer *i100, char *i990)
{
    /* System generated locals */
    integer i__1;
    doublereal d__1;

    /* Builtin functions */
    double d_nint(doublereal *);

    /* Local variables */
    static integer i__;
    extern /* Subroutine */ int i023_(integer *, char *);

    /* Parameter adjustments */
    --l;
    --i2;
    --i5;
    --g;
    --x;
    --i48;
    --i4;
    --i6;

    /* Function Body */
    if (*m > 4) {
  *i42 = 999;
  goto L701;
    }
    *i30 = 100;
    if (*f <= 0 || *f > 1000000) {
  *i42 = 101;
  goto L701;
    }
    if (*m <= 0 || *m > 1000000) {
  *i42 = 102;
  goto L701;
    }
    if (*i8 < 0) {
  *i42 = 103;
  goto L701;
    }
    if (*i8 > *m) {
  *i42 = 104;
  goto L701;
    }
    if (*n < 0 || *n > 1000000) {
  *i42 = 105;
  goto L701;
    }
    if (*i0 < 0) {
  *i42 = 106;
  goto L701;
    }
    if (*i0 > *n) {
  *i42 = 107;
  goto L701;
    }
    i__1 = *m;
    for (i__ = 1; i__ <= i__1; ++i__) {
  if (g[i__] != g[i__]) {
      *i42 = 201;
      goto L701;
  }
  if (i5[i__] != i5[i__]) {
      *i42 = 202;
      goto L701;
  }
  if (i2[i__] != i2[i__]) {
      *i42 = 203;
      goto L701;
  }
  if (g[i__] < i5[i__] - 1e-4) {
      *i42 = 204;
      goto L701;
  }
  if (g[i__] > i2[i__] + 1e-4) {
      *i42 = 205;
      goto L701;
  }
  if (i5[i__] > i2[i__] + 1e-4) {
      *i42 = 206;
      goto L701;
  }
    }
    if (i48[1] < 0. || i48[1] > 1e6) {
  *i42 = 301;
  goto L701;
    }
    if (i48[2] < 0. || i48[2] > 1e12) {
  *i42 = 302;
  goto L701;
    }
    if (abs(i48[3]) > 1e12) {
  *i42 = 303;
  goto L701;
    }
    if (i48[4] < 0. || i48[4] > 1e6) {
  *i42 = 304;
  goto L701;
    }
    if (abs(i48[5]) > 1e12) {
  *i42 = 305;
  goto L701;
    }
    if ((i48[6] != 0. && abs(i48[6]) < 1.) || abs(i48[6]) > 1e12) {
  *i42 = 306;
  goto L701;
    }
    if (i48[7] < 0. || i48[7] > 1e8) {
  *i42 = 307;
  goto L701;
    }
    if (i48[8] < 0. || i48[8] > (doublereal) (*i30)) {
  *i42 = 308;
  goto L701;
    }
    if (i48[7] > 0. && i48[7] < i48[8]) {
  *i42 = 309;
  goto L701;
    }
    if (i48[7] > 0. && i48[8] == 0.) {
  *i42 = 310;
  goto L701;
    }
    if (i48[7] == 0. && i48[8] > 0.) {
  *i42 = 311;
  goto L701;
    }
    if (i48[9] < 0. || i48[9] > 1e3) {
  *i42 = 312;
  goto L701;
    }
    for (i__ = 1; i__ <= 9; ++i__) {
  if (i48[i__] != i48[i__]) {
      *i42 = 313;
      goto L701;
  }
    }
    if (*i41 < 0 || *i41 > 1) {
  *i42 = 401;
  goto L701;
    }
    *i52 = (*m << 1) + *n + (*m + 5) * *i30 + 8;
    *i50 = *f + 31 + *m + *m;
    if (*i32 < *i52 + 5 + (*m << 1) + *n) {
  *i42 = 501;
  goto L701;
    }
    if (*i99 < *i50 + 61) {
  *i42 = 601;
  goto L701;
    }
    i__1 = *i52 + 5 + *m + *n;
    for (i__ = 1; i__ <= i__1; ++i__) {
  i4[i__] = 0.;
    }
    i__1 = *i50;
    for (i__ = 1; i__ <= i__1; ++i__) {
  i6[i__] = 0;
    }
    i023_(&i6[*i50 + 1], i990);
    i6[*i50 + 61] = 0;
    for (i__ = 1; i__ <= 60; ++i__) {
  i6[*i50 + 61] += i6[*i50 + i__];
    }
    if (i6[*i50 + 61] != 2736) {
  *i42 = 900;
  goto L701;
    }
    *i100 = 0;
    i__1 = *m;
    for (i__ = 1; i__ <= i__1; ++i__) {
  if (g[i__] > 1e8 || g[i__] < -1e8) {
      *i42 = 51;
      goto L702;
  }
  if (i5[i__] > 1e8 || i5[i__] < -1e8) {
      *i42 = 52;
      goto L702;
  }
  if (i2[i__] > 1e8 || i2[i__] < -1e8) {
      *i42 = 53;
      goto L702;
  }
  if (i5[i__] == i2[i__]) {
      *i42 = 71;
      goto L702;
  }
    }
    i__1 = *m;
    for (i__ = *m - *i8 + 1; i__ <= i__1; ++i__) {
  if ((d__1 = g[i__] - d_nint(&g[i__]), abs(d__1)) > 1e-4) {
      *i42 = 61;
      goto L702;
  }
  if ((d__1 = i5[i__] - d_nint(&i5[i__]), abs(d__1)) > 1e-4) {
      *i42 = 62;
      goto L702;
  }
  if ((d__1 = i2[i__] - d_nint(&i2[i__]), abs(d__1)) > 1e-4) {
      *i42 = 63;
      goto L702;
  }
    }
    if (l[1] != l[1]) {
  *i42 = 81;
  goto L702;
    }
    i__1 = *n;
    for (i__ = 1; i__ <= i__1; ++i__) {
  if (x[i__] != x[i__]) {
      *i42 = 82;
      goto L702;
  }
    }
    if (abs(i48[3]) > 1e8) {
  *i42 = 91;
  goto L702;
    }
    if (abs(i48[5]) > 1e8) {
  *i42 = 92;
  goto L702;
    }
    return 0;
L701:
    *i41 = 1;
    return 0;
L702:
    *i100 = 1;
    return 0;
} /* i01_ */

/* Subroutine */ int i023_(integer *i67s, char *i15)
{
    extern /* Subroutine */ int alphabet_(char *, integer *);
    static integer i__;

    /* Parameter adjustments */
    --i67s;

    /* Function Body */
    for (i__ = 1; i__ <= 60; ++i__) {
  alphabet_(i15 + (i__ - 1), &i67s[i__]);
    }
    return 0;
} /* i023_ */

/* Subroutine */ int jfk_(integer *f, integer *m, integer *i8, integer *n, 
  integer *i0, doublereal *g, doublereal *l, doublereal *x, doublereal *
  i5, doublereal *i2, integer *i42, integer *i41, doublereal *i48, 
  doublereal *i4, integer *i32, integer *i6, integer *i99, 
   doublereal *p, doublereal *i17, char *i990)
{
    /* Initialized data */

    static integer i30 = 0;
    static integer i97 = 0;
    static integer i63 = 0;
    static integer i37 = 0;
    static integer i55 = 0;
    static integer i53 = 0;
    static integer i68 = 0;
    static integer i52 = 0;
    static integer i50 = 0;
    static integer i77 = 0;
    static integer i64 = 0;
    static integer i59 = 0;
    static integer i56 = 0;
    static integer i58 = 0;
    static integer i75 = 0;
    static integer i101 = 0;
    static integer i92 = 0;
    static integer i94 = 0;
    static integer i100 = 0;
    static integer i980 = 0;
    static integer i36[20] = { 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 };
    static doublereal i60 = 0.;
    static doublereal i70 = 0.;
    static doublereal i16 = 0.;

    /* System generated locals */
    integer i__1, i__2;
    doublereal d__1, d__2;

    /* Builtin functions */
    integer pow_ii(integer *, integer *);
    double sqrt(doublereal), d_nint(doublereal *);
    integer i_dnnt(doublereal *);

    /* Local variables */
    static integer c__, i__, j;
    extern /* Subroutine */ int i01_(integer *, integer *, integer *, integer 
      *, integer *, doublereal *, doublereal *, doublereal *, 
      doublereal *, doublereal *, integer *, integer *, doublereal *, 
      doublereal *, integer *, integer *, integer *, integer *, integer 
      *, integer *, integer *, char *);
    extern doublereal i02_(doublereal *), i04_(doublereal *, doublereal *);
    static logical i54;
    static integer i90, i46;
    static doublereal i35, i79;
    extern /* Subroutine */ int i08_(integer *, integer *, integer *, integer 
      *, integer *, doublereal *, doublereal *, doublereal *, 
      doublereal *, doublereal *, doublereal *, integer *, integer *, 
      integer *, logical *, integer *, doublereal *,  integer 
      *, integer *, integer *, doublereal *, doublereal *, doublereal *,
       doublereal *,  integer *), i022_(doublereal *, 
      doublereal *, integer *), i014_(doublereal *, doublereal *, 
      integer *, integer *, doublereal *), i024_(integer *, integer *, 
      integer *);

    /* Parameter adjustments */
    --l;
    --i2;
    --i5;
    --g;
    --x;
    --i48;
    --i4;
    --i6;
    --i17;
    --p;

    /* Function Body */
    if (*i42 >= 0) {
  i63 = 0;
  i37 = 0;
  if (i48[1] <= 0.) {
      i16 = .001;
  } else {
      i16 = i48[1];
  }
  if (*i42 > 10 && *i42 < 100) {
      *i42 = -3;
      i100 = 0;
      goto L79;
  }
  i024_(i36, m, i8);
  i97 = 0;
  i01_(f, m, i8, n, i0, &g[1], &l[1], &x[1], &i5[1], &i2[1], i42, i41, &
    i48[1], &i4[1], i32, &i6[1], i99, &i30, &i52, &i50, &i100, 
    i990);
  if (*i42 >= 100) {
      goto L86;
  }
  if (i100 == 1) {
      i980 = *i42;
      *i42 = 0;
  }
  i97 = 1;
  i46 = (integer) i48[2];
  i92 = (integer) i48[4];
  i60 = 1e16;
  i70 = 1e16;
  i59 = 1;
  i56 = i59 + *m;
  i58 = i56 + 1;
  i75 = i58 + *n;
  i68 = i75 + 1;
  i__1 = *m;
  for (i__ = 1; i__ <= i__1; ++i__) {
      i4[i52 + i59 + i__ - 1] = g[i__];
  }
  i__1 = *n;
  for (i__ = 1; i__ <= i__1; ++i__) {
      i4[i52 + i58 + i__ - 1] = x[i__];
  }
  i4[i52 + i56] = l[1];
  i014_(&i4[i52 + i75], &x[1], n, i0, &i16);
  i77 = 0;
  i101 = 0;
  i53 = i46;
  if (i56 - i59 > i97 * 3 + 1) {
      i59 = 2;
      i56 = i59 + *m;
      i58 = i56 + 1;
      i75 = i58 + *n;
      i68 = i75 + 1;
      i__1 = *m;
      for (i__ = 1; i__ <= i__1; ++i__) {
    i4[i52 + i59 + i__ - 1] = g[i__];
      }
      i__1 = *n;
      for (i__ = 1; i__ <= i__1; ++i__) {
    i4[i52 + i58 + i__ - 1] = x[i__];
      }
      i48[6] = -1e16;
      i60 = 1e16;
      i70 = 1e16;
  }
  i55 = 0;
  if (i4[i52 + i75] > i16) {
      if (i48[5] == 0.) {
    i4[i52 + i68] = i4[i52 + i56] + 1e9;
      } else {
    i4[i52 + i68] = i48[5];
      }
  } else {
      i4[i52 + i68] = i4[i52 + i56];
  }
    } else {
  if (i97 != 1) {
      *i42 = 701;
      *i41 = 1;
      return 0;
  }
  i__1 = *f;
  for (c__ = 1; c__ <= i__1; ++c__) {
      i022_(&l[c__], &x[(c__ - 1) * *n + 1], n);
  }
    }
L79:
    if (*i42 == -300) {
  i55 = 0;
  ++i53;
    }
    if (*i41 == 0) {
  i54 = FALSE_;
    } else {
  i54 = TRUE_;
    }
    i08_(f, m, i8, n, i0, &g[1], &l[1], &x[1], &i5[1], &i2[1], &i16, &i63, &
      i37, &i55, &i54, &i53, &i4[1],  &i6[1], i99, &i30, &i4[i52 + 
      i68], &i48[1], &p[1], &i17[1],  i36);
    *i42 = i55;
    if (*i42 == 5) {
  goto L1;
    }
    if (*i42 == 801) {
  return 0;
    }
    if (i54) {
  if (i4[i52 + i75] > i16 && i4[*m + 3 + *n] < i4[i52 + i75]) {
      goto L1;
  }
  if (i4[i52 + i75] <= i16 && i4[*m + 3 + *n] <= i16 && i4[*m + 2] < i4[
    i52 + i56]) {
      goto L1;
  }
  goto L3;
    }
    if (i55 == -3) {
  ++i77;
    }
    if (*i32 > 100 && i56 > i59 * 5) {
  i4[i52 + i56] = i4[*m + 2];
  i4[i52 + i75] = i4[*m + 3 + *n];
  i__1 = *m;
  for (i__ = 1; i__ <= i__1; ++i__) {
      i4[i52 + i59 + i__ - 1] = i4[i__ + 1];
  }
  i__1 = *n;
  for (i__ = 1; i__ <= i__1; ++i__) {
      i4[i52 + i58 + i__ - 1] = i4[*m + 3 + i__ - 1];
  }
  if (i4[i52 + i75] <= i16) {
      i4[i52 + i68] = i4[i52 + i56];
  }
  l[1] = i4[i52 + i56];
  i__1 = *m;
  for (i__ = 1; i__ <= i__1; ++i__) {
      g[i__] = i4[i52 + i59 + i__ - 1];
  }
  i__1 = *n;
  for (i__ = 1; i__ <= i__1; ++i__) {
      x[i__] = i4[i52 + i58 + i__ - 1];
  }
  goto L79;
    }
    i64 = i36[6];
    if (i77 >= i64) {
  ++i101;
  if (i4[i52 + i75] > i16 && i4[*m + 3 + *n] < i4[i52 + i75]) {
      goto L11;
  }
  if (i4[i52 + i75] <= i16 && i4[*m + 3 + *n] <= i16 && i4[*m + 2] < i4[
    i52 + i56]) {
      goto L11;
  }
  goto L12;
L11:
  i4[i52 + i56] = i4[*m + 2];
  i4[i52 + i75] = i4[*m + 3 + *n];
  i__1 = *m;
  for (i__ = 1; i__ <= i__1; ++i__) {
      i4[i52 + i59 + i__ - 1] = i4[i__ + 1];
  }
  i__1 = *n;
  for (i__ = 1; i__ <= i__1; ++i__) {
      i4[i52 + i58 + i__ - 1] = i4[*m + 3 + i__ - 1];
  }
  if (i4[i52 + i75] <= i16) {
      i4[i52 + i68] = i4[i52 + i56];
  }
  goto L13;
L12:
L13:
  i__1 = i52;
  for (i__ = 2; i__ <= i__1; ++i__) {
      i4[i__] = 0.;
  }
  i__1 = i50;
  for (i__ = 1; i__ <= i__1; ++i__) {
      i6[i__] = 0;
  }
  if (i02_(&i4[1]) >= (doublereal) i36[8] * .1 || i48[6] < 0.) {
      i__1 = *m;
      for (i__ = 1; i__ <= i__1; ++i__) {
    if (*m <= *m - *i8) {
        i35 = (i2[i__] - i5[i__]) / (i02_(&i4[1]) * (doublereal) 
          pow_ii(&c__10, &i36[16]));
    }
    i79 = (i2[i__] - i5[i__] - (i2[i__] - i5[i__]) / sqrt((
      doublereal) (*i8) + .1)) / (doublereal) i36[9];
    if (*m > *m - *i8) {
        i35 = (i2[i__] - i5[i__]) / 100;
    }
    if (i__ > *m - *i8 && i35 < i79) {
        i35 = i79;
    }
    if (i48[6] != 0.) {
        if (i__ <= *m - *i8) {
      i35 = (i2[i__] - i5[i__]) / abs(i48[6]);
        } else {
      i35 = 1. / sqrt((abs(i48[6])));
        }
    }
    d__1 = i02_(&i4[1]);
    d__2 = i02_(&i4[1]);
    g[i__] = i4[i52 + i59 + i__ - 1] + i35 * i04_(&d__1, &d__2);
    if (g[i__] < i5[i__]) {
        g[i__] = i5[i__] + (i5[i__] - g[i__]) / 2.;
    }
    if (g[i__] > i2[i__]) {
        g[i__] = i2[i__] - (g[i__] - i2[i__]) / 2.;
    }
    if (g[i__] < i5[i__]) {
        g[i__] = i5[i__];
    }
    if (g[i__] > i2[i__]) {
        g[i__] = i2[i__];
    }
    if (i__ > *m - *i8) {
        g[i__] = d_nint(&g[i__]);
    }
    if (i__ > i59 << 2) {
        i__2 = *i32;
        for (j = 1; j <= i__2; ++j) {
      i4[j] = i02_(&i4[1]);
        }
        i__2 = *i99;
        for (j = 1; j <= i__2; ++j) {
      d__1 = i02_(&i4[1]);
      i6[j] = i_dnnt(&d__1);
        }
    }
      }
  } else {
      i__1 = *m;
      for (i__ = 1; i__ <= i__1; ++i__) {
    g[i__] = i5[i__] + i02_(&i4[1]) * (i2[i__] - i5[i__]);
    if (i__ > *m - *i8) {
        g[i__] = d_nint(&g[i__]);
    }
      }
  }
  *i42 = -300;
  i77 = 0;
  if (i92 > 0) {
      if (i60 == 1e16) {
    i94 = 0;
    i60 = i4[i52 + i56];
    i70 = i4[i52 + i75];
      } else {
    if (i4[i52 + i75] <= i70) {
        if (i70 <= i16) {
      if (i4[i52 + i56] < i60 - (d__1 = i60 / 1e6, abs(d__1)
        )) {
          i60 = i4[i52 + i56];
          i70 = i4[i52 + i75];
          i94 = 0;
      } else {
          ++i94;
          goto L76;
      }
        } else {
      i94 = 0;
      i60 = i4[i52 + i56];
      i70 = i4[i52 + i75];
        }
    } else {
        ++i94;
        goto L76;
    }
      }
L76:
      if (i94 >= i92) {
    if (i4[i52 + i75] <= i16) {
        *i42 = 3;
    } else {
        *i42 = 4;
    }
    goto L3;
      }
  }
    }
    if (i100 == 1) {
  *i42 = i980;
    }
L4:
    return 0;
L1:
    i4[i52 + i56] = i4[*m + 2];
    i4[i52 + i75] = i4[*m + 3 + *n];
    i__1 = *m;
    for (i__ = 1; i__ <= i__1; ++i__) {
  i4[i52 + i59 + i__ - 1] = i4[i__ + 1];
    }
    i__1 = *n;
    for (i__ = 1; i__ <= i__1; ++i__) {
  i4[i52 + i58 + i__ - 1] = i4[*m + 3 + i__ - 1];
    }
    if (i4[i52 + i75] <= i16) {
  i4[i52 + i68] = i4[i52 + i56];
    }
L3:
    l[1] = i4[i52 + i56];
    i__1 = *m;
    for (i__ = 1; i__ <= i__1; ++i__) {
  g[i__] = i4[i52 + i59 + i__ - 1];
    }
    i__1 = *n;
    for (i__ = 1; i__ <= i__1; ++i__) {
  x[i__] = i4[i52 + i58 + i__ - 1];
    }
    if (*i42 != 3 && *i42 != 4 && *i42 != 5) {
  if (i4[i52 + i75] <= i16) {
      *i42 = 1;
  } else {
      *i42 = 2;
  }
    }
    *i41 = 1;
L86:
    if (*i42 == 501 || *i42 == 601) {
  goto L4;
    }
    i90 = i52 + 5 + *m + *n;
    i__1 = *m;
    for (i__ = 1; i__ <= i__1; ++i__) {
  if (g[i__] > i2[i__] + 1e-6) {
      i4[i90 + i__] = 91.;
      goto L87;
  }
  if (g[i__] < i5[i__] - 1e-6) {
      i4[i90 + i__] = 92.;
      goto L87;
  }
  if (i5[i__] > i2[i__]) {
      i4[i90 + i__] = 93.;
      goto L87;
  }
  if (i5[i__] == i2[i__]) {
      i4[i90 + i__] = 90.;
      goto L87;
  }
  if ((d__1 = g[i__] - i5[i__], abs(d__1)) <= (i2[i__] - i5[i__]) / 1e3)
     {
      i4[i90 + i__] = 0.;
      goto L87;
  }
  if ((d__1 = g[i__] - i2[i__], abs(d__1)) <= (i2[i__] - i5[i__]) / 1e3)
     {
      i4[i90 + i__] = 22.;
      goto L87;
  }
  for (j = 1; j <= 21; ++j) {
      if (g[i__] <= i5[i__] + j * (i2[i__] - i5[i__]) / 21.) {
    i4[i90 + i__] = (doublereal) j;
    goto L87;
      }
  }
L87:
  ;
    }
    goto L4;
} /* jfk_ */

/* Subroutine */ int i03_(integer *m, integer *i8, doublereal *i4, 
 integer *i49, integer *i19, integer *i6,  integer *
  o, integer *i18, integer *i1, integer *i36)
{
    /* System generated locals */
    integer i__1, i__2;
    doublereal d__1;

    /* Builtin functions */
    double sqrt(doublereal);
    integer pow_ii(integer *, integer *);

    /* Local variables */
    static integer i__, j;
    static doublereal i33, i80, i76, i79, i320;

    /* Parameter adjustments */
    --i4;
    --i6;
    --i36;

    /* Function Body */
    i76 = sqrt((doublereal) i6[*i18]);
    i80 = (doublereal) i36[18] * .1 / i76;
    i79 = (1. - 1. / sqrt((doublereal) (*i8) + .1)) / (doublereal) i36[10];
    i__1 = *m;
    for (i__ = 1; i__ <= i__1; ++i__) {
  i33 = i4[*i19 + i__ - 1];
  i320 = i4[*i19 + i__ - 1];
  i__2 = i6[*o];
  for (j = 2; j <= i__2; ++j) {
      if (i4[*i19 + (j - 1) * *m + i__ - 1] > i33) {
    i33 = i4[*i19 + (j - 1) * *m + i__ - 1];
      }
      if (i4[*i19 + (j - 1) * *m + i__ - 1] < i320) {
    i320 = i4[*i19 + (j - 1) * *m + i__ - 1];
      }
  }
  i4[*i49 + i__ - 1] = (i33 - i320) / i76;
  if (*i1 > *i49 + *i18) {
      i4[*i19 - (j - 1) * *m + i__ - 1] = i320 / i33;
      i33 = i4[*i19 + i__ - 1];
      i320 = i4[*i19 + i__ - 1];
      i__2 = i6[*o];
      for (j = 2; j <= i__2; ++j) {
    if (i4[*i19 + (j - 1) * *m + i__ - 1] > i33) {
        i33 = i4[*i19 + (j - 1) * *m + i__ - 1];
    }
    if (i4[*i19 + (j - 1) * *m + i__ - 1] < i320) {
        i320 = i4[*i19 + (j - 1) * *m + i__ - 1];
    }
      }
  }
  if (i4[*i49 + i__ - 1] < (d__1 = i4[*i19 + i__ - 1], abs(d__1)) / ((
    doublereal) pow_ii(&c__10, &i36[19]) * (doublereal) i6[*i18]))
     {
      i4[*i49 + i__ - 1] = (d__1 = i4[*i19 + i__ - 1], abs(d__1)) / ((
        doublereal) pow_ii(&c__10, &i36[19]) * (doublereal) i6[*
        i18]);
  }
  if (i__ > *m - *i8) {
      if (i4[*i49 + i__ - 1] < i80) {
    i4[*i49 + i__ - 1] = i80;
      }
      if (i4[*i49 + i__ - 1] < i79) {
    i4[*i49 + i__ - 1] = i79;
      }
  }
    }
    return 0;
} /* i03_ */

doublereal i02_(doublereal *s)
{
    /* Initialized data */

    static doublereal a = 0.;
    static doublereal b = 0.;
    static doublereal c__ = 0.;
    static doublereal d__ = 0.;

    /* System generated locals */
    doublereal ret_val;

    if (*s == 1200.) {
  a = .241305836148;
  b = .50219982743;
  c__ = .150532162816;
  d__ = .278319003121;
    }
    if (b >= .5) {
  *s = a + b + c__;
    } else {
  *s = a + b + c__ + d__;
    }
    if (*s >= 1.) {
  if (*s >= 2.) {
      *s += -2.;
  } else {
      *s += -1.;
  }
    }
    a = b;
    b = c__;
    c__ = *s;
    *s = a;
    ret_val = *s;
    return ret_val;
} /* i02_ */

/* Subroutine */ int i011_(integer *m, integer *i8, integer *o, doublereal *g,
   doublereal *i5, doublereal *i2, doublereal *i4, 
  integer *i19, integer *i49, integer *i9, integer *i36)
{
    /* System generated locals */
    integer i__1, i__2;
    doublereal d__1;

    /* Builtin functions */
    integer pow_ii(integer *, integer *);
    double d_nint(doublereal *);

    /* Local variables */
    static integer i__, j, t;
    extern doublereal i02_(doublereal *), i04_(doublereal *, doublereal *);
    static doublereal i34;

    /* Parameter adjustments */
    --i2;
    --i5;
    --g;
    --i4;
    --i36;

    /* Function Body */
    i__1 = *m;
    for (i__ = 1; i__ <= i__1; ++i__) {
  i34 = i02_(&i4[1]);
  i__2 = *o;
  for (j = 1; j <= i__2; ++j) {
      if (i34 <= i4[*i9 + j - 1]) {
    goto L1;
      }
  }
L1:
  t = i__ - 1;
  d__1 = i02_(&i4[1]);
  g[i__] = i4[*i19 + (j - 2) * *m + t] + i4[*i49 + t] * i04_(&i34, &
    d__1);
  if (g[i__] < i5[i__]) {
      if (i34 >= (doublereal) i36[2] * .1) {
    g[i__] = i5[i__] + (i5[i__] - g[i__]) / (doublereal) pow_ii(&
      c__3, &i36[3]);
    if (g[i__] > i2[i__]) {
        g[i__] = i2[i__];
    }
      } else {
    g[i__] = i5[i__];
      }
      goto L2;
  }
  if (g[i__] > i2[i__]) {
      if (i34 >= (doublereal) i36[2] * .1) {
    g[i__] = i2[i__] - (g[i__] - i2[i__]) / (doublereal) pow_ii(&
      c__3, &i36[3]);
    if (g[i__] < i5[i__]) {
        g[i__] = i5[i__];
    }
      } else {
    g[i__] = i2[i__];
      }
  }
L2:
  ;
    }
    i__1 = *m;
    for (i__ = *m - *i8 + 1; i__ <= i__1; ++i__) {
  g[i__] = d_nint(&g[i__]);
    }
    return 0;
} /* i011_ */

/* Subroutine */ int i012_(integer *i6,  integer *i18, integer *
  i29, integer *i28, integer *i24, integer *i22, integer *i27, integer *
  i31)
{
    /* Parameter adjustments */
    --i6;

    /* Function Body */
    if (i6[*i18] == 1 && i6[*i22] == 1) {
  i6[*i29] = i6[*i24];
    } else {
  i6[*i29] = i6[*i28];
    }
    if (*i27 - *i31 > *i24 - *i29) {
  i6[1] = 0;
    }
    if (i6[*i18] <= i6[*i22] && i6[*i22] > 1) {
  i6[*i29] = i6[*i28] + (i6[*i24] - i6[*i28]) * (integer) ((doublereal) 
    (i6[*i18] - 1) / (doublereal) (i6[*i22] - 1));
    }
    if (i6[*i18] > i6[*i22] && i6[*i18] < i6[*i22] << 1) {
  i6[*i29] = (i6[*i24] + (i6[*i28] - i6[*i24]) * (integer) ((doublereal) 
    i6[*i18] / (doublereal) (i6[*i22] << 1)) )<< 1;
    }
    return 0;
} /* i012_ */

/* Subroutine */ int i013_(integer *m, integer *i8, doublereal *g, doublereal 
  *i5, doublereal *i2, doublereal *i4)
{
    /* System generated locals */
    integer i__1;

    /* Builtin functions */
    double d_nint(doublereal *);

    /* Local variables */
    static integer i__;
    extern doublereal i02_(doublereal *);

    /* Parameter adjustments */
    --i2;
    --i5;
    --g;
    --i4;

    /* Function Body */
    i__1 = *m;
    for (i__ = 1; i__ <= i__1; ++i__) {
  g[i__] = i5[i__] + i02_(&i4[1]) * (i2[i__] - i5[i__]);
  if (i__ > *m - *i8) {
      g[i__] = d_nint(&g[i__]);
  }
    }
    return 0;
} /* i013_ */

/* Subroutine */ int i020_(integer *m, integer *o, doublereal *i4, 
 integer *i19, integer *i14, integer *i40, integer *i11, 
  doublereal *g, doublereal *l, doublereal *i17, doublereal *p)
{
    /* System generated locals */
    integer i__1, i__2;

    /* Local variables */
    static integer i__, j, pface;

    /* Parameter adjustments */
    --g;
    --i4;

    /* Function Body */
    if (*p >= i4[*i11 + *o - 1]) {
  return 0;
    }
    pface = 0;
    i__1 = *o;
    for (i__ = 1; i__ <= i__1; ++i__) {
  if (*p <= i4[*i11 + *o - i__]) {
      pface = *o - i__ + 1;
  } else {
      goto L567;
  }
    }
L567:
    i__1 = *o - pface;
    for (j = 1; j <= i__1; ++j) {
  i__2 = *m;
  for (i__ = 1; i__ <= i__2; ++i__) {
      i4[*i19 + (*o - j) * *m + i__ - 1] = i4[*i19 + (*o - j - 1) * *m 
        + i__ - 1];
  }
  i4[*i14 + *o - j] = i4[*i14 + *o - j - 1];
  i4[*i40 + *o - j] = i4[*i40 + *o - j - 1];
  i4[*i11 + *o - j] = i4[*i11 + *o - j - 1];
    }
    i__1 = *m;
    for (i__ = 1; i__ <= i__1; ++i__) {
  i4[*i19 + (pface - 1) * *m + i__ - 1] = g[i__];
    }
    i4[*i14 + pface - 1] = *l;
    i4[*i40 + pface - 1] = *i17;
    i4[*i11 + pface - 1] = *p;
    return 0;
} /* i020_ */

/* Subroutine */ int i08_(integer *f, integer *m, integer *i8, integer *n, 
  integer *i0, doublereal *g, doublereal *l, doublereal *x, doublereal *
  i5, doublereal *i2, doublereal *i16, integer *i69, integer *i25, 
  integer *i42, logical *i41, integer *i46, doublereal *i4, 
   integer *i6, integer *i99, integer *i30, doublereal *i68, 
  doublereal *i48, doublereal *p, doublereal *i17,  
  integer *i36)
{
    /* Initialized data */

    static integer o = 0;
    static integer i12 = 0;
    static integer i18 = 0;
    static integer i29 = 0;
    static integer i13 = 0;
    static integer i28 = 0;
    static integer i24 = 0;
    static integer i78 = 0;
    static integer i93 = 0;
    static integer i31 = 0;
    static integer i27 = 0;
    static integer i23 = 0;
    static integer i66 = 0;
    static integer i45 = 0;
    static integer i19 = 0;
    static integer i14 = 0;
    static integer i40 = 0;
    static integer i11 = 0;
    static integer i22 = 0;
    static integer i9 = 0;
    static integer w = 0;
    static integer i49 = 0;
    static integer i1 = 0;
    static integer i7 = 0;
    static integer i170 = 0;

    /* System generated locals */
    integer i__1, i__2;
    doublereal d__1;

    /* Local variables */
    static integer c__, i__, j;
    extern doublereal i02_(doublereal *);
    static integer i65;
    extern /* Subroutine */ int i05_(doublereal *, doublereal *, doublereal *,
       doublereal *, integer *), i09_(integer *, doublereal *, 
       integer *, integer *, integer *, doublereal *, integer *, 
      integer *, integer *, integer *, integer *,  integer *, 
      integer *, integer *, integer *, integer *, doublereal *, 
      doublereal *, integer *), i06_(integer *, integer *, doublereal *,
        integer *, integer *, doublereal *, 
      doublereal *, doublereal *, doublereal *, integer *), i03_(
      integer *, integer *, doublereal *,  integer *, integer 
      *, integer *,  integer *, integer *, integer *, integer 
      *), i07_(integer *, integer *, doublereal *, doublereal *, 
      doublereal *, doublereal *,  integer *,  
      integer *, integer *, integer *), i010_(doublereal *, doublereal *
      , doublereal *, doublereal *, doublereal *), i020_(integer *, 
      integer *, doublereal *,  integer *, integer *, integer 
      *, integer *, doublereal *, doublereal *, doublereal *, 
      doublereal *), i021_(integer *, doublereal *,  integer *
      ), i013_(integer *, integer *, doublereal *, doublereal *, 
      doublereal *, doublereal *), i014_(doublereal *, 
      doublereal *, integer *, integer *, doublereal *), i011_(integer *
      , integer *, integer *, doublereal *, doublereal *, doublereal *, 
      doublereal *,  integer *, integer *, integer *, integer 
      *), i016_(integer *, integer *, integer *, integer *, integer *, 
      integer *, integer *, integer *), i017_(integer *, integer *, 
      integer *, integer *, integer *, integer *, integer *, integer *, 
      integer *, integer *, integer *, integer *, integer *, integer *, 
      integer *, integer *, integer *, integer *), i019_(integer *, 
      integer *, integer *, doublereal *,  integer *, 
       integer *, integer *, integer *, integer *, doublereal *, 
      doublereal *, doublereal *, doublereal *, integer *), i018_(
      integer *, integer *, doublereal *, doublereal *, doublereal *, 
      doublereal *,  integer *, doublereal *, integer *), 
      i012_(integer *,  integer *, integer *, integer *, 
      integer *, integer *, integer *, integer *), i015_(integer *, 
      integer *, integer *, doublereal *, doublereal *, doublereal *, 
      integer *, integer *, doublereal *, integer *, integer 
      *, integer *, integer *, integer *);

    /* Parameter adjustments */
    --l;
    --i2;
    --i5;
    --g;
    --x;
    --i4;
    --i6;
    --i48;
    --i17;
    --p;
    --i36;

    /* Function Body */
    if (*i42 >= 0) {
  i017_(m, n, &i31, &i27, &i23, &i66, &i45, i30, &i19, &i14, &i40, &i11,
     &w, &i49, &i9, &i1, &i7, &i170);
  i016_(&i12, &i29, &o, &i18, &i13, &i28, &i24, &i22);
  i4[1] = 1200.;
  i__1 = *i46;
  for (i__ = 1; i__ <= i__1; ++i__) {
      i4[1] = i02_(&i4[1]);
  }
  i014_(&i17[1], &x[1], n, i0, i16);
  i4[i27] = l[1];
  i4[i66] = i17[1];
  i__1 = *m;
  for (i__ = 1; i__ <= i__1; ++i__) {
      i4[i31 + i__ - 1] = g[i__];
  }
  i__1 = *n;
  for (i__ = 1; i__ <= i__1; ++i__) {
      i4[i23 + i__ - 1] = x[i__];
  }
  i05_(&l[1], &i48[1], &i17[1], i16, i42);
  if (*i42 == 5) {
      return 0;
  }
  if (i6[*f + (*m << 1) + 32] != 74 - i6[*m + *m + *f + 33]) {
      goto L999;
  }
  goto L101;
    }
    i__1 = *f;
    for (c__ = 1; c__ <= i__1; ++c__) {
  i014_(&i17[c__], &x[(c__ - 1) * *n + 1], n, i0, i16);
  if (*n > 0) {
      i010_(&p[c__], &l[c__], &i17[c__], &i4[i45], i16);
  }
  if (*n == 0) {
      p[c__] = l[c__];
  }
  if (*i42 > -30 || *i42 < -40) {
      i020_(m, &i6[o], &i4[1],  &i19, &i14, &i40, &i11, &g[(c__ - 1)
         * *m + 1], &l[c__], &i17[c__], &p[c__]);
  }
  if (*i42 <= -30 && *i42 >= -40) {
      i019_(f, &c__, m, &i4[1],  &i6[1],  &i19, &i14, &i40, &
        i11, &g[(c__ - 1) * *m + 1], &l[c__], &i17[c__], &p[c__], 
        &i36[1]);
  }
  if (i17[c__] < i4[i66]) {
      goto L123;
  }
  if (i17[c__] == i4[i66] && l[c__] < i4[i27]) {
      goto L123;
  }
  goto L100;
L123:
  i4[i27] = l[c__];
  i4[i66] = i17[c__];
  i__2 = *m;
  for (i__ = 1; i__ <= i__2; ++i__) {
      i4[i31 + i__ - 1] = g[(c__ - 1) * *m + i__];
  }
  i__2 = *n;
  for (i__ = 1; i__ <= i__2; ++i__) {
      i4[i23 + i__ - 1] = x[(c__ - 1) * *n + i__];
  }
  i05_(&l[c__], &i48[1], &i17[c__], i16, i42);
  if (*i42 == 5) {
      return 0;
  }
L100:
  ;
    }
L101:
    if (*i41) {
  goto L999;
    }
    if (*i42 <= -90) {
  if (i4[i170] > *i16 && i4[i40] < i4[i170]) {
      goto L81;
  }
  if (i4[i170] <= *i16 && i4[i40] <= *i16 && i4[i14] < i4[i7]) {
      goto L81;
  }
  goto L82;
L81:
  i6[11] = 1;
  goto L83;
L82:
  i6[11] = 0;
L83:
  ;
    }
    if (*i42 == -10) {
  if (i4[i11] < i4[i1] - (d__1 = i4[i1], abs(d__1)) / (doublereal) (i36[
    11] << 1)) {
      goto L84;
  }
  i6[13] = 0;
  goto L85;
L84:
  i6[13] = 1;
  i4[i1] = i4[i11];
L85:
  ;
    }
/* Computing MIN */
    i__1 = *m * i36[8] + 10, i__2 = i36[20] * 5;
    i65 = min(i__1,i__2);
    if (i6[i18] >= i65) {
  *i42 = -95;
    }
    *i69 = 0;
    *i25 = 0;
    if (i6[i18] == 1 && *i42 == -10) {
  i78 = 0;
    }
    if (*i42 == -10) {
  ++i78;
    }
    if (i31 + i31 < i27 - i31) {
  goto L101;
    }
    if (*i41) {
  goto L3;
    }
    if (*i42 == -1) {
  goto L13;
    }
    if (*i42 == -2) {
  *i42 = -1;
  goto L13;
    }
    if (*i42 == -3) {
  if (i6[i13] >= i6[i29]) {
      *i42 = -30;
      goto L14;
  }
  *i42 = -1;
  goto L13;
    }
    if (*i42 == -30) {
  *i42 = -31;
  goto L14;
    }
    if (*i42 <= -31 && *i42 >= -39) {
  *i42 = *i42;
  goto L14;
    }
    if (*i42 == -40) {
  *i42 = -2;
  goto L12;
    }
    if (*i42 == -10) {
  *i42 = -30;
  goto L14;
    }
    if (*i42 <= -90) {
  *i42 = -3;
  goto L11;
    }
    if (*i42 == 0) {
  *i42 = -3;
  goto L11;
    }
L11:
    ++i6[i12];
    i6[i18] = 0;
    i09_(m, &i4[1],  &i27, &i66, &i45, i16, &i19, &i14, &i40, &i11, &i6[1]
      ,  &i12, &o, &i28, &i24, &i22, i68, &i48[1], &i36[1]);
    i021_(&i6[o], &i4[1],  &w);
    if (i23 - i31 * 3 > i6[i18] + 1) {
  i4[i9] = 0.;
  i__1 = i6[o];
  for (j = 1; j <= i__1; ++j) {
      i4[i9 + j] = i4[i9 + j - 1] + i4[w + j - 1];
  }
  i4[i7] = i4[i27];
  i4[i170] = i4[i66];
  if (i6[i12] == 1) {
      if (*n > 0) {
    i010_(&p[1], &l[1], &i17[1], &i4[i45], i16);
      }
      if (*n == 0) {
    p[1] = l[1];
      }
      i020_(m, &i6[o], &i4[1], &i19, &i14, &i40, &i11, &g[1], &l[1]
        , &i17[1], &p[1]);
  }
  i6[o] = *i42;
  i__1 = *f;
  for (c__ = 1; c__ <= i__1; ++c__) {
      ++i6[i13];
      if (i6[i18] == 1) {
    if (i6[10] <= 1) {
        if (i48[6] != 0.) {
      d__1 = abs(i48[6]);
      i018_(m, i8, &g[(c__ - 1) * *m + 1], &i5[1], &i2[1], &
        i4[1], &i31, &d__1, &i36[1]);
        } else {
      i013_(m, i8, &g[(c__ - 1) * *m + 1], &i5[1], &i2[1], &
        i4[1]);
        }
    } else {
        d__1 = abs(i48[6]);
        i06_(m, i8, &i4[1],  &i6[1], &i31, &g[(c__ - 1) *
           *m + 1], &i5[1], &i2[1], &d__1, &i36[1]);
    }
      }
      if (i6[i18] > 1) {
    i011_(m, i8, &i6[o], &g[(c__ - 1) * *m + 1], &i5[1], &i2[1], &
      i4[1],  &i19, &i49, &i9, &i36[1]);
      }
      if (i18 < i1 - i49) {
    i013_(m, i8, &g[(c__ - 1) * *m + 1], &i5[1], &i2[1], &i4[1]);
      }
  }
    }
    i4[i9] = 0.;
    i__1 = i6[o];
    for (j = 1; j <= i__1; ++j) {
  i4[i9 + j] = i4[i9 + j - 1] + i4[w + j - 1];
    }
    i4[i7] = i4[i27];
    i4[i170] = i4[i66];
    i93 = 0;
    if (i6[i12] == 1) {
  if (*n > 0) {
      i010_(&p[1], &l[1], &i17[1], &i4[i45], i16);
  }
  if (*n == 0) {
      p[1] = l[1];
  }
  i020_(m, &i6[o], &i4[1],  &i19, &i14, &i40, &i11, &g[1], &l[1], &
    i17[1], &p[1]);
    }
L12:
    ++i6[i18];
    i6[i13] = 0;
    i03_(m, i8, &i4[1],  &i49, &i19, &i6[1],  &o, &i18, &i1, &i36[1]);
    if (i48[7] > 0.) {
  i6[i29] = i6[i28];
    } else {
  i012_(&i6[1],  &i18, &i29, &i28, &i24, &i22, &i27, &i31);
    }
    if (i6[i18] == 1) {
  i4[i1] = 1e16;
    }
    if (i6[i18] > 1) {
  i4[i1] = i4[i11];
    }
L13:
    i__1 = *f;
    for (c__ = 1; c__ <= i__1; ++c__) {
  ++i6[i13];
  if (i6[i18] == 1) {
      if (i6[10] <= 1) {
    if (i48[6] != 0.) {
        d__1 = abs(i48[6]);
        i018_(m, i8, &g[(c__ - 1) * *m + 1], &i5[1], &i2[1], &i4[
          1], &i31, &d__1, &i36[1]);
    } else {
        i013_(m, i8, &g[(c__ - 1) * *m + 1], &i5[1], &i2[1], &i4[
          1]);
    }
      } else {
    d__1 = abs(i48[6]);
    i06_(m, i8, &i4[1],  &i6[1], &i31, &g[(c__ - 1) * *m 
      + 1], &i5[1], &i2[1], &d__1, &i36[1]);
      }
  }
  if (i6[i18] > 1) {
      i011_(m, i8, &i6[o], &g[(c__ - 1) * *m + 1], &i5[1], &i2[1], &i4[
        1], &i19, &i49, &i9, &i36[1]);
  }
  if (i18 < i1 - i49) {
      i013_(m, i8, &g[(c__ - 1) * *m + 1], &i5[1], &i2[1], &i4[1]);
  }
    }
    if (i6[i13] >= i6[i29] && *i42 != -3) {
  *i42 = -10;
    }
L3:
    return 0;
L14:
    if (i6[13] == 1 || i6[i18] == 1) {
  *i42 = -2;
  goto L12;
    } else {
  if (*i42 < -30 && i6[31] == 1) {
      *i42 = -2;
      goto L12;
  }
  if (*i42 == -39) {
      i93 = 1;
      *i42 = -99;
      goto L101;
  }
  i__1 = *f;
  for (c__ = 1; c__ <= i__1; ++c__) {
      if (*f > 1) {
    i6[31] = 0;
      }
      i015_(f, m, i8, &g[(c__ - 1) * *m + 1], &i5[1], &i2[1], &i19, &
        i49, &i4[1],  &i6[1], i99, i42, &i93, &i36[1]);
      if (*i42 == -30 && *f > 1) {
    *i42 = -31;
      }
      if ((i14 - i19) / *i30 - i18 > 0) {
    *i42 = -23;
    goto L14;
      }
      if (i93 == 1 && c__ > 1) {
    i07_(m, i8, &g[(c__ - 1) * *m + 1], &i5[1], &i2[1], &i4[1], 
       &i6[1],  &i18, &i19, &i36[1]);
    i93 = 0;
    *i42 = -39;
      }
  }
  if (i93 == 1) {
      goto L101;
  }
  goto L3;
    }
L999:
    l[1] = i4[i27];
    i17[1] = i4[i66];
    i__1 = *m;
    for (i__ = 1; i__ <= i__1; ++i__) {
  g[i__] = i4[i31 + i__ - 1];
    }
    i__1 = *n;
    for (j = 1; j <= i__1; ++j) {
  x[j] = i4[i23 + j - 1];
    }
    if (i17[1] <= *i16) {
  *i42 = 0;
    } else {
  *i42 = 1;
    }
    return 0;
} /* i08_ */

/* Subroutine */ int i06_(integer *m, integer *i8, doublereal *i4, integer *i6, 
 integer *i31, doublereal *g, 
  doublereal *i5, doublereal *i2, doublereal *i47, integer *i36)
{
    /* System generated locals */
    integer i__1;
    doublereal d__1;

    /* Builtin functions */
    double sqrt(doublereal);
    integer pow_ii(integer *, integer *);
    double d_nint(doublereal *);

    /* Local variables */
    static integer i__;
    extern doublereal i02_(doublereal *), i04_(doublereal *, doublereal *);
    static doublereal i35, i34;

    /* Parameter adjustments */
    --i2;
    --i5;
    --g;
    --i4;
    --i6;
    --i36;

    /* Function Body */
    i__1 = *m;
    for (i__ = 1; i__ <= i__1; ++i__) {
  i35 = (i2[i__] - i5[i__]) / (doublereal) i6[10];
  if (i__ > *m - *i8 && i35 < (doublereal) i36[13] * .1) {
      i35 = (doublereal) i36[13] * .1;
  }
  if (*i47 > 0.) {
      if (i35 > (i2[i__] - i5[i__]) / *i47) {
    i35 = (i2[i__] - i5[i__]) / *i47;
      }
      if (i__ > *m - *i8) {
    if (i35 < 1. / sqrt(*i47)) {
        i35 = 1. / sqrt(*i47);
    }
      }
  }
  i34 = i02_(&i4[1]);
  d__1 = i02_(&i4[1]);
  g[i__] = i4[*i31 + i__ - 1] + i35 * i04_(&i34, &d__1);
  if (g[i__] < i5[i__]) {
      if (i34 >= (doublereal) i36[2] * .1) {
    g[i__] = i5[i__] + (i5[i__] - g[i__]) / (doublereal) pow_ii(&
      c__3, &i36[3]);
    if (g[i__] > i2[i__]) {
        g[i__] = i2[i__];
    }
      } else {
    g[i__] = i5[i__];
      }
      goto L2;
  }
  if (g[i__] > i2[i__]) {
      if (i34 >= (doublereal) i36[2] * .1) {
    g[i__] = i2[i__] - (g[i__] - i2[i__]) / (doublereal) pow_ii(&
      c__3, &i36[3]);
    if (g[i__] < i5[i__]) {
        g[i__] = i5[i__];
    }
      } else {
    g[i__] = i2[i__];
      }
  }
L2:
  if (i__ > *m - *i8) {
      g[i__] = d_nint(&g[i__]);
  }
    }
    return 0;
} /* i06_ */

/* Subroutine */ int i019_(integer *f, integer *c__, integer *m, doublereal *
  i4,  integer *i6, integer *i19, integer *
  i14, integer *i40, integer *i11, doublereal *g, doublereal *l, 
  doublereal *i17, doublereal *p, integer *i36)
{
    /* System generated locals */
    integer i__1;
    doublereal d__1;

    /* Builtin functions */
    integer pow_ii(integer *, integer *);

    /* Local variables */
    static integer i__;

    /* Parameter adjustments */
    --g;
    --i4;
    --i6;
    --i36;

    /* Function Body */
    if (*i17 <= 0. && i4[*i40] <= 0.) {
  if (*l >= i4[*i14] - (d__1 = i4[*i14], abs(d__1)) / (doublereal) 
    pow_ii(&c__10, &i36[12])) {
      i6[*c__ + 31] = 0;
      goto L1;
  }
    } else {
  if (*p >= i4[*i11] - (d__1 = i4[*i11], abs(d__1)) / (doublereal) 
    pow_ii(&c__10, &i36[12])) {
      i6[*c__ + 31] = 0;
      goto L1;
  }
    }
    i__1 = *m;
    for (i__ = 1; i__ <= i__1; ++i__) {
  i4[*i19 + i__ - 1] = g[i__];
    }
    i4[*i40] = *i17;
    i4[*i14] = *l;
    i4[*i11] = *p;
    i6[*c__ + 31] = 1;
L1:
    if (*c__ == *f) {
  i6[31] = 0;
  i__1 = *f;
  for (i__ = 1; i__ <= i__1; ++i__) {
      i6[31] += i6[i__ + 31];
  }
  if (i6[31] > 1) {
      i6[31] = 1;
  }
    }
    return 0;
} /* i019_ */

/* Subroutine */ int i016_(integer *i12, integer *i29, integer *o, integer *
  i18, integer *i13, integer *i28, integer *i24, integer *i22)
{
    *o = 1;
    *i12 = 2;
    *i29 = 3;
    *i18 = 4;
    *i13 = 5;
    *i28 = 6;
    *i24 = 7;
    *i22 = 8;
    return 0;
} /* i016_ */

/* Subroutine */ int i017_(integer *m, integer *n, integer *i31, integer *i27,
   integer *i23, integer *i66, integer *i45, integer *i30, integer *i19,
   integer *i14, integer *i40, integer *i11, integer *w, integer *i49, 
  integer *i9, integer *i1, integer *i7, integer *i170)
{
    *i31 = 2;
    *i27 = *i31 + *m;
    *i23 = *i27 + 1;
    *i66 = *i23 + *n;
    *i45 = *i66 + 1;
    *i19 = *i45 + 1;
    *i14 = *i19 + *m * *i30;
    *i40 = *i14 + *i30;
    *i11 = *i40 + *i30;
    *i9 = *i11 + *i30;
    *w = *i9 + *i30 + 1;
    *i49 = *w + *i30;
    *i1 = *i49 + *m;
    *i7 = *i1 + 1;
    *i170 = *i7 + 1;
    return 0;
} /* i017_ */

/* Subroutine */ int i018_(integer *m, integer *i8, doublereal *g, doublereal 
  *i5, doublereal *i2, doublereal *i4,  integer *i31, 
  doublereal *i47, integer *i36)
{
    /* System generated locals */
    integer i__1;
    doublereal d__1;

    /* Builtin functions */
    double sqrt(doublereal);
    integer pow_ii(integer *, integer *);
    double d_nint(doublereal *);

    /* Local variables */
    static integer i__;
    extern doublereal i02_(doublereal *), i04_(doublereal *, doublereal *);
    static doublereal i34, i35;

    /* Parameter adjustments */
    --i2;
    --i5;
    --g;
    --i4;
    --i36;

    /* Function Body */
    i__1 = *m;
    for (i__ = 1; i__ <= i__1; ++i__) {
  i35 = (i2[i__] - i5[i__]) / *i47;
  if (i__ > *m - *i8) {
      if (i35 < 1. / sqrt(*i47)) {
    i35 = 1. / sqrt(*i47);
      }
  }
  i34 = i02_(&i4[1]);
  d__1 = i02_(&i4[1]);
  g[i__] = i4[*i31 + i__ - 1] + i35 * i04_(&i34, &d__1);
  if (g[i__] < i5[i__]) {
      if (i34 >= (doublereal) i36[2] * .1) {
    g[i__] = i5[i__] + (i5[i__] - g[i__]) / (doublereal) pow_ii(&
      c__3, &i36[3]);
    if (g[i__] > i2[i__]) {
        g[i__] = i2[i__];
    }
      } else {
    g[i__] = i5[i__];
      }
      goto L2;
  }
  if (g[i__] > i2[i__]) {
      if (i34 >= (doublereal) i36[2] * .1) {
    g[i__] = i2[i__] - (g[i__] - i2[i__]) / (doublereal) pow_ii(&
      c__3, &i36[3]);
    if (g[i__] < i5[i__]) {
        g[i__] = i5[i__];
    }
      } else {
    g[i__] = i2[i__];
      }
  }
L2:
  if (i__ > *m - *i8) {
      g[i__] = d_nint(&g[i__]);
  }
    }
    return 0;
} /* i018_ */

/* Subroutine */ int i010_(doublereal *p, doublereal *l, doublereal *i17, 
  doublereal *i45, doublereal *i16)
{
    /* System generated locals */
    doublereal d__1;

    /* Local variables */
    static doublereal i61;

    i61 = *l - *i45;
    if (*l <= *i45 && *i17 <= *i16) {
  *p = i61;
  return 0;
    } else {
  if (*l <= *i45) {
      *p = *i17;
      return 0;
  } else {
      if (*i17 <= i61) {
/* Computing 2nd power */
    d__1 = *i17;
    *p = i61 + d__1 * d__1 / (i61 * 2.) - *i17 / 2.;
      } else {
/* Computing 2nd power */
    d__1 = i61;
    *p = *i17 + d__1 * d__1 / (*i17 * 2.) - i61 / 2.;
      }
  }
    }
    return 0;
} /* i010_ */

/* Subroutine */ int i021_(integer *o, doublereal *i4,  integer *
  w)
{
    /* System generated locals */
    integer i__1;

    /* Local variables */
    static integer j, i57;

    /* Parameter adjustments */
    --i4;

    /* Function Body */
    i57 = 0;
    i__1 = *o;
    for (j = 1; j <= i__1; ++j) {
  i57 += j;
    }
    i__1 = *o;
    for (j = 1; j <= i__1; ++j) {
  i4[*w + j - 1] = (doublereal) (*o - j + 1) / (doublereal) i57;
    }
    return 0;
} /* i021_ */

/* Subroutine */ int i014_(doublereal *i17, doublereal *x, integer *n, 
  integer *i0, doublereal *i16)
{
    /* System generated locals */
    integer i__1;

    /* Local variables */
    static integer i__;

    /* Parameter adjustments */
    --x;

    /* Function Body */
    *i17 = 0.;
    if (*n == 0) {
  return 0;
    }
    i__1 = *n;
    for (i__ = 1; i__ <= i__1; ++i__) {
  if (x[i__] < -(*i16)) {
      *i17 -= x[i__];
  }
    }
    i__1 = *i0;
    for (i__ = 1; i__ <= i__1; ++i__) {
  if (x[i__] > *i16) {
      *i17 += x[i__];
  }
    }
    return 0;
} /* i014_ */

/* Subroutine */ int i024_(integer *i36, integer *m, integer *i8)
{
    /* Parameter adjustments */
    --i36;

    /* Function Body */
    if (*m - *i8 > 0) {
  i36[1] = 18;
  i36[2] = 6;
  i36[3] = 4;
  i36[4] = 4;
  i36[5] = 5;
  i36[6] = 18;
  i36[7] = 13;
  i36[8] = 5;
  i36[9] = 2;
  i36[10] = 7;
  i36[11] = 15;
  i36[12] = 8;
  i36[13] = 9;
  i36[14] = 7;
  i36[15] = 12;
  i36[16] = 5;
  i36[17] = 5;
  i36[18] = 5;
  i36[19] = 8;
  i36[20] = 37;
    } else {
  i36[1] = 4;
  i36[2] = 4;
  i36[3] = 3;
  i36[4] = 3;
  i36[5] = 2;
  i36[6] = 49;
  i36[7] = 37;
  i36[8] = 7;
  i36[9] = 0;
  i36[10] = 8;
  i36[11] = 3;
  i36[12] = 6;
  i36[13] = 4;
  i36[14] = 3;
  i36[15] = 3;
  i36[16] = 2;
  i36[17] = 10;
  i36[18] = 12;
  i36[19] = 4;
  i36[20] = 58;
    }
    return 0;
} /* i024_ */

/* Subroutine */ int alphabet_(char *a, integer *b)
{
    *b = 0;
    if (*(unsigned char *)a == 'A') {
  *b = 52;
    }
    if (*(unsigned char *)a == 'B') {
  *b = 28;
    }
    if (*(unsigned char *)a == 'C') {
  *b = 49;
    }
    if (*(unsigned char *)a == 'D') {
  *b = 30;
    }
    if (*(unsigned char *)a == 'E') {
  *b = 31;
    }
    if (*(unsigned char *)a == 'F') {
  *b = 32;
    }
    if (*(unsigned char *)a == 'G') {
  *b = 33;
    }
    if (*(unsigned char *)a == 'H') {
  *b = 34;
    }
    if (*(unsigned char *)a == 'I') {
  *b = 35;
    }
    if (*(unsigned char *)a == 'J') {
  *b = 36;
    }
    if (*(unsigned char *)a == 'K') {
  *b = 37;
    }
    if (*(unsigned char *)a == 'L') {
  *b = 38;
    }
    if (*(unsigned char *)a == 'M') {
  *b = 39;
    }
    if (*(unsigned char *)a == 'N') {
  *b = 40;
    }
    if (*(unsigned char *)a == 'O') {
  *b = 41;
    }
    if (*(unsigned char *)a == 'P') {
  *b = 42;
    }
    if (*(unsigned char *)a == 'Q') {
  *b = 43;
    }
    if (*(unsigned char *)a == 'R') {
  *b = 44;
    }
    if (*(unsigned char *)a == 'S') {
  *b = 45;
    }
    if (*(unsigned char *)a == 'T') {
  *b = 46;
    }
    if (*(unsigned char *)a == 'U') {
  *b = 47;
    }
    if (*(unsigned char *)a == 'V') {
  *b = 48;
    }
    if (*(unsigned char *)a == 'W') {
  *b = 29;
    }
    if (*(unsigned char *)a == 'X') {
  *b = 50;
    }
    if (*(unsigned char *)a == 'Y') {
  *b = 51;
    }
    if (*(unsigned char *)a == 'Z') {
  *b = 27;
    }
    if (*(unsigned char *)a == '0') {
  *b = 53;
    }
    if (*(unsigned char *)a == '1') {
  *b = 54;
    }
    if (*(unsigned char *)a == '2') {
  *b = 55;
    }
    if (*(unsigned char *)a == '3') {
  *b = 56;
    }
    if (*(unsigned char *)a == '4') {
  *b = 57;
    }
    if (*(unsigned char *)a == '5') {
  *b = 58;
    }
    if (*(unsigned char *)a == '6') {
  *b = 59;
    }
    if (*(unsigned char *)a == '7') {
  *b = 60;
    }
    if (*(unsigned char *)a == '8') {
  *b = 61;
    }
    if (*(unsigned char *)a == '9') {
  *b = 62;
    }
    if (*(unsigned char *)a == 'a') {
  *b = 23;
    }
    if (*(unsigned char *)a == 'b') {
  *b = 2;
    }
    if (*(unsigned char *)a == 'c') {
  *b = 3;
    }
    if (*(unsigned char *)a == 'd') {
  *b = 16;
    }
    if (*(unsigned char *)a == 'e') {
  *b = 5;
    }
    if (*(unsigned char *)a == 'f') {
  *b = 13;
    }
    if (*(unsigned char *)a == 'g') {
  *b = 7;
    }
    if (*(unsigned char *)a == 'h') {
  *b = 8;
    }
    if (*(unsigned char *)a == 'i') {
  *b = 9;
    }
    if (*(unsigned char *)a == 'j') {
  *b = 10;
    }
    if (*(unsigned char *)a == 'k') {
  *b = 11;
    }
    if (*(unsigned char *)a == 'l') {
  *b = 12;
    }
    if (*(unsigned char *)a == 'm') {
  *b = 6;
    }
    if (*(unsigned char *)a == 'n') {
  *b = 14;
    }
    if (*(unsigned char *)a == 'o') {
  *b = 15;
    }
    if (*(unsigned char *)a == 'p') {
  *b = 4;
    }
    if (*(unsigned char *)a == 'q') {
  *b = 17;
    }
    if (*(unsigned char *)a == 'r') {
  *b = 18;
    }
    if (*(unsigned char *)a == 's') {
  *b = 19;
    }
    if (*(unsigned char *)a == 't') {
  *b = 20;
    }
    if (*(unsigned char *)a == 'u') {
  *b = 21;
    }
    if (*(unsigned char *)a == 'v') {
  *b = 22;
    }
    if (*(unsigned char *)a == 'w') {
  *b = 1;
    }
    if (*(unsigned char *)a == 'x') {
  *b = 24;
    }
    if (*(unsigned char *)a == 'y') {
  *b = 25;
    }
    if (*(unsigned char *)a == 'z') {
  *b = 26;
    }
    if (*(unsigned char *)a == '_') {
  *b = 64;
    }
    if (*(unsigned char *)a == '(') {
  *b = 65;
    }
    if (*(unsigned char *)a == ')') {
  *b = 66;
    }
    if (*(unsigned char *)a == '+') {
  *b = 67;
    }
    if (*(unsigned char *)a == '-') {
  *b = 68;
    }
    if (*(unsigned char *)a == '&') {
  *b = 69;
    }
    if (*(unsigned char *)a == '.') {
  *b = 70;
    }
    if (*(unsigned char *)a == ',') {
  *b = 71;
    }
    if (*(unsigned char *)a == ':') {
  *b = 72;
    }
    if (*(unsigned char *)a == ';') {
  *b = 73;
    }
    if (*(unsigned char *)a == '*') {
  *b = 74;
    }
    if (*(unsigned char *)a == '=') {
  *b = 75;
    }
    if (*(unsigned char *)a == '/') {
  *b = 76;
    }
    if (*(unsigned char *)a == '!') {
  *b = 80;
    }
    if (*(unsigned char *)a == '[') {
  *b = 83;
    }
    if (*(unsigned char *)a == ']') {
  *b = 84;
    }
    return 0;
} 
#ifdef KR_headers
double pow();
double pow_dd(ap, bp) doublereal *ap, *bp;
#else
#undef abs
#include "math.h"
#ifdef __cplusplus
extern "C" {
#endif
double pow_dd(doublereal *ap, doublereal *bp)
#endif
{
return(pow(*ap, *bp) );
}
#ifdef __cplusplus
}
#endif
double d_nint(x)
doublereal *x;
{
double floor();

return( (*x)>=0 ?
 floor(*x + .5) : -floor(.5 - *x) );
}
#ifdef __cplusplus
extern "C" {
#endif
#ifdef KR_headers
double pow_di(ap, bp) doublereal *ap; integer *bp;
#else
double pow_di(doublereal *ap, integer *bp)
#endif
{
double pow, x;
integer n;
unsigned long u;
pow = 1;
x = *ap;
n = *bp;
if(n != 0)
 {
 if(n < 0)
  {
  n = -n;
  x = 1/x;
  }
 for(u = n; ; )
  {
  if(u & 01)
   pow *= x;
  if(u >>= 1)
   x *= x;
  else
   break;
  }
 }
return(pow);
}
#ifdef __cplusplus
}
#endif
#ifdef __cplusplus
extern "C" {
#endif
#ifdef KR_headers
integer pow_ii(ap, bp) integer *ap, *bp;
#else
integer pow_ii(integer *ap, integer *bp)
#endif
{
 integer pow, x, n;
 unsigned long u;
 x = *ap;
 n = *bp;
 if (n <= 0) {
  if (n == 0 || x == 1)
   return 1;
  if (x != -1)
   return x == 0 ? 1/x : 0;
  n = -n;
  }
 u = n;
 for(pow = 1; ; )
  {
  if(u & 01)
   pow *= x;
  if(u >>= 1)
   x *= x;
  else
   break;
  }
 return(pow);
 }
#ifdef __cplusplus
}
#endif
#ifdef KR_headers
double floor();
integer i_dnnt(x) doublereal *x;
#else
#undef abs
#include "math.h"
#ifdef __cplusplus
extern "C" {
#endif
integer i_dnnt(doublereal *x)
#endif
{
return (integer)(*x >= 0. ? floor(*x + .5) : -floor(.5 - *x));
}
#ifdef __cplusplus
}
#endif




#include <stdio.h>
#include <math.h>
#include <time.h>
double gettime(){ time_t second; second = time(NULL); return (double) second; }
int force_output( FILE *iout );
int warnings_and_erros( long int *iflag, FILE *iout );
int print_head(long int n, long int ni, long int m, long int me, double *param,
               long int maxeval, long int printeval, long int maxtime,
               long int save2file, char *key, FILE *iout);
int print_line(long int eval, double tnow, double f, double vio, FILE *iout);  
int print_solution(long int n,long int m,long int me,double *x,double *g, 
                   double *f,double *vio,double *xl,double *xu, double acc, 
                   long int eval, double tnow,long int iflag, FILE *iout);                 
int print_final(long int iflag, double tnow, double tmax, long int eval, long int maxeval,
                long int n, long int m, long int me, double *x, double *g, double *f,
                double *xl, double *xu, double *rw, double acc, long int wres, double *param, 
    FILE *iout);
/*CCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCC
C     This subroutine handles all printing commands for MIDACO.
C     Note that this subroutine is called independently from MIDACO and
C     MIDACO itself does not include any print commands (due to 
C     compiler portability and robustness).
CCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCC*/    
int midaco_print(int c, long int printeval, long int save2file, long int *iflag, long int *istop,
                double *f, double *g, double *x, double *xl, double *xu, long int n, long int ni,
                long int m, long int me, double *rw, long int maxeval, long int maxtime,
                double *param, long int P, char *key)
{
  int i;

  static double tstart, tnow, tmax;
  static long int eval;

  static double acc;
  static int tic;
  static long int kmax;
  static int q; 
  
  static int kx;  
  static int kf;
  static int kg; 
  static int kres;  
 
  static int wx;   
  static int wf; 
  static int wg;
  static int wres;    
  
  static int kbest;
  static int wbest;
  
  static double bestf[1];
  static double bestr[1];
  static double bestg[1000]; /* Increase size, if problems with N > 1000 are solved */
  static double bestx[1000]; /* Increase size, if problems with N > 1000 are solved */
  
  static int update;
  
  static double dummy_f, dummy_vio;
  
  static FILE* iout;
  static FILE* iout1;
  static FILE* iout2;    
  if(c == 2)
  {     
    tnow = gettime()-tstart;   
    eval = eval + P;         
    if(*iflag >= 10)
    {
      warnings_and_erros( iflag, &*iout);
      if(save2file > 0)
      {
        warnings_and_erros( iflag, &*iout1);
        warnings_and_erros( iflag, &*iout2);
      }
    }      
    if(printeval > 0)
    {            
        tic = tic + P;
        if((tic >= printeval)||(eval == P)||(*iflag >= 1))
        {
           if(eval > P){ tic = 0; } 
           if(rw[kres] == rw[wres]){
             kbest = kf;
             wbest = wf;
           }else{
             kbest = kres;
             wbest = wres;
           }                
           if((rw[wbest] < rw[kbest]) ||
              (*iflag >= 1)||(*iflag == -300)){
              
                 bestf[0] = rw[wf];
                 bestr[0] = rw[wres];
                 for(i=0;i<m;i++){  bestg[i] = rw[wg+i]; }
                 for(i=0;i<n;i++){  bestx[i] = rw[wx+i]; }  
           }
           else
           {        
                 bestf[0] = rw[kf];
                 bestr[0] = rw[kres];
                 for(i=0;i<m;i++){  bestg[i] = rw[kg+i]; }
                 for(i=0;i<n;i++){  bestx[i] = rw[kx+i]; }          
           }
           
           print_line( eval, tnow, bestf[0], bestr[0], &*iout);  
           if(save2file > 0)
           {
              print_line( eval, tnow, bestf[0], bestr[0], &*iout1);            
           }
           if(save2file > 0)
           {           
               update = 0;
               
               if( (bestr[0]  < dummy_vio)||
                  ((bestr[0] == dummy_vio)&&(bestf[0] < dummy_f)) )
                  {
                    dummy_f = bestf[0];
                    dummy_vio = bestr[0];
                    update = 1;
                  }

               if( update > 0 )
               {                              
                 fprintf(iout2,"\n\n            CURRENT BEST SOLUTION");
                  
                 print_solution( n, m, me, &*bestx, &*bestg, 
                                 &*bestf, &*bestr, &*xl, &*xu, acc, 
                                 eval, tnow, *iflag, &*iout2);
               }
              force_output( &*iout1 );
              force_output( &*iout2 );               
           }          
        }
    }            
    if(*istop == 0)
    {
        if(tnow >= tmax)     { *iflag = -999;}
        if(eval >= maxeval-1)
    {
     if(maxeval <= 99999999){ *iflag = -999;}
    }          
    }
  return 0;
  }
  if(c == 1)
  {     
    *iflag = 0;    
    *istop = 0;
    tmax = (double) maxtime;
    tstart = gettime(); 
    eval   = 0;
    if(param[0] <= 0.0)
    {
      acc = 0.001;
    }else{
      acc = param[0];
    }
    kmax = 100;
    q    = 2*n+m+(n+5)*kmax+8;    
    kx   = 1;
    kf   = 1+n;
    kg   = 1+n+1;
    kres = 1+n+1+m;        
    wx   = q;
    wf   = q+n;
    wg   = q+n+1;
    wres = q+n+1+m; 
    iout = stdout; 
    if(save2file > 0)
    {
      iout1 = fopen("MIDACO_SCREEN.TXT","w");
      iout2 = fopen("MIDACO_SOLUTION.TXT","w");
    }        
    bestf[0] = 1.0e+32;
    bestr[0] = 1.0e+32;
    dummy_f   = 1.0e+32;
    dummy_vio = 1.0e+32;    
    tic = 0;    
    if(printeval >= 1)
    {
        print_head(n,ni,m,me,&*param,maxeval,maxtime,
                   printeval,save2file,&*key,&*iout);
            
        if(save2file > 0)
        { 
            print_head(n,ni,m,me,&*param,maxeval,maxtime,
                       printeval,save2file,&*key,&*iout1);
        }
     }
     if(save2file > 0)
     {
         fprintf(iout2," MIDACO - SOLUTION\n");
         fprintf(iout2," -----------------\n");
         fprintf(iout2,"\n This file saves the current best solution X found by MIDACO.");
         fprintf(iout2,"\n This file is updated after every PRINTEVAL function evaluation,");
         fprintf(iout2,"\n if X has been improved.\n\n");
     }
     if(save2file > 0)
     {
       force_output( &*iout1 );
       force_output( &*iout2 );
     }
  }
  if((c == 3)&&(printeval > 0))
  {
 print_final(*iflag,tnow,tmax,eval,maxeval,n,m,me,&*x,&*g,&*f, 
    &*xl,&*xu,&*rw,acc,wres,param,&*iout); 
 if(save2file > 0)
 {
   print_final(*iflag,tnow,tmax,eval,maxeval,n,m,me,&*x,&*g,&*f, 
      &*xl,&*xu,&*rw,acc,wres,param,&*iout1);                    
   print_final(*iflag,tnow,tmax,eval,maxeval,n,m,me,&*x,&*g,&*f, 
      &*xl,&*xu,&*rw,acc,wres,param,&*iout2);              
   force_output( &*iout1 );
   force_output( &*iout2 );
   fclose( iout1 );
   fclose( iout2 );
 }        
  }
  return 0;
}
int print_head(long int n, long int ni, long int m, long int me, double *param,
               long int maxeval, long int maxtime, long int printeval, 
               long int save2file, char *key, FILE *iout)
{
  int i, dummy;
  fprintf(iout,"\n MIDACO 4.0    (www.midaco-solver.com)");
  fprintf(iout,"\n -------------------------------------\n\n");
  fprintf(iout," LICENSE-KEY:  ");for(i=0;i<=59;i++){fprintf(iout,"%c",key[i]);}
  fprintf(iout,"\n\n -------------------------------------\n");
  fprintf(iout," | N%7li    | MAXEVAL%12li |\n",n,maxeval); 
  fprintf(iout," | NI%6li    | MAXTIME%12li |\n",ni,maxtime);
  fprintf(iout," | M%7li    | PRINTEVAL%10li |\n",m,printeval);
  fprintf(iout," | ME%6li    | SAVE2FILE%10li |\n",me,save2file);
  fprintf(iout," -------------------------------------\n");
  dummy = 0; for(i=0;i<=8;i++){ if(param[i] != 0.0){ dummy=1;}}  
  if(dummy == 0)
  {
    fprintf(iout," | PARAMETER:   All by default (0)   |\n");
  }
  else
  {
    if((param[0] != 0.0)&&(param[0] < 0.000001)){
    fprintf(iout," | PARAM[0]%10.1e  ACCURACY G(X) |\n",param[0]);}else{

    if(param[0]==0){ fprintf(iout," | PARAM[0]%10.6f  ACCURACY G(X) |\n",0.001); }
    if(param[0] >0){ fprintf(iout," | PARAM[0]%10.6f  ACCURACY G(X) |\n",param[0]); }
    
    }
    fprintf(iout," -------------------------------------\n");
    fprintf(iout," | PARAM[1]  %10.1f  RANDOM-SEED |\n",param[1]);
    if(param[2] == 0.0){
    fprintf(iout," | PARAM[2]  %10.1f  FSTOP       |\n",param[2]);}else{
    fprintf(iout," | PARAM[2]  %10.3e  FSTOP       |\n",param[2]);}
    fprintf(iout," | PARAM[3]  %10.1f  AUTOSTOP    |\n",param[3]);
    if(param[4] == 0.0){
    fprintf(iout," | PARAM[4]  %10.1f  ORACLE      |\n",param[4]);}else{
    fprintf(iout," | PARAM[4]  %10.3e  ORACLE      |\n",param[4]);}
    fprintf(iout," | PARAM[5]  %10.1f  FOCUS       |\n",param[5]);
    fprintf(iout," | PARAM[6]  %10.1f  ANTS        |\n",param[6]);
    fprintf(iout," | PARAM[7]  %10.1f  KERNEL      |\n",param[7]);
    fprintf(iout," | PARAM[8]  %10.1f  CHARACTER   |\n",param[8]);
  }
  fprintf(iout," -------------------------------------\n\n");
  /* print headline */
  fprintf(iout," [     EVAL,    TIME]        OBJECTIVE FUNCTION VALUE         VIOLATION OF G(X)\n");
  fprintf(iout," ------------------------------------------------------------------------------\n"); 
  return 0;
}

int print_line(long int eval, double tnow, double f, double vio, FILE *iout)
{   
    if(fabs(f) <= 1.0e+10)
    {
      if(vio <= 1.0e+5)
      {
        fprintf(iout," [%9li,%8.0f]        F(X):%19.8f         VIO:%13.6f\n",eval,tnow,f,vio);      
      }else{
        fprintf(iout," [%9li,%8.0f]        F(X):%19.8f         VIO:%13.6e\n",eval,tnow,f,vio);      
      }
    }else{
      if(vio <= 1.0e+5)
      {
        fprintf(iout," [%9li,%8.0f]        F(X):%19.8e         VIO:%13.6f\n",eval,tnow,f,vio);      
      }else{
        fprintf(iout," [%9li,%8.0f]        F(X):%19.8e         VIO:%13.6e\n",eval,tnow,f,vio);      
      }    
    }
    return 0;
}
                                      
int print_solution(long int n,long int m,long int me,double *x,double *g, 
                   double *f,double *vio,double *xl,double *xu, double acc, 
                   long int eval, double tnow,long int iflag, FILE *iout)
{
    int i,j;
    int on;
    int profil;
    
    fprintf(iout,"\n --------------------------------------------\n");
    fprintf(iout," EVAL:%10li,  TIME:%8.2f,  IFLAG:%4li \n",eval,tnow,iflag);
    fprintf(iout," --------------------------------------------\n");
    
    if(fabs(f[0]) <= 1.0e+14)
    {
            fprintf(iout," F(X) =%38.15f \n",f[0]);
    }else{
            fprintf(iout," F(X) =%38.6e \n",f[0]);    
    }    
    fprintf(iout," --------------------------------------------\n");
    
     
    
    if(m > 0)
    {
        if(iflag < 100)
        {
          if(vio[0] <= 1.0e+12)
          {
                  fprintf(iout," VIOLATION OF G(X)%27.12f\n",vio[0]);
          }else{
                  fprintf(iout," VIOLATION OF G(X)%27.6e\n",vio[0]); 
          }
          fprintf(iout," --------------------------------------------\n");
        }
        for( i=0; i<m; i++)
        {                     
            if(i < me)
            {
              if(fabs(g[i]) <= acc)
              {
                fprintf(iout," g[%4i] =%16.8f  (EQUALITY CONSTR)\n",i,g[i]);
              }
              else
              {
                if(fabs(g[i]) <= 1.0e+14){
                fprintf(iout," g[%4i] =%16.8f  (EQUALITY CONSTR)  <---  INFEASIBLE  ( G NOT = 0 )\n",i,g[i]);
                }else{
                fprintf(iout," g[%4i] =%16.1e  (EQUALITY CONSTR)  <---  INFEASIBLE  ( G NOT = 0 )\n",i,g[i]);
                }
              }
            }
            if(i >= me)
            {
              if(g[i] > -acc)
              {
                if(fabs(g[i]) <= 1.0e+14){
                fprintf(iout," g[%4i] =%16.8f  (IN-EQUAL CONSTR)\n",i,g[i]);
                }else{
                fprintf(iout," g[%4i] =%16.1e  (IN-EQUAL CONSTR)\n",i,g[i]);
                }
              }
              else
              {
                if(fabs(g[i]) <= 1.0e+14){
                fprintf(iout," g[%4i] =%16.8f  (IN-EQUAL CONSTR)  <---  INFEASIBLE  ( G < 0 )\n",i,g[i]);
                }else{
                fprintf(iout," g[%4i] =%16.1e  (IN-EQUAL CONSTR)  <---  INFEASIBLE  ( G < 0 )\n",i,g[i]);
                }
              }
            }            
    
        }
        fprintf(iout," --------------------------------------------         BOUNDS-PROFIL    \n");
    }                
    for( i=0; i<n; i++)
    {
        profil = -1; on = 1; 
        if((on==1)&&( x[i] > xu[i]+1.0e-6 )){ profil = 91; on = 0; }
        if((on==1)&&( x[i] < xl[i]-1.0e-6 )){ profil = 92; on = 0; }        
        if((on==1)&&( xl[i] > xu[i]       )){ profil = 93; on = 0; }         
        if((on==1)&&( xl[i] == xu[i]      )){ profil = 90; on = 0; }
        if((on==1)&&( fabs(x[i]-xl[i]) < (xu[i]-xl[i])/1000.0 )){ profil =  0; on = 0; }                
        if((on==1)&&( fabs(xu[i]-x[i]) < (xu[i]-xl[i])/1000.0 )){ profil = 22; on = 0; }     
        for( j=1; j<=21; j++)
        {
          if((on==1)&&( x[i] <= xl[i] + ((double) j) * (xu[i]-xl[i])/21.0 )){ profil = j; on = 0; }        
        }  
if( fabs(x[i]) <= 1.0e+14 )
{
        if(profil == 0){fprintf(iout," x[%4i] =%34.15f;  # XL___________________ \n",i,x[i]);}
        if(profil == 1){fprintf(iout," x[%4i] =%34.15f;  # x____________________ \n",i,x[i]);}
        if(profil == 2){fprintf(iout," x[%4i] =%34.15f;  # _x___________________ \n",i,x[i]);}
        if(profil == 3){fprintf(iout," x[%4i] =%34.15f;  # __x__________________ \n",i,x[i]);}
        if(profil == 4){fprintf(iout," x[%4i] =%34.15f;  # ___x_________________ \n",i,x[i]);}
        if(profil == 5){fprintf(iout," x[%4i] =%34.15f;  # ____x________________ \n",i,x[i]);}
        if(profil == 6){fprintf(iout," x[%4i] =%34.15f;  # _____x_______________ \n",i,x[i]);}
        if(profil == 7){fprintf(iout," x[%4i] =%34.15f;  # ______x______________ \n",i,x[i]);}
        if(profil == 8){fprintf(iout," x[%4i] =%34.15f;  # _______x_____________ \n",i,x[i]);}
        if(profil == 9){fprintf(iout," x[%4i] =%34.15f;  # ________x____________ \n",i,x[i]);}
        if(profil ==10){fprintf(iout," x[%4i] =%34.15f;  # _________x___________ \n",i,x[i]);}
        if(profil ==11){fprintf(iout," x[%4i] =%34.15f;  # __________x__________ \n",i,x[i]);}
        if(profil ==12){fprintf(iout," x[%4i] =%34.15f;  # ___________x_________ \n",i,x[i]);}
        if(profil ==13){fprintf(iout," x[%4i] =%34.15f;  # ____________x________ \n",i,x[i]);}
        if(profil ==14){fprintf(iout," x[%4i] =%34.15f;  # _____________x_______ \n",i,x[i]);}
        if(profil ==15){fprintf(iout," x[%4i] =%34.15f;  # ______________x______ \n",i,x[i]);}
        if(profil ==16){fprintf(iout," x[%4i] =%34.15f;  # _______________x_____ \n",i,x[i]);}
        if(profil ==17){fprintf(iout," x[%4i] =%34.15f;  # ________________x____ \n",i,x[i]);}
        if(profil ==18){fprintf(iout," x[%4i] =%34.15f;  # _________________x___ \n",i,x[i]);}
        if(profil ==19){fprintf(iout," x[%4i] =%34.15f;  # __________________x__ \n",i,x[i]);}
        if(profil ==20){fprintf(iout," x[%4i] =%34.15f;  # ___________________x_ \n",i,x[i]);}
        if(profil ==21){fprintf(iout," x[%4i] =%34.15f;  # ____________________x \n",i,x[i]);}
        if(profil ==22){fprintf(iout," x[%4i] =%34.15f;  # ___________________XU \n",i,x[i]);}
        if(profil ==90){fprintf(iout," x[%4i] =%34.15f;  # WARNING: XL = XU      \n",i,x[i]);}
        if(profil ==91){fprintf(iout," x[%4i] =%34.15f; ***ERROR*** (X > XU)        \n",i,x[i]);}
        if(profil ==92){fprintf(iout," x[%4i] =%34.15f; ***ERROR*** (X < XL)        \n",i,x[i]);}
        if(profil ==93){fprintf(iout," x[%4i] =%34.15f; ***ERROR*** (XL > XU)       \n",i,x[i]);}
        if(profil < 0 ){fprintf(iout," PROFIL-ERROR");} 
}else{
        if(profil == 0){fprintf(iout," x[%4i] =%34.1e;  # XL___________________ \n",i,x[i]);}
        if(profil == 1){fprintf(iout," x[%4i] =%34.1e;  # x____________________ \n",i,x[i]);}
        if(profil == 2){fprintf(iout," x[%4i] =%34.1e;  # _x___________________ \n",i,x[i]);}
        if(profil == 3){fprintf(iout," x[%4i] =%34.1e;  # __x__________________ \n",i,x[i]);}
        if(profil == 4){fprintf(iout," x[%4i] =%34.1e;  # ___x_________________ \n",i,x[i]);}
        if(profil == 5){fprintf(iout," x[%4i] =%34.1e;  # ____x________________ \n",i,x[i]);}
        if(profil == 6){fprintf(iout," x[%4i] =%34.1e;  # _____x_______________ \n",i,x[i]);}
        if(profil == 7){fprintf(iout," x[%4i] =%34.1e;  # ______x______________ \n",i,x[i]);}
        if(profil == 8){fprintf(iout," x[%4i] =%34.1e;  # _______x_____________ \n",i,x[i]);}
        if(profil == 9){fprintf(iout," x[%4i] =%34.1e;  # ________x____________ \n",i,x[i]);}
        if(profil ==10){fprintf(iout," x[%4i] =%34.1e;  # _________x___________ \n",i,x[i]);}
        if(profil ==11){fprintf(iout," x[%4i] =%34.1e;  # __________x__________ \n",i,x[i]);}
        if(profil ==12){fprintf(iout," x[%4i] =%34.1e;  # ___________x_________ \n",i,x[i]);}
        if(profil ==13){fprintf(iout," x[%4i] =%34.1e;  # ____________x________ \n",i,x[i]);}
        if(profil ==14){fprintf(iout," x[%4i] =%34.1e;  # _____________x_______ \n",i,x[i]);}
        if(profil ==15){fprintf(iout," x[%4i] =%34.1e;  # ______________x______ \n",i,x[i]);}
        if(profil ==16){fprintf(iout," x[%4i] =%34.1e;  # _______________x_____ \n",i,x[i]);}
        if(profil ==17){fprintf(iout," x[%4i] =%34.1e;  # ________________x____ \n",i,x[i]);}
        if(profil ==18){fprintf(iout," x[%4i] =%34.1e;  # _________________x___ \n",i,x[i]);}
        if(profil ==19){fprintf(iout," x[%4i] =%34.1e;  # __________________x__ \n",i,x[i]);}
        if(profil ==20){fprintf(iout," x[%4i] =%34.1e;  # ___________________x_ \n",i,x[i]);}
        if(profil ==21){fprintf(iout," x[%4i] =%34.1e;  # ____________________x \n",i,x[i]);}
        if(profil ==22){fprintf(iout," x[%4i] =%34.1e;  # ___________________XU \n",i,x[i]);}
        if(profil ==90){fprintf(iout," x[%4i] =%34.1e;  # WARNING: XL = XU      \n",i,x[i]);}
        if(profil ==91){fprintf(iout," x[%4i] =%34.1e; ***ERROR*** (X > XU)        \n",i,x[i]);}
        if(profil ==92){fprintf(iout," x[%4i] =%34.1e; ***ERROR*** (X < XL)        \n",i,x[i]);}
        if(profil ==93){fprintf(iout," x[%4i] =%34.1e; ***ERROR*** (XL > XU)       \n",i,x[i]);}
        if(profil < 0 ){fprintf(iout," PROFIL-ERROR");}
}               
    }  
    fprintf(iout," \n ");
    return 0;
}   
int print_final(long int iflag, double tnow, double tmax, long int eval, long int maxeval,
                long int n, long int m, long int me, double *x, double *g, double *f,
                double *xl, double *xu, double *rw, double acc, long int wres, double *param, FILE *iout)
  {
      double vio[1];
      vio[0] = rw[wres];
      
      if((iflag == 1)||(iflag == 2))
      {
      if(tnow >=    tmax){ fprintf(iout,"\n OPTIMIZATION FINISHED  --->  MAXTIME REACHED");}
      if(eval >= maxeval){ fprintf(iout,"\n OPTIMIZATION FINISHED  --->  MAXEVAL REACHED");}
      }
      if((iflag == 3)||(iflag == 4))
      {      
          fprintf(iout,"\n OPTIMIZATION FINISHED  --->  AUTOSTOP (=%3li)",(long int)param[3]);
      }
      if((iflag == 5)||(iflag == 6))
      {      
          fprintf(iout,"\n OPTIMIZATION FINISHED  --->  FSTOP REACHED");
      }     
      fprintf(iout,"\n\n\n         BEST SOLUTION FOUND BY MIDACO");

      print_solution( n, m, me, &*x, &*g, &*f, &*vio, 
                      &*xl, &*xu, acc,eval, tnow, iflag, &*iout);
  return 0;
  }
  int warnings_and_erros( long int *iflag, FILE *iout )
  {
    if(*iflag < 100)
    {
      fprintf(iout,"\n *** WARNING ***   ( IFLAG =%6li )\n\n", *iflag);
    }
    else
    {
     fprintf(iout,"\n *** MIDACO INPUT ERROR ***   ( IFLAG =%6li )\n\n", *iflag);
    }
    return 0;
  }  
  
/**********************************************************************************/
/**********************************************************************************/
/**********************************************************************************/

  int force_output( FILE *iout )
  {
 /* This command flushes the MIDACO output to text files. */
 /* In case you have problems compiling this command, you can remove it. */
    fflush( iout );
    return 0;
  }
  /* END OF FILE */

