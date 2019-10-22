;
; bulk-save.scm
;
; Simple example of the bulk save of an entire AtomSpace.
;
(use-modules (opencog))
(use-modules (opencog persist))
(use-modules (opencog persist-ipfs))

; hostname of IPFS server, and a key name ...
; (ipfs-open "ipfs://localhost/demo-atomspace")
; (ipfs-open "ipfs://localhost:5001/demo-atomspace")
(ipfs-open "ipfs:///demo-atomspace")

; Create some atoms
(Concept "example concept")
(Evaluation
	(Predicate "Some relationship")
	(List (Concept "foo") (Concept "bar"))))

; Bulk-save of the entire AtomSpace
(ipfs-store)

; Review the stats
(ipfs-stats)

; Close the connection.
(ipfs-close)

; The End.  That's all for now.