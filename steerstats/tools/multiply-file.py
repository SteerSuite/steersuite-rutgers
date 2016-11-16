import sys

if len(sys.argv) != 4:
    print "Wring number of arguments should be 3"
    print "Example: python multiple-file.py <file-name> <number-to-create> <destination-dir>"
    sys.exit()

output_file_prefix = "testcase-"
output_dir = sys.argv[3]

file_to_multiply = open(sys.argv[1], "r")

multiply_data = file_to_multiply.read()

for i in range(0, int(sys.argv[2])):
    # tmp_file = open(output_dir+output_file_prefix+str(i))
    # print output_dir+output_file_prefix+str(i)+".xml"
    tmp_file = open(output_dir+output_file_prefix+str(i)+".xml", "w")
    tmp_file.write(multiply_data)
    tmp_file.close()
    # print multiply_data