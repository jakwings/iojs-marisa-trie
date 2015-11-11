{
  'targets': [
    {
      'target_name': 'libmarisa',
      'type': 'static_library',
      'product_prefix': 'lib',
      'sources': [
        'marisa-trie/lib/marisa/agent.cc',
        'marisa-trie/lib/marisa/grimoire/io/mapper.cc',
        'marisa-trie/lib/marisa/grimoire/io/reader.cc',
        'marisa-trie/lib/marisa/grimoire/io/writer.cc',
        'marisa-trie/lib/marisa/grimoire/trie/louds-trie.cc',
        'marisa-trie/lib/marisa/grimoire/trie/tail.cc',
        'marisa-trie/lib/marisa/grimoire/vector/bit-vector.cc',
        'marisa-trie/lib/marisa/keyset.cc',
        'marisa-trie/lib/marisa/trie.cc',
      ],
      'include_dirs': [
        'marisa-trie/lib'
      ],
      'conditions': [
        ['OS == "mac"', {
          'xcode_settings': {
            'GCC_ENABLE_CPP_EXCEPTIONS': 'YES',
          },
        }],
        ['OS == "linux"', {
          'cflags!': [
            '-fno-exceptions'
          ],
          'cflags_cc!': [
            '-fno-exceptions'
          ],
          'cflags': [
            '-fexceptions',
          ],
          'cflags_cc': [
            '-fexceptions',
          ],
        }],
      ]
    }
  ]
}
