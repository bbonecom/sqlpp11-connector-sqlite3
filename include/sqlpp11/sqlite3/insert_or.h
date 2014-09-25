/*
 * Copyright (c) 2013-2014, Roland Bock
 * All rights reserved.
 * 
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 * 
 *   Redistributions of source code must retain the above copyright notice, this
 *   list of conditions and the following disclaimer.
 * 
 *   Redistributions in binary form must reproduce the above copyright notice, this
 *   list of conditions and the following disclaimer in the documentation and/or
 *   other materials provided with the distribution.
 * 
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
 * ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef SQLPP_SQLITE3_INSERT_OR_H
#define SQLPP_SQLITE3_INSERT_OR_H

#include <sqlpp11/statement.h>
#include <sqlpp11/type_traits.h>
#include <sqlpp11/parameter_list.h>
#include <sqlpp11/prepared_insert.h>
#include <sqlpp11/default_value.h>
#include <sqlpp11/noop.h>
#include <sqlpp11/into.h>
#include <sqlpp11/insert_value_list.h>

namespace sqlpp
{
	namespace sqlite3
	{
		struct insert_or_replace_name_t {};

		struct insert_or_replace_t: public statement_name_t<insert_or_replace_name_t>
		{
			using _traits = make_traits<no_value_t, tag::is_return_value>;
			struct _name_t {};

			template<typename Policies>
				struct _result_methods_t
				{
					using _statement_t = derived_statement_t<Policies>;

					const _statement_t& _get_statement() const
					{
						return static_cast<const _statement_t&>(*this);
					}

					template<typename Db>
						auto _run(Db& db) const -> decltype(db.insert(this->_get_statement()))
						{
							_statement_t::_check_consistency();

							static_assert(_statement_t::_get_static_no_of_parameters() == 0, "cannot run insert or replace directly with parameters, use prepare instead");
							return db.insert(_get_statement());
						}

					template<typename Db>
						auto _prepare(Db& db) const
						-> prepared_insert_t<Db, _statement_t>
						{
							_statement_t::_check_consistency();

							return {{}, db.prepare_insert(_get_statement())};
						}
				};
		};

		template<typename Database>
			using blank_insert_or_replace_t = statement_t<Database,
						insert_or_replace_t,
						no_into_t, 
						no_insert_value_list_t>;

		inline auto insert()
			-> blank_insert_or_replace_t<void>
			{
				return { blank_insert_or_replace_t<void>() };
			}

		template<typename Table>
			constexpr auto insert_or_replace(Table table)
			-> decltype(blank_insert_or_replace_t<void>().into(table))
			{
				return { blank_insert_or_replace_t<void>().into(table) };
			}

		template<typename Database>
			constexpr auto  dynamic_insert_or_replace(const Database&)
			-> decltype(blank_insert_or_replace_t<Database>())
			{
				return { blank_insert_or_replace_t<Database>() };
			}

		template<typename Database, typename Table>
			constexpr auto  dynamic_insert_or_replace(const Database&, Table table)
			-> decltype(blank_insert_or_replace_t<Database>().into(table))
			{
				return { blank_insert_or_replace_t<Database>().into(table) };
			}
	}

	template<typename Context>
		struct serializer_t<Context, sqlite3::insert_or_replace_name_t>
		{
			using T = sqlite3::insert_or_replace_name_t;

			static Context& _(const T& t, Context& context)
			{
				context << "INSERT OR REPLACE ";

				return context;
			}
		};
}

#endif