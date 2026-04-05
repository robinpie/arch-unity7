#
# Utility to transcode the Zeitgeist ontology from Python code
# to a C header file defining a collection of macros for the
# members of the ontology
#
# Currently this tool requires the lp:~zetgeist/zeitgeist/ontology_definition
# branch in order to work
#

import sys
import zeitgeist.datamodel as dm

def caseconv(name):
	"""
	Converts CamelCase to CAMEL_CASE
	"""
	result = ""
	for i in range(len(name) - 1) :
		if name[i].islower() and name[i+1].isupper():
			result += name[i].upper() + "_"
		else:
			result += name[i].upper()
	result += name[-1].upper()
	return result

def symbolname (symbol):
	try:
		ns, name = symbol[symbol.rfind("/")+1:].split("#")
		return "ZEITGEIST_%s_%s" % (ns.upper(), caseconv(name))
	except Exception, e:
		return symbol.uri.rpartition("#")[2].upper()

def buildCdoc(name, symbol, docprefix=""):
	"""
	Builds a C-style docstring for gtk-doc processing
	"""
	# Strip trailing ``(Display name: 'ReceiveEvent')`` text
	doc = symbol.__doc__
	doc = docprefix + doc
	doc = doc[:doc.find("``") - 1]

	# Fix link to external ontology
	doc = doc.replace(symbol.uri,
	                  '<ulink url="%s">%s</ulink>' % (symbol.uri, symbol.uri.replace("#", "&num;")))
	
	# List children
	children = [ "#" + symbolname(child) for child in symbol.get_children()]
	if children:
		doc += "\n\n Children: %s" % ", ".join(children)
	else:
		doc += "\n\n Children: None"

	# List parents
	parents = [ "#" + symbolname(parent) for parent in symbol.get_parents()]
	if parents and not parents == ["#INTERPRETATION"] and not parents == ["#MANIFESTATION"]:
		doc += "\n\n Parents: %s" % ", ".join(parents)
	else:
		doc += "\n\n Parents: None"
	
	# Convert docstring to gtk-doc style C comment
	doc = doc.replace("\n", "\n *")
	doc = "/**\n * %s:\n *\n * %s\n */" % (name, doc)
	return doc

if __name__ == "__main__":
	interpretations = []
	manifestations = []
	for interp in dm.Interpretation.get_all_children():
		try:
			name = symbolname (interp)
			doc = buildCdoc(name, interp, docprefix="Macro defining the interpretation type ")
			stmt = '%s\n#define %s "%s"' % (doc, name, interp.uri)
			interpretations.append(stmt)
		except Exception, e:
			print >> sys.stderr, "Failed to convert %s: %s" % (interp, e)
	interpretations.sort()
	for interp in interpretations :
		print interp
		print ""
	
	print "\n#\n#\# SKIP \n#\n#\n#"

	for manif in dm.Manifestation.get_all_children():
		try:
			name = symbolname (manif)
			doc = buildCdoc(name, manif, docprefix="Macro defining the manifestation type ")
			stmt = '%s\n#define %s "%s"' % (doc, name, manif.uri)
			manifestations.append(stmt)
		except Exception, e:
			print >> sys.stderr, "Failed to convert %s: %s" % (manif, e)

	manifestations.sort()
	for manif in manifestations :
		print manif
		print ""
