/// \file
/// \brief The tally_residue_ids class definitions

/// \copyright
/// CATH Tools - Protein structure comparison tools such as SSAP and SNAP
/// Copyright (C) 2011, Orengo Group, University College London
///
/// This program is free software: you can redistribute it and/or modify
/// it under the terms of the GNU General Public License as published by
/// the Free Software Foundation, either version 3 of the License, or
/// (at your option) any later version.
///
/// This program is distributed in the hope that it will be useful,
/// but WITHOUT ANY WARRANTY; without even the implied warranty of
/// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
/// GNU General Public License for more details.
///
/// You should have received a copy of the GNU General Public License
/// along with this program.  If not, see <http://www.gnu.org/licenses/>.

#include "tally_residue_ids.hpp"

#include <boost/algorithm/string/join.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/log/trivial.hpp>
#include <boost/range/algorithm.hpp>
#include <boost/range/algorithm/find.hpp>

#include "biocore/residue_id.hpp"
#include "common/algorithm/contains.hpp"
#include "common/algorithm/is_uniq_for_unordered.hpp"
#include "common/boost_addenda/range/adaptor/lexical_casted.hpp"
#include "common/boost_addenda/range/indices.hpp"
#include "common/exception/invalid_argument_exception.hpp"

#include <set>

using namespace cath;
using namespace cath::common;
using namespace std;

using boost::algorithm::join;
using boost::log::trivial::trace;

/// \brief Tally up the residue records parsed from a PDB file with those parsed from a corresponding DSSP/WOLF file
///
/// \pre Every valid DSSP/WOLF residue should have an equivalent PDB residue and matching residues should be in the same order in both.
///
/// The DSSP file may have a NULL residue to indicate a break in the chain or a residue that cannot be properly represented.
/// The WOLF file may just skip some residues.
///
/// Since this is attempting to find the PDB residue that matches each DSSP/WOLF residue, it is implemented
/// with a simple loop through each of the DSSP/WOLF residues whilst maintaining a counter to point to the
/// current PDB residue name.
///
/// \returns A list of pairs of equivalent indices (offset 0) between residues in the PDB and DSSP/WOLF
size_size_pair_vec cath::file::tally_residue_ids(const residue_id_vec &arg_pdb_residue_ids,                             ///< A list of residue_ids parsed from the PDB file
                                                 const residue_id_vec &arg_dssp_or_wolf_residue_ids,                    ///< A list of residue_ids parsed from the DSSP/WOLF file (with a null residue represented with an empty string)
                                                 const bool           &arg_permit_breaks_without_null_residues,         ///< (true for WOLF files and false for DSSP files (at least >= v2.0)
                                                 const bool           &arg_permit_head_tail_break_without_null_residue, ///< (true even for DSSP v2.0.4: file for chain A of 1bvs stops with neither residue 203 or null residue (verbose message: "ignoring incomplete residue ARG  (203)")
                                                 const size_set       &arg_skippable_pdb_indices                        ///< A list of the indices of PDB residue names that should always be considered for being skipped over to find a match to the next DSSP/WOLF residue
                                                 ) {
	BOOST_LOG_TRIVIAL( trace ) << "Tallying PDB residue names: "
	                           << join( arg_pdb_residue_ids          | lexical_casted<string>(), "," )
	                           << " with DSSP/WOLF residue names: "
	                           << join( arg_dssp_or_wolf_residue_ids | lexical_casted<string>(), "," );

	// Sanity check the inputs
	//
	/// \todo Add check that arg_dssp_or_wolf_residue_ids has no duplicates other than empty strings and

	// Check that arg_pdb_residue_ids contains no empty strings
	if ( contains( arg_pdb_residue_ids, residue_id{} ) ) {
		BOOST_THROW_EXCEPTION(invalid_argument_exception("PDB residues should not contain empty residues names"));
	}
	// Check that arg_pdb_residue_ids contains no duplicates
	if ( ! is_uniq_for_unordered( arg_pdb_residue_ids ) ) {
		BOOST_THROW_EXCEPTION(invalid_argument_exception("PDB residues should not contain duplicate entries " + join( arg_pdb_residue_ids | lexical_casted<string>(), "," ) ));
	}

	// Check that arg_dssp_or_wolf_residue_ids contains no consecutive duplicates (not even duplicate empty strings)
	if ( contains_adjacent_match( arg_dssp_or_wolf_residue_ids ) ) {
		BOOST_THROW_EXCEPTION(invalid_argument_exception("DSSP residues should not contain duplicate consecutive entries (not even null residues)"));
	}

	size_size_pair_vec alignment;
	const auto num_pdb_residues          = arg_pdb_residue_ids.size();
	const auto num_dssp_or_wolf_residues = arg_dssp_or_wolf_residue_ids.size();
	const auto min_num_residues          = min( num_pdb_residues, num_dssp_or_wolf_residues );
	alignment.reserve( min_num_residues );

	// Loop through the DSSP/WOLF residues, whilst also indexing through the PDB residues
	size_t pdb_residue_ctr = 0;
	for (const size_t &dssp_residue_ctr : indices( num_dssp_or_wolf_residues ) ) {

		// Grab the DSSP/WOLF residue name
		const residue_id &dssp_or_wolf_res_id      = arg_dssp_or_wolf_residue_ids[ dssp_residue_ctr ];
		const bool       &dssp_or_wolf_res_is_null = is_null( dssp_or_wolf_res_id );

		// If the PDB index has stepped past the end of the PDB residues
		if (pdb_residue_ctr >= num_pdb_residues) {

			// Then if this DSSP/WOLF residue is a NULL entry then just skip it
			if ( dssp_or_wolf_res_is_null ) {
				continue;
			}
			// Otherwise, this is a valid DSSP/WOLF residue with no match in the PDB so throw a wobbly
			else {
				BOOST_THROW_EXCEPTION(invalid_argument_exception("DSSP/WOLF residue " + to_string( dssp_or_wolf_res_id ) + " overshoots the end of the PDB residues"));
			}
		}

		// Record whether this is a permitted head break region
		const bool permitted_head_break = ( arg_permit_head_tail_break_without_null_residue && pdb_residue_ctr == 0 );

		// Create a lambda function to calculate whether the specified PDB residue counter should/can be advanced
		// to find a match with the specified DSSP/WOLF residue name target
		const auto should_advance_pdb_res_ctr_for_target_fn = [&] (const size_t     &arg_pdb_res_ctr,
		                                                           const residue_id &arg_target
		                                                           ) {
			const bool mismatches          = ( arg_pdb_residue_ids[ arg_pdb_res_ctr ] != arg_target );
			const bool reason_for_mismatch = (
				dssp_or_wolf_res_is_null
				||
				arg_permit_breaks_without_null_residues
				||
				permitted_head_break
				||
				contains( arg_skippable_pdb_indices, arg_pdb_res_ctr )
			);
			return ( mismatches && reason_for_mismatch );
		};

		// If should advance...
		if ( should_advance_pdb_res_ctr_for_target_fn( pdb_residue_ctr, dssp_or_wolf_res_id ) ) {

			// If is a null residue at the end of the DSSP/WOLF, then set pdb_residue_ctr to the end of the PDB
			if ( dssp_or_wolf_res_is_null && dssp_residue_ctr + 1 >= num_dssp_or_wolf_residues) {
				pdb_residue_ctr = num_pdb_residues;
			}

			// Otherwise, it's necessary to search for the PDB residue that matches:
			//  * the next DSSP/WOLF residue if this one is empty or
			//  * this mismatching DSSP/WOLF residue otherwise
			else {
				// Grab the string to search for
				const residue_id &dssp_or_wolf_res_id_to_find = dssp_or_wolf_res_is_null ? arg_dssp_or_wolf_residue_ids[ dssp_residue_ctr + 1 ]
				                                                                         : arg_dssp_or_wolf_residue_ids[ dssp_residue_ctr     ];

				// Scan through the PDB residues to find a match
				while ( pdb_residue_ctr < num_pdb_residues && should_advance_pdb_res_ctr_for_target_fn( pdb_residue_ctr, dssp_or_wolf_res_id_to_find ) ) {
					++pdb_residue_ctr;
				}

				// If no matching residue was found in the PDB then throw a wobbly
				if ( pdb_residue_ctr >= num_pdb_residues ) {
					BOOST_THROW_EXCEPTION(invalid_argument_exception("Cannot find a match for DSSP/WOLF residue " + to_string( dssp_or_wolf_res_id_to_find ) ));
				}
			}

			// If this DSSP/WOLF residue is an empty then there is nothing more to do so move to the next loop
			if ( dssp_or_wolf_res_is_null ) {
				continue;
			}
		}

		// If these two residue names don't match then throw a wobbly
		const residue_id &pdb_res_id = arg_pdb_residue_ids[ pdb_residue_ctr ];
		if ( pdb_res_id != dssp_or_wolf_res_id ) {
			BOOST_THROW_EXCEPTION(invalid_argument_exception(
				"Cannot match PDB residue "
				+ to_string( pdb_res_id )
				+ " with DSSP/WOLF residue "
				+ to_string( dssp_or_wolf_res_id )
				+ " - it may be worth double-checking the DSSP/WOLF file is generated from"
				  " this version of the PDB file and, if you're using DSSP files, it may be"
				  " worth ensuring you're using an up-to-date DSSP binary"
			));
		}

		// Add this pair of residues to the alignment
		alignment.push_back(make_pair(pdb_residue_ctr, dssp_residue_ctr));

		// Increment the pdb_residue_ctr
		++pdb_residue_ctr;
	}

	// If there are further residues remaining in the PDB and permit_tail_break_without_null_residue is off then throw a wobbly
	if ( ! arg_permit_head_tail_break_without_null_residue && pdb_residue_ctr < num_pdb_residues ) {
		BOOST_THROW_EXCEPTION(invalid_argument_exception("PDB contains residues at the end that are not present at the end of the DSSP/WOLF"));
	}

	// No problem has been spotted so return the constructed alignment
	return alignment;
}
