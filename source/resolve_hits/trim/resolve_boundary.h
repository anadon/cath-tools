/// \file
/// \brief The resolve_boundary class header

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

#ifndef _CATH_TOOLS_SOURCE_RESOLVE_HITS_TRIM_RESOLVE_BOUNDARY_H
#define _CATH_TOOLS_SOURCE_RESOLVE_HITS_TRIM_RESOLVE_BOUNDARY_H

#include "common/debug_numeric_cast.h"
#include "resolve_hits/res_arrow.h"
#include "resolve_hits/resolve_hits_type_aliases.h"

namespace cath {
	namespace rslv {

		namespace detail {

			/// \brief Return whether or not the specified value ends in .5
			inline constexpr bool ends_with_half(const double &arg_value ///< The value to test
			                                     ) {
				return ( arg_value - static_cast<double>( static_cast<residx_t>( arg_value ) ) ) == 0.5;
			}

			/// \brief Implement the smart rounding for resolve_boundary, returning a rounded version
			///        of the proposed extension that should be added to the arg_hard_lhs_posn
			///
			/// If the result is greater than arg_lhs_trim, then the ends don't meet so throw 
			/// If the value doesn't end in .5, then just perform normal rounding
			/// Otherwise, add the spare residue to the left if that has a strictly shorter trim
			/// Otherwise add the spare residue to the right
			///
			/// Come a constexpr std::round, use that here rather than `static_cast<>( 0.5 + ... )`
			inline constexpr residx_t round_boundary_value(const double   &arg_value,    ///< The value to examine
			                                               const residx_t &arg_lhs_trim, ///< The trim that is present on the left side
			                                               const residx_t &arg_rhs_trim  ///< The trim that is present on the right side
			                                               ) {
				return ( arg_value > arg_lhs_trim      ) ? throw std::invalid_argument("Cannot resolve_boundary() for non-meeting ends") :
				       ( ! ends_with_half( arg_value ) ) ? static_cast<residx_t>( 0.5 + arg_value ) :
				       ( arg_lhs_trim > arg_rhs_trim   ) ? static_cast<residx_t>(       arg_value ) :
				                                           static_cast<residx_t>( 1.0 + arg_value );
			}
		}

		/// \brief Calculated a resolved boundary for two segments with overlapping trimmed regions
		///
		/// \pre `arg_hard_lhs_posn <= arg_hard_lhs_posn` else throws an invalid_argument
		inline constexpr res_arrow resolve_boundary(const res_arrow &arg_hard_lhs_posn, ///< The hard (ie trimmed) stop of the first segment
		                                            const residx_t  &arg_lhs_trim,      ///< The length of the trim at the end of the first segment
		                                            const res_arrow &arg_hard_rhs_posn, ///< The hard (ie trimmed) start of the second segment
		                                            const residx_t  &arg_rhs_trim       ///< The length of the trim at the start of the second segment
		                                            ) {
			return ( arg_hard_rhs_posn < arg_hard_lhs_posn ) ? throw std::invalid_argument("Cannot resolve_boundary for mis-ordered data")
				: arg_hard_lhs_posn + detail::round_boundary_value(
					(
						static_cast<double>( arg_lhs_trim                            )
						*
						static_cast<double>( arg_hard_rhs_posn - arg_hard_lhs_posn )
						/
						static_cast<double>( arg_lhs_trim       + arg_rhs_trim     )
					),
					arg_lhs_trim,
					arg_rhs_trim
				);
		}

	} // namespace rslv
} // namespace cath

#endif
