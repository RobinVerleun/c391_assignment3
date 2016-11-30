Our assignment submission comes with an easy to use makefile for questions 8 and 9.
Simply run make in the main directory and both files will compile.

For q8.c:
run the object file as such
./q8 <Database Name> <RDF Datapath>

For q9.C:
run the object file as such
./q9 <Database Name> <SPARQL Query Filepath>

Limitations on q9:

q9 cannot handle certain queries due to lack of implementation.

	1) If a SPARQL query uses the keyword FILTER
	2) If a SPARQL query has multiple variables in the same triple,
		ie) ?x rdf:type ?y