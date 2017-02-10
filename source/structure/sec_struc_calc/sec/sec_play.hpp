/// \file
/// \brief The sec_play header

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

#ifndef _CATH_TOOLS_SOURCE_STRUCTURE_SEC_STRUC_CALC_SEC_SEC_PLAY_H
#define _CATH_TOOLS_SOURCE_STRUCTURE_SEC_STRUC_CALC_SEC_SEC_PLAY_H

#include "common/type_aliases.hpp"
#include "structure/geometry/coord.hpp"

namespace cath { class protein; }
namespace cath { class residue; }
namespace cath { enum class sec_struc_type : char; }
namespace cath { namespace file { class sec_file_record; } }

namespace cath {
	namespace sec {

		geom::coord round_like_sec_file_copy(const geom::coord &);

		file::sec_file_record round_like_sec_file_copy(const file::sec_file_record &);

		file::sec_file_record calculate_sec_record(const protein &,
		                                           const size_t  &,
		                                           const size_t  &);

		/// \brief The types of secondary structure processed by Prosec
		enum class prosec_sec_type {
			ALPHA_HELIX, ///< Alpha-helix (as used in Prosec)
			BETA_STRAND  ///< Beta-strand (as used in Prosec)
			// THREE_TEN_HELIX ///< 3-10 helix  (as used in Prosec)
		};

		prosec_sec_type make_prosec_sec_type(const sec_struc_type &);

		/// \brief The weight of the middle residue's location for the specified type of secondary structure
		///
		/// This is part of calculating prosec_axis_point_of_residue_triple()
		inline constexpr double ends_midpoint_weight_of_sec_type(const prosec_sec_type &arg_sec_struc ///< The type of secondary structure
		                                                         ) {
			return ( arg_sec_struc == prosec_sec_type::BETA_STRAND      ) ? 1.0 * 0.521590916564475382 :
			       ( arg_sec_struc == prosec_sec_type::ALPHA_HELIX      ) ? 1.0 * 0.852044095520923672 :
			                       /* prosec_sec_type::THREE_TEN_HELIX */         0.666666666666666666;
		}

		/// \brief The weight of the midpoint-of-straddling residues' locations for the specified type of secondary structure
		///
		/// This is part of calculating prosec_axis_point_of_residue_triple()
		inline constexpr double central_weight_of_sec_type(const prosec_sec_type &arg_sec_struc ///< The type of secondary structure
		                                                   ) {
			return 1.0 - ends_midpoint_weight_of_sec_type( arg_sec_struc );
		}

		/// \brief Get the prosec axis point of the middle of the three specified locations of consecutive carbon-alphas
		///        that are part of the specified type of secondary structure
		///
		/// The result is somewhere between the cetral location and the midpoint of the two straddling locations.
		/// The fraction along hat line is determined by the ends_midpoint_weight_of_sec_type() for the secondary
		/// structure type
		inline constexpr geom::coord prosec_axis_point_of_residue_triple(const geom::coord     &arg_previous_ca, ///< The location of the carbon-alpha of the residue before the residue in question
		                                                                 const geom::coord     &arg_this_ca,     ///< The location of the carbon-alpha of the residue                    in question
		                                                                 const geom::coord     &arg_next_ca,     ///< The location of the carbon-alpha of the residue after  the residue in question
		                                                                 const prosec_sec_type &arg_sec_struc    ///< The type of secondary structure of these three residues
		                                                                 ) {
			return (
				(
					ends_midpoint_weight_of_sec_type( arg_sec_struc )
					*
					0.5 * ( arg_previous_ca + arg_next_ca )
				)
				+
				(
					central_weight_of_sec_type      ( arg_sec_struc )
					*
					arg_this_ca
				)
			);
		}

		geom::coord prosec_axis_point_of_residue_triple(const residue &,
		                                                const residue &,
		                                                const residue &);

		geom::coord prosec_axis_point_of_residue_triple(const protein &,
		                                                const size_t &);

		size_size_pair_vec get_sec_starts_and_stops(const protein &);

	} // namespace sec
} // namespace cath

#endif
