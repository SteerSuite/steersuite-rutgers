from optparse import OptionParser


parser = OptionParser()
parser.add_option("-f", "--file", dest="filename",
                  help="write report to FILE", metavar="FILE")
parser.add_option("-v", "--verbose",
                  action="store_const", dest="verbose", default=False,
                  help="don't print status messages to stdout")
parser.add_option("--recordDIR", "--recordingDirectory",
                  action="store", dest="recordingDirectory", default="scenarioRecordings",
                  help="Specify the directory that recording files will be stored")

parser.add_option("--frameDIR", "--frameDirectory",
                  action="store", dest="frameDirectory", default="scenarioFrames",
                  help="Specify the directory that frame image files will be stored")

parser.add_option("--testcaseDIR", "--testcaseDirectory",
                  action="store", dest="testcaseDirectory", default="testCases",
                  help="Specify the directory that scenarios files will be stored")

parser.add_option("--scenarioDIR", "--scenarioDirectory",
                  action="store", dest="scenarioDirectory", default="scenarios",
                  help="Specify the directory that testcase files will be read from")

parser.add_option("--benchFile", "--benchmarkFileName",
                  action="store", dest="benchmarkFileName", default="test.log",
                  help="The name of the benchmark file.")

parser.add_option("--numScenarios", "--numTests",
                  action="store", dest="numScenarios", default=0,
                  help="The number of scenarios to be executed")

parser.add_option("--scenarioSetInitId", "--scenarioSetStartId",
                  action="store", dest="scenarioSetStartId", default=-1,
                  help="The scenario ID to start with")

parser.add_option("--ai", "--steeringAlgorithm",
                  action="store", dest="steeringAlgorithm", default="SimpleAI",
                  metavar="module", help="Specify the directory that  files will be stored")


(options, args) = parser.parse_args()

print options
