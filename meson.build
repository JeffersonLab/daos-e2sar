
# To set the project version, make sure `e2sar.pc` is included in $PKG_CONFIG_PATH
project('e2sar-daos', 'cpp',
    version: '0.1.0--e2sar_' + run_command('pkg-config', '--modversion', 'e2sar',
                            check: true).stdout().strip(),
    default_options: ['cpp_std=c++17'])

# Get the C++ compiler object
cpp_compiler = meson.get_compiler('cpp')

# Find e2sar
e2sar_dep = dependency('e2sar', required: true)
e2sar_version = e2sar_dep.version()

# Manually include DAOS since there is no *.pc for DAOS
# Currently DAOS is only for RHEL systems.
daos_include_dir = include_directories('/usr/include')

daos_lib = cpp_compiler.find_library('daos', required: false, dirs: '/usr/lib64')
dfs_lib = cpp_compiler.find_library('dfs', required: false, dirs: '/usr/lib64')

if daos_lib.found() and dfs_lib.found()
    daos_dep = declare_dependency(
        include_directories: [daos_include_dir],
        dependencies: [daos_lib, dfs_lib]
    )
    message('  - DAOS manually linked.')
else
    error('DAOS libraries or headers not found. Install daos-devel package first.')
endif

# # # Find Boost (required for unit testing)
# boost_dep = dependency('boost', modules: ['unit_test_framework'], required: true)

# Print Versions
message('Building e2sar-daos with:')
message('  - e2sar version: ' + e2sar_version)
# message('  - DAOS version: ' + daos_version)
# message('  - Boost version: ' + boost_dep.version())

# Sources
project_include_dir = include_directories('include')

project_sources = [
    'src/DAOSConnectorImpl.cc'
]

# Build Shared Library
# NOTE: update to "install: true" when it's ready
e2sar_daos = library('e2sar-daos',
    project_sources,
    include_directories: project_include_dir,
    dependencies: [e2sar_dep, daos_dep],
    install: false
)

# # Install Headers
# install_headers('include/all_headers.h')

# # Include tests
# subdir('tests')
