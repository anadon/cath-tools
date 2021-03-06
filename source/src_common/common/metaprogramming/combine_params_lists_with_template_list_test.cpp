/// \file
/// \brief The combine_params_lists_with_template_list test suite

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

#include "combine_params_lists_with_template_list.hpp"

#include <boost/test/unit_test.hpp>

#include "common/metaprogramming/template_list.hpp"

#include <deque>
#include <tuple>
#include <type_traits>
#include <vector>

using namespace cath::common;

using std::deque;
using std::is_same;
using std::tuple;
using std::vector;

BOOST_AUTO_TEST_SUITE(combine_params_lists_with_template_list_test_suite)

BOOST_AUTO_TEST_CASE(basic) {
	static_assert(
		is_same<
			combine_params_lists_with_template_list_t<
				tuple< tuple<int>, tuple<double> >,
				template_list<vector, deque>
			>,
			tuple< vector<int>, vector<double>, deque<int>, deque<double> >
		>::value,
		"combine_params_lists_with_template_list_t should combine typelists with a template_list"
	);
	BOOST_CHECK( true );
}

BOOST_AUTO_TEST_SUITE_END()
