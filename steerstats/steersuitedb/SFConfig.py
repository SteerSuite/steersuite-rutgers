import psycopg2
# from Sequence import ConfigSequence

class SFConfig(object):
    _id_name = "sf_ai_config_id"
    _table_name = "sf_ai_config"
    _insert_order = """  
(sf_ai_config_id,
hidac_acceleration,
hidac_personal_space_threshold,
hidac_agent_repulsion_importance,
hidac_query_radius,
hidac_body_force,
hidac_agent_body_force,
hidac_sliding_friction_force,
hidac_agent_b,
hidac_agent_a,
hidac_wall_b,
hidac_wall_a,
hidac_max_speed
)"""

    """
    sf_ai_config
(

sf_ai_config_id integer NOT NULL references config(config_id),
hidac_acceleration double precision not null,
hidac_personal_space_threshold double precision not null,
hidac_agent_repulsion_importance double precision not null,
hidac_query_radius double precision not null,
hidac_body_force double precision not null,
hidac_agent_body_force double precision not null,
hidac_agent_b double precision not null,
hidac_agent_a double precision not null,
hidac_wall_b double precision not null,
hidac_wall_a double precision not null,
hidac_max_speed double precision not null,
CONSTRAINT sf_ai_config_pkey PRIMARY KEY (sf_ai_config_id)

)   ;"""

    # data should be a properly formed dictionary
    def __init__(self, data):
        self._data=data
    
    def getConfigData(self, cur, n):
        cur.execute("SELECT * FROM " + self._table_name + " where " + self._id_name + " = "+ str(n))
        row = cur.fetchone()
        return SFConfig(row)
    
    def setValuesFromDict(self, valuesDict):
        self._data = valuesDict

    def insertConfig(self, cur, data):
        try:
            cur.execute("INSERT INTO " + self._table_name + " " +
                        self._insert_order + " " +
                        "VALUES(%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s)", # 12 of them
                        ( 
                           data['sf_ai_config_id'],
                           data['hidac_acceleration'],
                           data['hidac_personal_space_threshold'],
                           data['hidac_agent_repulsion_importance'],
                           data['hidac_query_radius'],
                           data['hidac_body_force'],
                           data['hidac_agent_body_force'],
                           data['hidac_sliding_friction_force'],
                           data['hidac_agent_b'],
                           data['hidac_agent_a'],
                           data['hidac_wall_b'],
                           data['hidac_wall_a'],
                           data['hidac_max_speed']
                            
                        )
                        )
            return data['sf_ai_config_id']
            
        except psycopg2.DatabaseError, e:
            print 'Error SFConfig: %s' % e    
            
    def insertConfig2(self, cur):
        return self.insertConfig(cur, self._data)
            # sys.exit(1)          
            
