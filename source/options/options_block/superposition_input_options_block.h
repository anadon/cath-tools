/// \file
/// \brief The superposition_input_options_block class header

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

#ifndef _CATH_TOOLS_SOURCE_OPTIONS_OPTIONS_BLOCK_SUPERPOSITION_INPUT_OPTIONS_BLOCK_H
#define _CATH_TOOLS_SOURCE_OPTIONS_OPTIONS_BLOCK_SUPERPOSITION_INPUT_OPTIONS_BLOCK_H

#include <boost/optional.hpp>

#include "common/path_type_aliases.h"
#include "options/options_block/options_block.h"

namespace cath {
	namespace opts {

		/// \brief Handle options for reading in a superposition from a JSON file
		class superposition_input_options_block final : public options_block {
		private:
			using super = options_block;

			static const std::string PO_JSON_SUP_INFILE;

			/// \brief The optional JSON superposition input file
			path_opt json_sup_infile;

			virtual std::unique_ptr<options_block> do_clone() const override final;
			virtual std::string do_get_block_name() const override final;
			virtual void do_add_visible_options_to_description(boost::program_options::options_description &) override final;
			virtual str_opt do_invalid_string(const boost::program_options::variables_map &) const override final;

		public:
			const path_opt & get_json_sup_infile() const;
		};

	} // namespace opts
} // namespace cath

#endif