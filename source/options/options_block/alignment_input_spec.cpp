/// \file
/// \brief The alignment_input_spec class definitions

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

#include "alignment_input_spec.hpp"

#include <boost/range/algorithm/count.hpp>

#include "common/cpp20/make_array.hpp"
#include "common/size_t_literal.hpp"

#include <array>

using namespace cath;
using namespace cath::common;
using namespace cath::opts;

using boost::filesystem::path;
using boost::range::count;
using std::array;

constexpr bool alignment_input_spec::DEFAULT_RESIDUE_NAME_ALIGN;

/// \brief Getter for whether to align based on matching residue names
const bool & alignment_input_spec::get_residue_name_align() const {
	return residue_name_align;
}

/// \brief Getter for a file from which to read a FASTA alignment
const path & alignment_input_spec::get_fasta_alignment_file() const {
	return fasta_alignment_file;
}

/// \brief Getter for a file from which to read a legacy-SSAP-format alignment
const path & alignment_input_spec::get_ssap_alignment_file() const {
	return ssap_alignment_file;
}

/// \brief Getter for a file from which to read a CORA alignment
const path & alignment_input_spec::get_cora_alignment_file() const {
	return cora_alignment_file;
}

/// \brief Getter for a file from which to read SSAP-scores format data to use to attempt to glue pairwise alignments together
const path & alignment_input_spec::get_ssap_scores_file() const {
	return ssap_scores_file;
}

/// \brief Getter for a directory in which SSAPs should be performed and then their alignments glued together
///        or (inner) none for cath-tools to choose a directory to use
const path_opt_opt & alignment_input_spec::get_do_the_ssaps_dir() const {
	return do_the_ssaps_dir;
}

/// \brief Setter for whether to align based on matching residue names
alignment_input_spec & alignment_input_spec::set_residue_name_align(const bool &arg_residue_name_align ///< Whether to align based on matching residue names
                                                                    ) {
	residue_name_align = arg_residue_name_align;
	return *this;
}

/// \brief Setter for a file from which to read a FASTA alignment
alignment_input_spec & alignment_input_spec::set_fasta_alignment_file(const path &arg_fasta_alignment_file ///< A file from which to read a FASTA alignment
                                                                      ) {
	fasta_alignment_file = arg_fasta_alignment_file;
	return *this;
}

/// \brief Setter for a file from which to read a legacy-SSAP-format alignment
alignment_input_spec & alignment_input_spec::set_ssap_alignment_file(const path &arg_ssap_alignment_file ///< A file from which to read a legacy-SSAP-format alignment
                                                                     ) {
	ssap_alignment_file = arg_ssap_alignment_file;
	return *this;
}

/// \brief Setter for a file from which to read a CORA alignment
alignment_input_spec & alignment_input_spec::set_cora_alignment_file(const path &arg_cora_alignment_file ///< A file from which to read a CORA alignment
                                                                     ) {
	cora_alignment_file = arg_cora_alignment_file;
	return *this;
}

/// \brief Setter for a file from which to read SSAP-scores format data to use to attempt to glue pairwise alignments together
alignment_input_spec & alignment_input_spec::set_ssap_scores_file(const path &arg_ssap_scores_file ///< A file from which to read SSAP-scores format data to use to attempt to glue pairwise alignments together
                                                                  ) {
	ssap_scores_file = arg_ssap_scores_file;
	return *this;
}

/// \brief Setter for a directory in which SSAPs should be performed and then their alignments glued together
///        or (inner) none for cath-tools to choose a directory to use
alignment_input_spec & alignment_input_spec::set_do_the_ssaps_dir(const path_opt &arg_do_the_ssaps_dir ///< A directory in which SSAPs should be performed and then their alignments glued together or (inner) none for cath-tools to choose a directory to use
                                                                  ) {
	do_the_ssaps_dir = arg_do_the_ssaps_dir;
	return *this;
}

/// \brief Get the number of alignment_acquirer objects that would be created by get_alignment_acquirers() on the specified alignment_input_spec
///
/// \relates alignment_input_spec
///
/// \alsorelates alignment_acquirer
size_t cath::opts::get_num_acquirers(const alignment_input_spec &arg_alignment_input_spec ///< The alignment_input_spec to query
                                     ) {
	return static_cast<size_t>( count(
		make_array(
			! arg_alignment_input_spec.get_cora_alignment_file().empty(),
			  arg_alignment_input_spec.get_residue_name_align(),
			! arg_alignment_input_spec.get_fasta_alignment_file().empty(),
			! arg_alignment_input_spec.get_ssap_alignment_file().empty(),
			! arg_alignment_input_spec.get_ssap_scores_file().empty(),
			  static_cast<bool>( arg_alignment_input_spec.get_do_the_ssaps_dir() )
		),
		true
	) );
}
