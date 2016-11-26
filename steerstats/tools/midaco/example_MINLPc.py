import math
########################################################################
#
#   This is an Example Call of MIDACO 4.0
#   -------------------------------------
#
#   MIDACO solves the general Mixed Integer Non-Linear Program (MINLP):
#
#
#     Minimize     F(X)           where X(1,...N-NI)   is *CONTINUOUS*
#                                 and   X(N-NI+1,...N) is *DISCRETE* 
#
#     Subject to:  G_j(X)  =  0   (j=1,...ME)     Equality Constraints
#                  G_j(X) >=  0   (j=ME+1,...M) Inequality Constraints
#
#     And bounds:  XL <= X <= XU
#
#
#   The problem statement of this example is given below in the function
#   '[f,g] = problem_function(x)'. You can use this example as a template to 
#   run MIDACO on your own problem. In order to do so: Replace the objective 
#   function 'F' (and in case the constraints 'G') given below with your own 
#   problem functions. Then simply follow the instruction steps 1 to 3 given 
#   in this file. 
#
#   See the MIDACO Header and MIDACO User Guide for more information.
#
#   Author(C): Dr. Martin Schlueter           
#              Information Initiative Center,
#              Division of Large Scale Computing Systems,
#              Hokkaido University, Japan.
#
#      Email:  info@midaco-solver.com
#      URL:    http://www.midaco-solver.com
#
########################################################################
########################## PROBLEM FUNCTION ############################
########################################################################
def problem_function(x):

    # Objective function value F(X)
    # the area of square defined by two points
    pillarRadius=0.5
    """
    f = (x[0] - 1)*(x[0] - 1) + \
        (x[1] - 2)*(x[1] - 2) + \
        (x[2] - 3)*(x[2] - 3) + \
        (x[3] - 4)*(x[3] - 4) + \
        1.23456789
        """
    f = math.fabs(x[0] - x[2])*math.fabs(x[1]-x[3])
        
    g = [0.0]*3 # Initialize array for constraints G(X)
    
    #  Equality constraints G(X) = 0 MUST COME FIRST in g[0:me-1]
    g[0] = x[0] - 1.0
    # Inequality constraints G(X) >= 0 MUST COME SECOND in g[me:m-1] 
    g[1] = math.fabs(x[1] - x[3]) - (pillarRadius*2)        
    g[2] = math.fabs(x[0] - x[2]) - (pillarRadius*2) 
    
    return f,g
########################################################################
########################### MAIN PROGRAM ###############################
########################################################################

#  STEP 1.A: Define problem dimensions
#  ***********************************
n  = 4 # Number of variables (in total)
ni = 2 # Number of integer variables (0 <= ni <= n)
m  = 3 # Number of constraints (in total) 
me = 1 # Number of equality constraints (0 <= me <= m)

#  STEP 1.B: Define lower and upper bounds 'xl' & 'xu'  
#  ***************************************************  
xl = [ 1, 1, 1, 1]
xu = [ 4, 4, 4, 4]

#  STEP 1.C: Define starting point 'X'  
#  ***********************************  
x = [0.0]*n
for i in range(0, n):
      x[i] = xl[i] # Here for example: starting point = lower bounds
    
########################################################################
   
#  STEP 2.A: Define stopping criteria 
#  ********************************** 
maxeval = 1000   # Maximum number of function evaluation (e.g. 1000000) 
maxtime = 60*60*24  # Maximum time limit in Seconds (e.g. 1 Day = 60*60*24) 

#  STEP 2.B: Choose printing options  
#  *********************************  
printeval = 10  # Print-Frequency for current best solution (e.g. 1000) 
save2file = 1     # Save SCREEN and SOLUTION to TXT-files [ 0=NO/ 1=YES] 

########################################################################

#  STEP 3: Choose MIDACO parameters  (ONLY FOR ADVANCED USERS)   
#  *******************************
param = [0.0]*9
param[0] =  0.0   #  ACCURACY      (default value is 0.001)      
param[1] =  0.0   #  RANDOM-SEED   (e.g. 1, 2, 3,... 1000)       
param[2] =  0.0   #  FSTOP                                       
param[3] =  0.0   #  AUTOSTOP      (e.g. 1, 5, 20, 100,... 500)   
param[4] =  0.0   #  ORACLE                                      
param[5] =  0.0   #  FOCUS         (e.g. +/- 10, 500,... 100000) 
param[6] =  0.0   #  ANTS          (e.g. 2, 10, 50, 100,... 500) 
param[7] =  0.0   #  KERNEL        (e.g. 2, 5, 15, 30,... 100)   
param[8] =  0.0   #  CHARACTER  
       
########################################################################
########################################################################
########################################################################
####################### IMPORT MIDACO LIBRARY ##########################
########################################################################
########################################################################
########################################################################
import ctypes; from ctypes import *; import os; import os.path
# Specify name of the MIDACO library depending on OS
if (os.name == "posix"):  lib_name = "midacopy.so"  # Linux//Mac/Cygwin
else:                     lib_name = "midacopy.dll" # Windows
# Specify path were the MIDACO library is expected  
lib_path=os.path.dirname(os.path.abspath(__file__))+os.path.sep+lib_name
# Assign CLIB as name for MIDACO library
CLIB = ctypes.CDLL(lib_path)      
########################################################################
######################### CALL MIDACO SOLVER ###########################
########################################################################
key_ = \
c_char_p('MIDACO_LIMITED_VERSION___[CREATIVE_COMMONS_BY-NC-ND_LICENSE]') 
########################################################################
# Create c-types arguments and initialize workspace and flags
n_  = pointer(c_long(n));  n__   = c_long(n);  xl_    = (c_double * n)()
ni_ = pointer(c_long(ni)); ni__  = c_long(ni); xu_    = (c_double * n)()
m_  = pointer(c_long(m));  m__   = c_long(m);  x_     = (c_double * n)()
me_ = pointer(c_long(me)); me___ = c_long(me); param_ = (c_double * 9)()
printeval_ = c_long(printeval);  iflag_ = pointer(c_long(0)) 
save2file_ = c_long(save2file);  istop_ = pointer(c_long(0))
maxeval_   = c_long(maxeval); p = 1; p__= c_long(p);
maxtime_   = c_long(maxtime);        p_ = pointer(c_long(p))  
lrw_ = pointer(c_long(110*n+2*m+1000)); rw_ =  (c_double * lrw_[0])()
liw_ = pointer(c_long(2*n+1+1000));     iw_ =  (c_long *   liw_[0])()   
for i in range(0, lrw_[0]):  rw_[i] = c_double(0.0)    
for i in range(0, liw_[0]):  iw_[i] = c_long(0); 
for i in range(0, 9): param_[i] = c_double(param[i])
for i in range(0, n): xl_[i]    = c_double(xl[i])  
for i in range(0, n): xu_[i]    = c_double(xu[i])  
for i in range(0, n): x_[i]     = c_double( x[i])
f_ = (c_double * 1)()
if ( m > 0): g_ = (c_double * m)()
if ( m ==0): g_ = (c_double * 1)() # Dummy for unconstrained problems
########################################################################
# Print MIDACO Head information
CLIB.midaco_print(1,printeval_,save2file_,iflag_,istop_,f_,g_,x_,xl_,xu_,\
                  n__,ni__,m__,me___,rw_,maxeval_,maxtime_,param_,p__,key_)      
########################################################################
while True: # Call MIDACO by reverse communication loop 

  [ f_[0], g_[:] ] =  problem_function(x_[:]) # Evaluate F(X) and G(X)  
               
  CLIB.midaco(p_,n_,ni_,m_,me_,x_,f_,g_,xl_,xu_, \
              iflag_,istop_,param_,rw_,lrw_,iw_,liw_,key_)
  CLIB.midaco_print(2,printeval_,save2file_,iflag_,istop_, \
                    f_,g_,x_,xl_,xu_,n__,ni__,m__,me___,rw_, \
                    maxeval_,maxtime_,param_,p__,key_)              
  
  if istop_[0] != 0: break  
########################################################################  
[ f_[0], g_[:] ] =  problem_function( x_[:] ) # Evaluate F(X) and G(X)  
# Printing final solution    
CLIB.midaco_print(3,printeval_,save2file_,iflag_,istop_,f_,g_,x_,xl_,xu_,\
                  n__,ni__,m__,me___,rw_,maxeval_,maxtime_,param_,p__,key_)

for xi in x_:
    print xi                      
########################################################################
############################ END OF FILE ###############################
########################################################################
      
      
