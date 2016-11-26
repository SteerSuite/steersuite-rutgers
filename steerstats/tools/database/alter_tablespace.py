 #!/usr/bin/python

# will generate the sql file needed to alter the tablespace for a database.


import unittest
import psycopg2
import psycopg2.extras
import sys

sys.path.append("../../")

#
# Configuration
# 
host = 'localhost';            # The host on which the database resides.
user = 'steeruser';            # The username to access the database.
password = 'steersuite';            # The password to access the database.
db = 'clustering_steersuitedb';            # The database to move.
tablespace = 'steer_store';    # The tablespace to move the database to.
output = ""
 
#
# Application
#
conn = psycopg2.connect(database=db, user=user, password=password)
conn.set_isolation_level(0)  
cur = conn.cursor(cursor_factory=psycopg2.extras.DictCursor)
 
# Create SQL code to put new tables and indexes in the new tablespace.
output = output + "ALTER DATABASE " + db + " SET default_tablespace = " + tablespace + ";\n"
 
# Select all tables from the database.
tableQuery = "SELECT * FROM pg_tables where tableowner='" + user + "' ORDER BY tablename"
 
cur.execute(tableQuery)

db_tables = cur.fetchall()
# print db_tables 
for table in db_tables:
    schemaName = table['schemaname']
    tableName = table['tablename']
 
    # Create SQL code to move the table to the new tablespace.
    output = output + "ALTER TABLE "+ schemaName +"." + tableName + " SET TABLESPACE " + tablespace + ";\n";
    # print output
 
    # Select all indexes from the table.
    indexQuery = "SELECT * FROM pg_indexes WHERE schemaname = '" + schemaName + "' AND tablename = '" + tableName + "' ORDER BY indexname"
    #print indexQuery
    cur.execute(indexQuery)
    #print cur.statusmessage
    db_indexes = cur.fetchall()
    # print db_indexes
    for index in db_indexes:
        #print dict(index)
        indexName = index['indexname']
 
        # Create SQL code to move the index to the new tablespace.
        output = output + "ALTER INDEX " + schemaName + "." + indexName + " SET TABLESPACE " + tablespace+ ";\n";
    

 
# Write the resulting SQL code to a file.
filename = 'migrate_' + host + '_' + db + '_to_' + tablespace + '.sql'
sqlfile = open('output/' + filename, 'w')
sqlfile.write(output)
 
