from multiprocessing import Semaphore
import time
from multiprocessing import Process
import random
import os


list_ = [0 for x in range(20)]

listSem = Semaphore(3)
printSem = Semaphore(1)


def doSomething(x):
    time.sleep(1.2 * random.random())
    listSem.acquire()
    list_[x] = x*x
    print list_
    time.sleep(1.2 * random.random())
    listSem.release()
    
    printSem.acquire()
    print "Process " + str(os.getpid()) + " is done listing."
    time.sleep(2.2 * random.random())
    printSem.release()
    
    
    

steersimProcesses = []

# adding the , makes it a list so it can be iterable
for i in range(20):
    steersimProcesses.append(Process(target=doSomething, args=(i,)))
    
for proc in steersimProcesses:
    proc.start()
    
for proc in steersimProcesses:
    proc.join()
    
