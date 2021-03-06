/// \file
/// \brief The chopping class definitions

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

#include "chopping.hpp"

#include <boost/range.hpp>

#include "chopping/domain/domain.hpp"
#include "chopping/region/region.hpp"
#include "common/cpp14/cbegin_cend.hpp"

#include <utility>

using namespace cath;
using namespace cath::chop;

/// \brief TODOCUMENT
void chopping::sanity_check() const {

}

/// \brief Ctor for chopping
chopping::chopping(domain_vec arg_domains,  ///< TODOCUMENT
                   region_vec arg_fragments ///< TODOCUMENT
                   ) : domains  { std::move( arg_domains   ) },
                       fragments{ std::move( arg_fragments ) } {
}

/// \brief TODOCUMENT
size_t chopping::num_domains() const {
	return domains.size();
}

/// \brief TODOCUMENT
size_t chopping::num_fragments() const {
	return fragments.size();
}

/// \brief TODOCUMENT
const region & chopping::get_fragment_of_index(const size_t &arg_index ///< TODOCUMENT
                                               ) const {
	return fragments[ arg_index ];
}

/// \brief TODOCUMENT
chopping::const_iterator chopping::begin() const {
	return common::cbegin( domains );
}

/// \brief TODOCUMENT
chopping::const_iterator chopping::end() const {
	return common::cend( domains );
}
