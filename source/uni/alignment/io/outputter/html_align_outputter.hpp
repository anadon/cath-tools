/// \file
/// \brief The html_align_outputter class header

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

#ifndef _CATH_TOOLS_SOURCE_UNI_ALIGNMENT_IO_OUTPUTTER_HTML_ALIGN_OUTPUTTER_H
#define _CATH_TOOLS_SOURCE_UNI_ALIGNMENT_IO_OUTPUTTER_HTML_ALIGN_OUTPUTTER_H

#include "chopping/chopping_type_aliases.hpp"
#include "common/type_aliases.hpp"
#include "file/strucs_context.hpp"

#include <iosfwd>

namespace cath { class display_colourer; }
namespace cath { namespace align { class alignment; } }
namespace cath { namespace file { class pdb_list; } }
namespace cath { namespace align { class alignment_context; } }

namespace cath {
	namespace align {

		/// \brief Simple wrapper class for outputting an alignment to an ostream in horizontal format
		///
		/// Use like this:
		///
		///    cerr << html_align_outputter( my_alignment, pdbs, names, colourer ) << endl;
		///
		/// This provides a convenient way for the user to choose a different format
		/// when outputting an alignment to an ostream via the insertion operator
		class html_align_outputter final {
		private:
			/// \brief A const-reference to the alignment to be output
			std::reference_wrapper<const alignment            > the_alignment;

			/// \brief TODOCUMENT
			std::reference_wrapper<const file::strucs_context > context;

			/// \brief TODOCUMENT
			std::reference_wrapper<const display_colourer     > colourer;

		public:
			html_align_outputter(const alignment &,
			                     const file::strucs_context &,
			                     const display_colourer &);

			const alignment & get_alignment() const;
			const file::strucs_context & get_strucs_context() const;
			const display_colourer & get_display_colourer() const;
		};

		const file::pdb_list & get_pdbs(const html_align_outputter &);
		const file::name_set_list & get_name_sets(const html_align_outputter &);
		const chop::region_vec_opt_vec & get_regions(const html_align_outputter &);

		html_align_outputter make_html_align_outputter(const alignment_context &,
		                                               const display_colourer &);

		std::ostream & operator<<(std::ostream &,
		                          const html_align_outputter &);

	} // namespace align
} // namespace cath

#endif
