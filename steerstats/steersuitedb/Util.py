import psycopg2

def getTime(cur):
    try:
        cur.execute("SELECT now()")
        return cur.fetchone()[0]
    except psycopg2.DatabaseError, e:
        print 'Error %s' % e    
