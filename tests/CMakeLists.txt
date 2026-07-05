project(MolecularTests)

add_executable(MolecularTests)

target_link_libraries(MolecularTests PRIVATE Molecular)

target_sources(MolecularTests PRIVATE main.cpp sanity_tests.cpp assets_path_tests.cpp)

add_test(NAME MolecularTests COMMAND MolecularTests)
