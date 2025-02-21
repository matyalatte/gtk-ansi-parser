project('gtk-ansi-parser', 'c',
    meson_version: '>=0.54.0',
    default_options: [
        'warning_level=3',              # always max warnings
        'c_std=c99',                    # strict C99
        'cpp_std=c++11',                # strict C++11
    ],
    version: '0.1.0')

# set source files
gtkansi_sources = ['src/gtk_ansi.c']

gtk_deps = [
	dependency('gtk+-3.0',
		version: '>=3.10.0',
		method: 'pkg-config',
		required: true),
	# We specify these as not required because some Unix systems include them with libc instead of providing them as separate files
    # (thanks textshell and jpakkane in freenode #mesonbuild)
	meson.get_compiler('c').find_library('m',
		required: false),
	meson.get_compiler('c').find_library('dl',
		required: false),
]

if meson.version().version_compare('>=1.3.0')
    gtkansi_lib = library('gtk_ansi_parser',
        gtkansi_sources,
        dependencies: gtk_deps,
        c_static_args: ['-D_GTKANSI_STATIC'],
        install: true,
        include_directories: include_directories('./include'),
        gnu_symbol_visibility: 'hidden')
else
    # TODO: Remove this else block to support only meson 1.3.0 or later.
    gtkansi_c_args = []
    if get_option('default_library') == 'both'
        error('gtk-ansi-parser requires meson 1.3.0 or later to build both shared and static libraries at the same time')
    elif get_option('default_library') == 'static'
        gtkansi_c_args = ['-D_GTKANSI_STATIC']
    endif
    gtkansi_lib = library('gtk_ansi_parser',
        gtkansi_sources,
        dependencies: gtk_deps,
        c_args: gtkansi_c_args,
        install: true,
        include_directories: include_directories('./include'),
        gnu_symbol_visibility: 'hidden')
endif

# dependency for other projects
gtkansi_dep = declare_dependency(
    dependencies: gtk_deps,
    include_directories: include_directories('./include'),
    link_with: gtkansi_lib)

# Build demo
if get_option('demo')
    executable('gtkansi_demo',
        gtkansi_sources + ['src/demo.c'],
        dependencies: gtkansi_dep,
        install : false)
endif

# Build unit tests
if get_option('tests')
    add_languages('cpp', native:false, required: true)

    # get gtest
    gtest_proj = subproject('gtest')
    gtest_dep = gtest_proj.get_variable('gtest_dep')
    gmock_dep = gtest_proj.get_variable('gmock_dep')

    # build tests
    subdir('tests')
endif
