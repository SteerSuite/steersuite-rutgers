from lxml import etree
import sys
data = open(sys.argv[1],'r').read()

doc = etree.XML(data,etree.XMLParser(remove_blank_text=True))

for parent in doc.xpath('//*[./*]'): # Search for parent elements
  parent[:] = sorted(parent,key=lambda x: x.get("name"))

print etree.tostring(doc,pretty_print=True)