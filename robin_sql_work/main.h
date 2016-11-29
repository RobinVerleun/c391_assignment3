
/*
	SELECT * 
	FROM tables
	WHERE

	select t1.sub from relational_rdf t1, relational_rdf t2 where 
   ...> t1.prd = "http://www.w3.org/1999/02/22-rdf-syntax-ns#type" AND
   ...> t1.obj = "http://schema.org/City" AND
   ...> t2.prd = "http://dbpedia.org/ontology/country" AND
   ...> t2.obj = "http://dbpedia.org/resource/Canada" AND
   ...> t1.sub = t2.sub;

   */
