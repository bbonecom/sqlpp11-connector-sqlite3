add_library(sqlpp11-connector-sqlite3
		connection.cpp
		bind_result.cpp
		prepared_statement.cpp
		detail/connection_handle.cpp
		sqlite3.c)

install(TARGETS sqlpp11-connector-sqlite3 DESTINATION lib)
