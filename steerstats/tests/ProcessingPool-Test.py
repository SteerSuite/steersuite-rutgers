from multiprocessing import Pool
import os

def f(x):
    print "X is: " + str(x) + " and the pid is: " + str(os.getpid())
    if ( x <= 1):
        return 1
    else:
        return x*f(x-1)+f(x-2)

if __name__ == '__main__':
    pool = Pool(processes=4)              # start 4 worker processes
    result = pool.apply_async(f, [10])    # evaluate "f(10)" asynchronously
    print result.get(timeout=1)           # prints "100" unless your computer is *very* slow
    print pool.map(f, range(40)) 