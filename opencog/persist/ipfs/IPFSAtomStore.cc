/*
 * IPFSAtomStore.cc
 * Save of individual atoms.
 *
 * Copyright (c) 2008,2009,2013,2017,2019 Linas Vepstas <linas@linas.org>
 * SPDX-License-Identifier: AGPL-3.0-or-later
 */

#include <stdlib.h>
#include <unistd.h>

#include <opencog/atoms/base/Atom.h>
#include <opencog/atomspace/AtomSpace.h>
#include <opencog/atomspaceutils/TLB.h>

#include "IPFSAtomStorage.h"

using namespace opencog;

/* ================================================================ */
/**
 * Recursively store the indicated atom and all of the values attached
 * to it.  Also store it's outgoing set, and all of the values attached
 * to those atoms.  The recursive store is unconditional.
 *
 * By default, the actual store is done asynchronously (in a different
 * thread); this routine merely queues up the atom. If the synchronous
 * flag is set, then the store is performed in this thread, and it is
 * completed (sent to the Postgres server) before this method returns.
 */
void IPFSAtomStorage::storeAtom(const Handle& h, bool synchronous)
{
	rethrow();

	// If a synchronous store, avoid the queues entirely.
	if (synchronous)
	{
		if (not_yet_stored(h)) do_store_atom(h);
		store_atom_values(h);
		return;
	}
	// _write_queue.enqueue(h);
	_write_queue.insert(h);
}

/**
 * Synchronously store a single atom. That is, the actual store is done
 * in the calling thread.  All values attached to the atom are also
 * stored.
 *
 * Returns the height of the atom.
 */
int IPFSAtomStorage::do_store_atom(const Handle& h)
{
	if (h->is_node())
	{
		do_store_single_atom(h, 0);
		return 0;
	}

	int lheight = 0;
	for (const Handle& ho: h->getOutgoingSet())
	{
		// Recurse.
		int heig = do_store_atom(ho);
		if (lheight < heig) lheight = heig;
	}

	// Height of this link is, by definition, one more than tallest
	// atom in outgoing set.
	lheight ++;
	do_store_single_atom(h, lheight);
	return lheight;
}

void IPFSAtomStorage::vdo_store_atom(const Handle& h)
{
	try
	{
		if (not_yet_stored(h)) do_store_atom(h);
		store_atom_values(h);
	}
	catch (...)
	{
		_async_write_queue_exception = std::current_exception();
	}
}

bool IPFSAtomStorage::not_yet_stored(const Handle& h)
{
	throw SyntaxException(TRACE_INFO, "Not implemented!\n");
	return true;
}

/* ================================================================ */

/**
 * Store just this one single atom.
 * Atoms in the outgoing set are NOT stored!
 * The store is performed synchronously (in the calling thread).
 */
void IPFSAtomStorage::do_store_single_atom(const Handle& h, int aheight)
{
	throw SyntaxException(TRACE_INFO, "Not implemented!\n");
	_store_count ++;

	if (bulk_store and _store_count%100000 == 0)
	{
		time_t secs = time(0) - bulk_start;
		double rate = ((double) _store_count) / secs;
		unsigned long kays = ((unsigned long) _store_count) / 1000;
		printf("\tStored %luK atoms in %d seconds (%d per second)\n",
			kays, (int) secs, (int) rate);
	}
}

/* ============================= END OF FILE ================= */
