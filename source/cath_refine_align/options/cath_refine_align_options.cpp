/// \file
/// \brief The cath_refine_align_options class definitions

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

#include "cath_refine_align_options.hpp"

#include <boost/program_options.hpp>
#include <boost/shared_array.hpp>

#include "acquirer/alignment_acquirer/alignment_acquirer.hpp"
#include "acquirer/pdbs_acquirer/file_list_pdbs_acquirer.hpp"
#include "acquirer/pdbs_acquirer/istream_pdbs_acquirer.hpp"
#include "acquirer/selection_policy_acquirer/selection_policy_acquirer.hpp"
#include "alignment/alignment.hpp"
#include "alignment/common_atom_selection_policy/common_atom_select_ca_policy.hpp"
#include "alignment/common_residue_selection_policy/common_residue_select_all_policy.hpp"
#include "alignment/common_residue_selection_policy/common_residue_select_best_score_percent_policy.hpp"
#include "chopping/domain/domain.hpp"
#include "chopping/region/region.hpp"
#include "common/algorithm/transform_build.hpp"
#include "common/argc_argv_faker.hpp"
#include "common/type_aliases.hpp"
#include "exception/invalid_argument_exception.hpp"
#include "exception/not_implemented_exception.hpp"
#include "exception/runtime_error_exception.hpp"
#include "file/pdb/pdb.hpp"
#include "file/pdb/pdb_atom.hpp"
#include "file/pdb/pdb_residue.hpp"
#include "outputter/alignment_outputter/alignment_outputter.hpp"
#include "outputter/alignment_outputter/alignment_outputter_list.hpp"
#include "outputter/superposition_outputter/superposition_outputter.hpp"
#include "outputter/superposition_outputter/superposition_outputter_list.hpp"
#include "superposition/superposition_context.hpp"

using namespace boost::program_options;
using namespace cath;
using namespace cath::align;
using namespace cath::chop;
using namespace cath::common;
using namespace cath::file;
using namespace cath::opts;

using boost::lexical_cast;
using boost::none;
using boost::ptr_vector;
using std::istream;
using std::string;
using std::unique_ptr;

/// \brief The name of the program that uses this executable_options
const string cath_refine_align_options::PROGRAM_NAME("cath-refine-align");

/// \brief Get the name of the program that uses this executable_options
string cath_refine_align_options::do_get_program_name() const {
	return PROGRAM_NAME;
}

/// \brief Review all specified options and return a string containing any errors or a help string
///        (possibly using a description of all visible options)
///
/// This is a concrete definition of a virtual method that's pure in executable_options
///
/// This should only be called by executable_options, as the last step of the parse_options()
/// method, after all real parsing has completed.
///
/// \pre The options should have been parsed
///
/// \returns Any error/help string arising from the newly specified options
///          or an empty string if there aren't any
str_opt cath_refine_align_options::do_get_error_or_help_string() const {
	// Grab the objects from the options blocks
	const size_t num_aln_acquirers = get_num_acquirers( the_alignment_input_options_block );
	const size_t num_pdb_acquirers = get_num_acquirers( the_pdb_input_options_block );
	const auto   aln_outputters    = get_alignment_outputters();
	const auto   sup_outputters    = get_superposition_outputters();

	// If there are no objects then no options were specified so just output the standard usage error string
	if ( ( num_aln_acquirers == 0 ) && ( num_pdb_acquirers == 0 ) && sup_outputters.empty()) {
		return ""s;
	}

	// Check that there is exactly one source of alignment
	if ( num_aln_acquirers != 1 ) {
		return "Please specify one source of an alignment or superposition ("
		       + ::std::to_string( num_aln_acquirers )
		       + " specified)";
	}

	// Check that there is exactly one source of PDBs
	if ( num_pdb_acquirers != 1 ) {
		return "Please specify one source of PDBs ("
		       + ::std::to_string( num_pdb_acquirers )
		       + " specified)";
	}

	const variables_map &local_vm = get_variables_map();

	const bool using_display_spec =    any_alignment_outputters_involve_display_spec    ( aln_outputters )
	                                || any_superposition_outputters_involve_display_spec( sup_outputters );
	if ( specifies_options_from_block( local_vm, the_display_options_block ) && ! using_display_spec ) {
		return "Cannot specify display options because no display is being used in superposition/alignment outputters"s;
	}

	return none;
}

/// \brief Get a string to prepend to the standard help
string cath_refine_align_options::do_get_help_prefix_string() const {
	return "Usage: " + PROGRAM_NAME + " alignment_source protein_file_source [superposition_outputs]\n\n"
		+ get_overview_string() + R"(

Please specify:
 * one alignment
 * one method of reading proteins (number of proteins currently restricted to 2))";
}

/// \brief Get a string to append to the standard help (just empty here)
string cath_refine_align_options::do_get_help_suffix_string() const {
	return "";
}

/// \brief Get an overview of the job that these options are for
///
/// This can be used in the --help and --version outputs
string cath_refine_align_options::do_get_overview_string() const {
	return "Iteratively refine an existing alignment by attempting to optimise SSAP score";
}

/// TODOCUMENT
void cath_refine_align_options::check_ok_to_use() const {
	if ( get_error_or_help_string() ) {
		BOOST_THROW_EXCEPTION(invalid_argument_exception("Attempt to use invalid cath_refine_align_options"));
	}
	const bool alignment_outputs_to_stdout     = the_alignment_output_options_block.outputs_to_stdout();
	const bool superposition_outputs_to_stdout = the_superposition_output_options_block.outputs_to_stdout();
	if ( alignment_outputs_to_stdout && superposition_outputs_to_stdout ) {
		BOOST_THROW_EXCEPTION(invalid_argument_exception("Cannot output both alignment and superposition to stdout"));
	}
}

/// \brief TODOCUMENT
cath_refine_align_options::cath_refine_align_options() {
	super::add_options_block( the_alignment_input_options_block      );
	super::add_options_block( the_ids_ob                             );
	super::add_options_block( the_pdb_input_options_block            );
	super::add_options_block( the_align_regions_ob                   );
	super::add_options_block( the_alignment_output_options_block     );
	super::add_options_block( the_superposition_output_options_block );
	super::add_options_block( the_display_options_block              );
}


/// \brief TODOCUMENT
const str_vec & cath_refine_align_options::get_ids() const {
	return the_ids_ob.get_ids();
}

/// TODOCUMENT
const pdb_input_spec & cath_refine_align_options::get_pdb_input_spec() const {
	return the_pdb_input_options_block.get_pdb_input_spec();
}

/// Getter for the alignment_input_spec
const alignment_input_spec & cath_refine_align_options::get_alignment_input_spec() const {
	return the_alignment_input_options_block.get_alignment_input_spec();
}

/// \brief TODOCUMENT
alignment_outputter_list cath_refine_align_options::get_alignment_outputters() const {
	check_ok_to_use();
	const display_spec the_display_spec = the_display_options_block.get_display_spec();
	return the_alignment_output_options_block.get_alignment_outputters( the_display_spec );
}

/// \brief TODOCUMENT
superposition_outputter_list cath_refine_align_options::get_superposition_outputters() const {
	check_ok_to_use();
	return the_superposition_output_options_block.get_superposition_outputters(
		the_display_options_block.get_display_spec(),
		the_content_spec
	);
}

/// \brief Get the regions to which the PDBs in the superposition should be restricted
///
/// For now, this just generates a list of none entries, indicating that no PDB
/// should be restricted.
///
/// \todo Add superposition regions options that are stored in cath_refine_align_options
///       and have this method return the results
const domain_vec & cath_refine_align_options::get_domains() const {
	return the_align_regions_ob.get_align_domains();
}

/// \brief Get the single alignment_acquirer implied by the specified cath_refine_align_options
///        (or throw an invalid_argument_exception if fewer/more are implied)
///
/// \relates cath_refine_align_options
unique_ptr<const alignment_acquirer> cath::opts::get_alignment_acquirer(const cath_refine_align_options &arg_cath_refine_align_options ///< The cath_refine_align_options to query
                                                                        ) {
	return align::get_alignment_acquirer( arg_cath_refine_align_options.get_alignment_input_spec() );
}

/// \brief Get the single pdbs_acquirer implied by the specified cath_refine_align_options
///        (or throw an invalid_argument_exception if fewer/more are implied)
///
/// \relates cath_refine_align_options
unique_ptr<const pdbs_acquirer> cath::opts::get_pdbs_acquirer(const cath_refine_align_options &arg_cath_refine_align_options ///< The cath_refine_align_options to query
                                                              ) {
	return get_pdbs_acquirer( arg_cath_refine_align_options.get_pdb_input_spec() );
}

/// \brief Get the selection_policy_acquirer implied by the specified cath_refine_align_options
///
/// \relates cath_refine_align_options
selection_policy_acquirer cath::opts::get_selection_policy_acquirer(const cath_refine_align_options &arg_cath_refine_align_options ///< The cath_refine_align_options to query
                                                                    ) {
	return get_selection_policy_acquirer( arg_cath_refine_align_options.get_alignment_input_spec() );
}

/// \brief Get PDBs and names as implied by the specified cath_refine_align_options
///
/// throw an invalid_argument_exception if the cath_refine_align_options isn't configured to read PDBs
///
/// \relates cath_refine_align_options
strucs_context cath::opts::get_pdbs_and_names(const cath_refine_align_options &arg_cath_ref_opts,          ///< The options to specify how to get the PDBs and names
                                              istream                         &arg_istream,                ///< The istream, which may contain PDB data
                                              const bool                      &arg_remove_partial_residues ///< Whether to remove partial residues from the PDB data
                                              ) {
	return get_strucs_context(
		*get_pdbs_acquirer( arg_cath_ref_opts ),
		arg_istream,
		arg_remove_partial_residues,
		arg_cath_ref_opts.get_ids(),
		arg_cath_ref_opts.get_domains()
	);
}
