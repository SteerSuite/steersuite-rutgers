#!/usr/bin/python

import unittest
import psycopg2
import psycopg2.extras
import sys

sys.path.append("../")

from steersuite.OptimizationExperiment import OptimizationExperiment

ppr_param_xml_file = open("data/ppr-param-config.xml")
ppr_param_xml_data = ppr_param_xml_file.read()


O_ex = OptimizationExperiment(ppr_param_xml_data)

OptimParams =  O_ex.parseExperimentXML()

for param in OptimParams:
    print param.get_name() + " type: " + str(param.get_type())
    print param.get_original()
