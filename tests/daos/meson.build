test_sources = files('test_daos_version.cpp')

daos_test = executable('daos_test', test_sources,
                            include_directories: project_include_dir,
                            dependencies: [boost_dep, daos_dep])


test('DAOSNativeTests', daos_test, suite: ['daos', 'unit'])
