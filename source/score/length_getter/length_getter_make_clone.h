/// \file
/// \brief The make_clone header

/// \copyright
/// Tony Lewis's Common C++ Library Code (here imported into the CATH Tools project and then tweaked, eg namespaced in cath)
/// Copyright (C) 2007, Tony Lewis
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

#ifndef LENGTH_GETTER_MAKE_CLONE_H_INCLUDED
#define LENGTH_GETTER_MAKE_CLONE_H_INCLUDED

#include "common/clone/detail/make_clone.h"

namespace cath { namespace score { class protein_only_length_getter; } }
namespace cath { namespace score { class sym_protein_only_length_getter; } }

namespace cath {
	namespace common {
		namespace detail {
			
			template <>
			std::unique_ptr<score::protein_only_length_getter> make_clone(const score::protein_only_length_getter &);

			template <>
			std::unique_ptr<score::sym_protein_only_length_getter> make_clone(const score::sym_protein_only_length_getter &);
		}
	}
}

#endif
