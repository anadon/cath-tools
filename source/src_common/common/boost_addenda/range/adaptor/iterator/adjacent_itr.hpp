/// \file
/// \brief The adjacent_itr class header

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

#ifndef _CATH_TOOLS_SOURCE_SRC_COMMON_COMMON_BOOST_ADDENDA_RANGE_ADAPTOR_ITERATOR_ADJACENT_ITR_HPP
#define _CATH_TOOLS_SOURCE_SRC_COMMON_COMMON_BOOST_ADDENDA_RANGE_ADAPTOR_ITERATOR_ADJACENT_ITR_HPP

#include <boost/iterator/iterator_adaptor.hpp>
#include <boost/range/category.hpp>

#include "common/boost_addenda/range/range_concept_type_aliases.hpp"

#include <iterator>

namespace cath { namespace common { template <class RNG> class adjacent_itr; } }

namespace cath {
	namespace common {
		namespace detail {

			/// \brief Convenience type alias for a pair of references to the elements in a range of type RNG
			template <typename RNG>
			using range_adjacent_ref_pair = std::pair<range_reference_t<RNG>,
			                                          range_reference_t<RNG>>;

			/// \brief Convenience type alias for the iterator_adaptor through which adjacent_itr is implemented
			template <typename RNG>
			using adjacent_itr_impl = boost::iterator_adaptor<adjacent_itr<RNG>,            // Derived
			                                                  range_iterator_t<RNG>,        // Base
			                                                  range_adjacent_ref_pair<RNG>, // Value
			                                                  range_category_t<RNG>,        // CategoryOrTraversal
			                                                  range_adjacent_ref_pair<RNG>  // Reference
			                                                  >;
		} // namespace detail

		/// \brief Iterator for wrapping a range and dereferencing to pairs of adjacent members
		///        (to a pair of references to: the current member and the member after that)
		///
		/// For a const_iterator, use a const RNG type.
		///
		/// Invariants:
		///  * After method calls, the base() iterator should always point to some member of the original range *except* the last member
		///    or the one-past-end position. If it points to the last member, the dereferencing will cause undefined behaviour
		///  * The client must preserve the validity of the original range and its one-past-end iterator throughout the adjacent_itr's lifetime
		template <class RNG>
		class adjacent_itr final : public detail::adjacent_itr_impl<RNG> {
		private:
			friend class boost::iterator_core_access;

			/// \brief TODOCUMENT
			using super              = detail::adjacent_itr_impl<RNG>;

			/// \brief TODOCUMENT
			using base_iterator_type = range_iterator_t<RNG>;

			/// \brief TODOCUMENT
			using adjacent_pair_type = detail::range_adjacent_ref_pair<RNG>;

		public:
			/// \brief TODOCUMENT
			using difference_type = typename super::difference_type;

			/// \brief TODOCUMENT
			using reference_type  = range_reference_t<RNG>;

		private:
			/// \brief The one-
			base_iterator_type end_itr;

			bool is_at_end() const;
			void advance_to_end_if_one_off();

			adjacent_pair_type dereference() const;
			void advance(const difference_type &);
			void decrement();
			void increment();

			template <class OTHER_RNG>
			difference_type distance_to(const adjacent_itr<OTHER_RNG> &) const;

		public:
			adjacent_itr(const base_iterator_type &,
			             const base_iterator_type &);
			explicit adjacent_itr(RNG &);
			adjacent_itr() = default;
		};

		template <class RNG>
		bool adjacent_itr<RNG>::is_at_end() const {
			return ( this->base() == end_itr );
		}

		/// A simplified version of std::next to workaround an apparent bug in libc++
		/// that's fixed in recent versions but was present around versions 3.6 / 3.7.
		///
		/// The basic problem can be reproduced with:
		///
		///     #include <boost/range/adaptor/filtered.hpp>
		///     #include <boost/range/irange.hpp>
		///     
		///     #include <iterator>
		///     
		///     int main() {
		///       auto bob = boost::irange( 0, 10 )
		///         | boost::adaptors::filtered( [] (const int &x) { return ( x % 2 == 0 ); } );
		///       auto next_itr = std::next( std::cbegin( bob ) );
		///     }
		///
		/// \todo Come libc++ >= 3.8, drop this fn and replace all calls with std::next()
		template <typename T>
		inline T pseudo_std_next_to_workaround_libcpp_bug(T arg) {
			++arg;
			return arg;
		}

		/// \brief Private method for advancing this iterator to the end if it's currently one off
		///        (because the
		template <class RNG>
		void adjacent_itr<RNG>::advance_to_end_if_one_off() {
			if ( ! is_at_end() ) {
				if ( pseudo_std_next_to_workaround_libcpp_bug( this->base() ) == end_itr ) {
					++( *this );
				}
			}
		}

		/// \brief TODOCUMENT
		template <class RNG>
		typename adjacent_itr<RNG>::adjacent_pair_type adjacent_itr<RNG>::dereference() const {
			using return_type = typename adjacent_itr<RNG>::adjacent_pair_type;
			return return_type(
				*                                           this->base(),
				* pseudo_std_next_to_workaround_libcpp_bug( this->base() )
			);
		}

		/// \brief TODOCUMENT
		template <class RNG>
		void adjacent_itr<RNG>::advance(const difference_type &arg_offset ///< TODOCUMENT
		                                ) {
			this->super::advance( arg_offset );
			advance_to_end_if_one_off();
		}

		/// \brief TODOCUMENT
		template <class RNG>
		void adjacent_itr<RNG>::decrement() {
			if ( this->base() == end_itr ) {
				--( this->base_reference() );
			}
			--( this->base_reference() );
		}

		/// \brief TODOCUMENT
		template <class RNG>
		void adjacent_itr<RNG>::increment() {
			++( this->base_reference() );
			advance_to_end_if_one_off();
		}

		/// \brief TODOCUMENT
		template <class RNG>
		template <class OTHER_RNG>
		typename adjacent_itr<RNG>::difference_type adjacent_itr<RNG>::distance_to(const adjacent_itr<OTHER_RNG> &arg_other_itr ///< TODOCUMENT
		                                                                           ) const {
			if (   is_at_end() && ! arg_other_itr.is_at_end() ) {
				return ( arg_other_itr.base() - this->base() ) + 1;
			}
			if ( ! is_at_end() &&   arg_other_itr.is_at_end() ) {
				return ( arg_other_itr.base() - this->base() ) - 1;
			}
			return ( arg_other_itr.base() - this->base() );
		}

		/// \brief Ctor from iterators
		template <class RNG>
		adjacent_itr<RNG>::adjacent_itr(const base_iterator_type &arg_begin, ///< Begin iterator for the original range
		                                const base_iterator_type &arg_end    ///< End   iterator for the original range
		                                ) : super  ( arg_begin ),
		                                    end_itr( arg_end   ) {
			advance_to_end_if_one_off();
		}

		/// \brief Ctor from a range
		template <class RNG>
		adjacent_itr<RNG>::adjacent_itr(RNG &arg_range ///< The range over which this adjacent_itr should act
		                                ) : adjacent_itr(
		                                    	std::begin( arg_range ),
		                                    	std::end  ( arg_range )
		                                    ) {
			advance_to_end_if_one_off();
		}

	} // namespace common
} // namespace cath

#endif
