# marisa-trie

[![Build Status](https://travis-ci.org/jakwings/iojs-marisa-trie.svg)](https://travis-ci.org/jakwings/iojs-marisa-trie)
[![NPM version](https://badge.fury.io/js/marisa-trie.svg)](http://badge.fury.io/js/marisa-trie)

This package provides a wrapper (a set of bindings) of [MARISA-Trie].

Node requirements:

*   Version >= 0.11.14 or Version == 0.10.x or Version == 0.8.x

[MARISA-Trie]: https://code.google.com/p/marisa-trie/


## Installation

```bash
npm install marisa-trie
```

If installation failed, try to update npm via `npm install -g npm` and [update
node-gyp bundled with npm].

[update node-gyp bundled with npm]:
https://github.com/TooTallNate/node-gyp/wiki/Updating-npm's-bundled-node-gyp


## Usage

The APIs are almost the same as the C++ library's. For example:

```javascript
var Marisa = require('marisa-trie');

var keyset = new Marisa.Keyset();  // The keyword "new" is optional.
keyset.push_back('a');
keyset.push_back('app');
keyset.push_back('apple');

var trie = new Marisa.Trie();
trie.build(keyset);

var agent = new Marisa.Agent();
agent.set_query('apple');
while (trie.common_prefix_search(agent)) {
    var key = agent.key();
    console.log(key.id(), key.ptr());
}
/* OUTPUT:
 * 0 'a'
 * 1 'app'
 * 2 'apple'
 */
```


## APIs

For detailed information, please see the [official site].

[official site]: http://marisa-trie.googlecode.com/svn/trunk/docs/readme.en.html

### Marisa.Trie

```cpp
class Trie {
 public:
  Trie();

  void build(Keyset &keyset, int config_flags = 0);

  void mmap(const char *filename);
  void map(const void *ptr, std::size_t size);

  void load(const char *filename);
  void read(int fd);

  void save(const char *filename) const;
  void write(int fd) const;

  bool lookup(Agent &agent) const;
  void reverse_lookup(Agent &agent) const;
  bool common_prefix_search(Agent &agent) const;
  bool predictive_search(Agent &agent) const;

  std::size_t num_tries() const;
  std::size_t num_keys() const;
  std::size_t num_nodes() const;

  TailMode tail_mode() const;
  NodeOrder node_order() const;

  bool empty() const;
  std::size_t size() const;
  std::size_t io_size() const;

  void clear();
  void swap(Trie &rhs);
};
```

*   `build(keyset:Keyset [, flag:Number])`

    Initialize the trie with a keyset.

*   `mmap(filepath:String)`

    Map a file of trie data into memory and build the trie with it.

*   `map(buffer:Buffer [, length:Number])`

    Map a UTF8-encoded string buffer into memory and build the trie with it.
    If `length` is specified, only load the first `length` bytes of the buffer.

*   `load(filepath:String)`

    Load trie data from a file and build the trie.

*   `read(fd:Number)`

    Read trie data from a ReadStream and build the trie. `fd` is an integer
    file descriptor.

*   `save(filepath:String)`

    Dump the trie data into a file.

*   `write(fd:Number)`

    Dump the trie data into a WriteStream. `fd` is an integer file descriptor'.

*   `empty(): Boolean`

    Check whether the trie has no keys.

*   `size(): Number`

    Return the number of keys. It is the same as `num_keys()`.

*   `io_size(): Number`

    Return the size of trie data in byte.

*   `lookup(agent:Agent): Boolean`

    Check whether the key `agent.query().ptr()` is registered. If it is
    registered, return `true`, otherwise `false`. The search result is available
    through `agent.key()` and `agent.key().ptr()` should be equal to
    `agent.query().ptr()`.

*   `reverse_lookup(agent:Agent)`

    Restore a key with its ID `agent.query().id()`. The result is
    `agent.key().ptr()`. This method will throw an error if the ID is out of range.

*   `common_prefix_search(agent:Agent): Boolean`

    Search keys with the possible prefixes of the query string
    `agent.query().ptr()`. If any matching key is found, return true. The first key
    is available through `agent.key()`. The next match will be available after the
    next `common_prefix_search()`.

*   `predictive_search(agent:Agent): Boolean`

    Search keys started with the query string `agent.query().ptr()`. And
    Similar to `common_prefix_search()`, it will return true until there are no
    more matching keys. **There is one difference from the C++ version**:
    `agent.key().length()` is equal to `Buffer.byteLength(agent.key().ptr())`.

Note: An agent keeps the internal state of `common_prefix_search()` and
`predictive_search()` until it is passed to another search function or
`agent.set_query()` is called.

### Marisa.Keyset

```cpp
class Keyset {
 public:
  Keyset();

  void push_back(const Key &key);
  //void push_back(const Key &key, char end_marker);

  void push_back(const char *str);
  void push_back(const char *ptr, std::size_t length, float weight = 1.0);

  const Key &operator[](std::size_t i) const;
  Key &operator[](std::size_t i);

  std::size_t num_keys();

  bool empty() const;
  std::size_t size() const;
  std::size_t total_length() const;

  void reset();

  void clear();
  void swap(Keyset &rhs);
};
```

*   `[index]: Key`

    Access the key at the index. Return `undefined` if the index is out of
    range.

*   `push_back(key:Key)`

    Add one key to the key set.

*   `push_back(string:String [, length:Number [, weight:Number]])`

    Add one key to the key set. If `length` is specified, only read the first
    `length` bytes from the string. And `weight` can be used as the frequency
    of the key to help optimize the searching performance optionally.  Weights
    will be accumulated for the same keys. After building the trie, weights
    will be overwritten by key IDs because every key uses a union to store
    a weight or an ID.

    If the key string contains NUL (0x00) characters, please specify the length
    explicitly so as to keep the NULs.

*   `empty(): Boolean`

    Check whether the key set has no keys.

*   `size(): Number`

    Return the number of keys. It is the same as `num_keys()`.

*   `total_length(): Number`

    Return the total length of all keys in byte.

### Marisa.Agent

```cpp
class Agent {
 public:
  Agent();

  const Query &query() const;
  const Key &key() const;

  void set_query(const char *str);
  void set_query(const char *ptr, std::size_t length);
  void set_query(std::size_t key_id);
};
```

*   `query(): Query`

    Return the query information.

*   `key(): Key`

    Return the search result.

*   `set_query(string:String [, length:Number])`

    Set the query string. If `length` is specified, read only the first
    `length` bytes from the string.

    If the query string contains NUL (0x00) characters, please specify the
    length explicitly so as to keep the NULs.

*   `set_query(id:Number)`

    Set the ID of key to restore.

### Marisa.Query

```cpp
class Query {
 public:
  char operator[](std::size_t i) const;
  const char *ptr() const;
  std::size_t length() const;
  std::size_t id() const;
};
```

*   `[index]: Number`

    Access the UTF-8 unit at the index. Return `undefined` if the index is out
    of range.

*   `ptr(): String`

    Return the query string.

*   `length(): Number`

    Return the byte-length of the query string.

*   `id(): Number`

    Return the ID of the key to restore.

### Marisa.Key

```cpp
class Key {
 public:
  char operator[](std::size_t i) const;
  const char *ptr() const;
  std::size_t length() const;
  std::size_t id() const;
};
```

*   `[index]: Number`

    Access the UTF-8 unit at the index. Return `undefined` if the index is out
    of range.

*   `ptr(): String`

    Return the last result of the query as a string.

*   `length(): Number`

    Return the byte-length of the result string.

*   `id(): Number`

    Return the ID of the key.

### Enumerations

```cpp
typedef enum marisa_num_tries_ {
  MARISA_MIN_NUM_TRIES     = 0x00001,
  MARISA_MAX_NUM_TRIES     = 0x0007F,
  MARISA_DEFAULT_NUM_TRIES = 0x00003,
} marisa_num_tries;
```

*   `Marisa.MIN_NUM_TRIES`
*   `Marisa.MAX_NUM_TRIES`
*   `Marisa.DEFAULT_NUM_TRIES`

```cpp
typedef enum marisa_cache_level_ {
  MARISA_HUGE_CACHE    = 0x00080,
  MARISA_LARGE_CACHE   = 0x00100,
  MARISA_NORMAL_CACHE  = 0x00200,
  MARISA_SMALL_CACHE   = 0x00400,
  MARISA_TINY_CACHE    = 0x00800,
  MARISA_DEFAULT_CACHE = MARISA_NORMAL_CACHE
} marisa_cache_level;
```

*   `Marisa.HUGE_CACHE`
*   `Marisa.LARGE_CACHE`
*   `Marisa.NORMAL_CACHE`
*   `Marisa.SMALL_CACHE`
*   `Marisa.TINY_CACHE`
*   `Marisa.DEFAULT_CACHE`

```cpp
typedef enum marisa_tail_mode_ {
  MARISA_TEXT_TAIL    = 0x01000,
  MARISA_BINARY_TAIL  = 0x02000,
  MARISA_DEFAULT_TAIL = MARISA_TEXT_TAIL,
} marisa_tail_mode;
```

*   `Marisa.TEXT_TAIL`
*   `Marisa.BINARY_TAIL`
*   `Marisa.DEFAULT_TAIL`

```cpp
typedef enum marisa_node_order_ {
  MARISA_LABEL_ORDER   = 0x10000,
  MARISA_WEIGHT_ORDER  = 0x20000,
  MARISA_DEFAULT_ORDER = MARISA_WEIGHT_ORDER,
} marisa_node_order;
```

*   `Marisa.LABEL_ORDER`
*   `Marisa.WEIGHT_ORDER`
*   `Marisa.DEFAULT_ORDER`

```cpp
typedef enum marisa_config_mask_ {
  MARISA_NUM_TRIES_MASK    = 0x0007F,
  MARISA_CACHE_LEVEL_MASK  = 0x00F80,
  MARISA_TAIL_MODE_MASK    = 0x0F000,
  MARISA_NODE_ORDER_MASK   = 0xF0000,
  MARISA_CONFIG_MASK       = 0xFFFFF
} marisa_config_mask;
```

*   `Marisa.NUM_TRIES_MASK`
*   `Marisa.CACHE_LEVEL_MASK`
*   `Marisa.TAIL_MODE_MASK`
*   `Marisa.NODE_ORDER_MASK`
*   `Marisa.CONFIG_MASK`


## Benchmarks

Node v0.10.40 on my Macbook i5 2.4Ghz (Your mileage may vary.):

```
 METHOD               | TIME (ms/r) | SPEED (K/s) | SPEED (ns/key) | SIZE (N) | IO_SIZE (bytes) | ROUNDS (N)
 build                | 412.37      | 863.35      | 1158.28        | 356017   | 1301528         | 10
 lookup               | 284.85      | 1249.86     | 800.09         | 356017   | 1301528         | 10
 reverse_lookup       | 198.15      | 1796.67     | 556.59         | 356017   | 1301528         | 10
 common_prefix_search | 347.49      | 1024.53     | 976.05         | 356017   | 1301528         | 10
 predictive_search    | 379.03      | 939.29      | 1064.64        | 356017   | 1301528         | 10
```

> K/s – thousand keys per second

Normally, the speed of `reverse_lookup` should be the highest.

Travis CI Results: <https://travis-ci.org/jakwings/iojs-marisa-trie>


## License

The wrapper code is licensed under ISC License.

The bundled source code of [MARISA-Trie] is dual-licensed under LGPL-2.1+ and BSD-2-Clause license.
