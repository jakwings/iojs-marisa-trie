{
  'targets': [
    {
      'target_name': 'binding',
      'sources': [
        'src/binding/binding.cpp',
        'src/binding/agent-wrap.cpp',
        'src/binding/key-wrap.cpp',
        'src/binding/keyset-wrap.cpp',
        'src/binding/marisa-wrap.cpp',
        'src/binding/query-wrap.cpp',
        'src/binding/trie-wrap.cpp',
      ],
      'dependencies': [
        'src/marisa-trie.gyp:libmarisa'
      ],
      'include_dirs': [
        'src/marisa-trie/lib',
        '<!(node -e "require(\'nan\')")',
      ],
      'library_dirs': [
        '<(PRODUCT_DIR)',
      ],
      'libraries': [
        '-lmarisa',
      ],
      'conditions': [
        ['OS == "mac"', {
          'xcode_settings': {
            'GCC_ENABLE_CPP_EXCEPTIONS': 'YES',
          },
        }],
        ['OS == "linux"', {
          'cflags_cc!': [
            '-fno-exceptions'
          ],
          'cflags_cc': [
            '-fexceptions',
          ],
        }],
      ]
    }
  ]
}
