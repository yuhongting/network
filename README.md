I'm from China and I'm trying to write the file without Chinese.

The project is a cross-platform simple server framework based on boost.asio and redis. Actually it's just a network and redis framework.
If you use it,you may need logger system,memorycycle system,a sql database,serialization tools and your own protocol.

Install boost,then you can compile with msvc140 easily.I am adding CMakeLists for it.So cmake will be surported on next commit.