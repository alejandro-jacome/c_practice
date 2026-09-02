project('hash_map_project', 'c',
    version : '0.1.0',
    default_options : [
        'c_std=c23',
        'warning_level=3',     # Enables -Wall, -Wextra, -Wpedantic
        'werror=true',          # Enables -Werror
        'debug=true',           # Enables -g
        'optimization=g',       # Enables -Og (optimize for debugging)
        # 'b_sanitize=address,undefined',  # Enables ASan and UBSan
        'b_lundef=false' 
    ]
)

# ---------- ADDITIONAL HARDENING FLAGS ----------
# These don't have built-in Meson options, so we add them manually.
add_project_arguments(
    '-fno-omit-frame-pointer',    # Better stack traces
    '-Wfatal-errors',             # Stop on first error
    '-Wshadow',                   # Warn if a variable shadows another
    '-Wformat=2',                 # Strict format string checking
    '-Wconversion',               # Warn on implicit type conversions that lose value
    '-Wsign-conversion',          # Warn on signed/unsigned conversions
    '-Wnull-dereference',         # Warn if a pointer might be dereferenced as NULL
    '-Wdouble-promotion',         # Warn if float is implicitly promoted to double
    '-Wstrict-prototypes',        # Warn on old-style function definitions
    '-Wmissing-prototypes',       # Warn if a function is used before declaration
    '-Wmissing-declarations',     # Warn on global functions without a declaration
    '-fstack-protector-strong',   # Protect against stack smashing attacks
    language : 'c'
)

# ---------- BUILD THE EXECUTABLE ----------
inc_dir = include_directories('inc')

src_files = files(
    'src/main.c',
    'inc/hash_map.c'
)

executable('hash_map',
    src_files,
    include_directories : inc_dir,
    install : false
)
