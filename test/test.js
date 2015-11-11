var fs = require('fs');
var ts = require('temp-fs');
var readline = require('readline');
var should = require('should').noConflict();
var Marisa = require(__dirname + '/..');

ts.track(true);

var unicode = '\u5927\ud842\udfb7\u5927\u5229';  // "大𠮷大利"
var simpleTextPath = __dirname + '/fixtures/test.txt';
var simpleDataPath = __dirname + '/fixtures/test.dat';
var complexTextPath = __dirname + '/fixtures/words.txt';
var complexDataPath = __dirname + '/fixtures/words.dat';

var uint2float = function (n) {
  var buffer = new Buffer(4);
  buffer.writeUInt32LE(n, 0);
  return buffer.readFloatLE(0);
};
var float2uint = function (n) {
  var buffer = new Buffer(4);
  buffer.writeFloatLE(n, 0);
  return buffer.readUInt32LE(0);
};
var testLookup = function (trie, keyset, skip) {
  var agent = new Marisa.Agent();
  for (var i = 0, l = keyset.size(); i < l; i++) {
    var key = keyset[i];
    agent.set_query(key.ptr());
    should(trie.lookup(agent)).be.true();
    should(agent.key().weight()).equal(uint2float(key.id()));
    should(agent.key().id()).equal(key.id());
    should(agent.key().length()).equal(key.length());
    should(agent.key().length()).equal(Buffer.byteLength(agent.key().ptr()));
    should(agent.key().ptr()).equal(key.ptr());
  }
};
var testReverseLookup = function (trie, keyset, skip) {
  var agent = new Marisa.Agent();
  for (var i = 0, l = keyset.size(); i < l; i++) {
    var key = keyset[i];
    agent.set_query(key.id());
    if (!skip) {
      should(function () {
        should(trie.reverse_lookup(agent)).be.undefined();
      }).not.throw();
    } else {
      should(trie.reverse_lookup(agent)).be.undefined();
    }
    should(agent.key().weight()).equal(uint2float(key.id()));
    should(agent.key().id()).equal(key.id());
    should(agent.key().length()).equal(key.length());
    should(agent.key().length()).equal(Buffer.byteLength(agent.key().ptr()));
    should(agent.key().ptr()).equal(key.ptr());
  }
};
var testCommonPrefixSearch = function (trie, keyset, skip) {
  var agent = new Marisa.Agent();
  for (var i = 0, l = keyset.size(); i < l; i++) {
    var key = keyset[i];
    agent.set_query(key.ptr());
    var found = false;
    while (found = trie.common_prefix_search(agent)) {
      should(found).be.true();
      should(key.ptr().indexOf(agent.key().ptr())).equal(0);
      should(agent.key().weight()).equal(uint2float(agent.key().id()));
      should(agent.key().id() <= key.id()).be.true();
      should(agent.key().length() <= key.length()).be.true();
      should(agent.key().length()).equal(Buffer.byteLength(agent.key().ptr()));
    }
    should(found).be.false();
    should(agent.key().id()).equal(key.id());
  }
};
var testPredictiveSearch = function (trie, keyset, skip) {
  var agent = new Marisa.Agent();
  for (var i = 0, l = keyset.size(); i < l; i++) {
    var key = keyset[i];
    agent.set_query(key.ptr());
    var found = false;
    should(found = trie.predictive_search(agent)).be.true();
    should(agent.key().id()).equal(key.id());
    while (found = trie.predictive_search(agent)) {
      should(found).be.true();
      should(agent.key().weight()).equal(uint2float(agent.key().id()));
      should(agent.key().id() > key.id()).be.true();
      should(agent.key().length() > key.length()).be.true();
      should(agent.key().length()).equal(Buffer.byteLength(agent.key().ptr()));
      should(agent.key().ptr().indexOf(key.ptr())).equal(0);
    }
    should(found).be.false();
  }
};


describe('Marisa', function () {

  it('checking exported classes and properties...', function () {
    should(Marisa).be.an.Object();
    should(Marisa).have.keys(
        'MIN_NUM_TRIES',
        'MAX_NUM_TRIES',
        'DEFAULT_NUM_TRIES',

        'HUGE_CACHE',
        'LARGE_CACHE',
        'NORMAL_CACHE',
        'SMALL_CACHE',
        'TINY_CACHE',
        'DEFAULT_CACHE',

        'TEXT_TAIL',
        'BINARY_TAIL',
        'DEFAULT_TAIL',

        'LABEL_ORDER',
        'WEIGHT_ORDER',
        'DEFAULT_ORDER',

        'NUM_TRIES_MASK',
        'CACHE_LEVEL_MASK',
        'TAIL_MODE_MASK',
        'NODE_ORDER_MASK',
        'CONFIG_MASK',

        'Query', 'Key', 'Keyset', 'Agent', 'Trie');
  });

  it('Marisa.MIN_NUM_TRIES', function () {
    should(Marisa.MIN_NUM_TRIES).equal(0x00001);
  });
  it('Marisa.MAX_NUM_TRIES', function () {
    should(Marisa.MAX_NUM_TRIES).equal(0x0007F);
  });
  it('Marisa.DEFAULT_NUM_TRIES', function () {
    should(Marisa.DEFAULT_NUM_TRIES).equal(0x00003);
  });

  it('Marisa.HUGE_CACHE', function () {
    should(Marisa.HUGE_CACHE).equal(0x00080);
  });
  it('Marisa.LARGE_CACHE', function () {
    should(Marisa.LARGE_CACHE).equal(0x00100);
  });
  it('Marisa.NORMAL_CACHE', function () {
    should(Marisa.NORMAL_CACHE).equal(0x00200);
  });
  it('Marisa.SMALL_CACHE', function () {
    should(Marisa.SMALL_CACHE).equal(0x00400);
  });
  it('Marisa.TINY_CACHE', function () {
    should(Marisa.TINY_CACHE).equal(0x00800);
  });
  it('Marisa.DEFAULT_CACHE', function () {
    should(Marisa.DEFAULT_CACHE).equal(Marisa.NORMAL_CACHE);
  });

  it('Marisa.TEXT_TAIL', function () {
    should(Marisa.TEXT_TAIL).equal(0x01000);
  });
  it('Marisa.BINARY_TAIL', function () {
    should(Marisa.BINARY_TAIL).equal(0x02000);
  });
  it('Marisa.DEFAULT_TAIL', function () {
    should(Marisa.DEFAULT_TAIL).equal(Marisa.TEXT_TAIL);
  });

  it('Marisa.LABEL_ORDER', function () {
    should(Marisa.LABEL_ORDER).equal(0x10000);
  });
  it('Marisa.WEIGHT_ORDER', function () {
    should(Marisa.WEIGHT_ORDER).equal(0x20000);
  });
  it('Marisa.DEFAULT_ORDER', function () {
    should(Marisa.DEFAULT_ORDER).equal(Marisa.WEIGHT_ORDER);
  });

  it('Marisa.NUM_TRIES_MASK', function () {
    should(Marisa.NUM_TRIES_MASK).equal(0x0007F);
  });
  it('Marisa.CACHE_LEVEL_MASK', function () {
    should(Marisa.CACHE_LEVEL_MASK).equal(0x00F80);
  });
  it('Marisa.TAIL_MODE_MASK', function () {
    should(Marisa.TAIL_MODE_MASK).equal(0x0F000);
  });
  it('Marisa.NODE_ORDER_MASK', function () {
    should(Marisa.NODE_ORDER_MASK).equal(0xF0000);
  });
  it('Marisa.CONFIG_MASK', function () {
    should(Marisa.CONFIG_MASK).equal(0xFFFFF);
  });

});


describe('Marisa::Query', function () {

  it('new Marisa.Query', function () {
    should(Marisa.Query).be.a.Function();
    should(Marisa.Query).have.keys();
    should(Marisa.Query.prototype).have.keys(
        'ptr', 'length', 'id', 'swap', 'clear');
    var query = new Marisa.Query();
    should(query).be.an.instanceof(Marisa.Query);
    should(query).eql(query = Marisa.Query());
    should(query).be.an.instanceof(Marisa.Query);
    should(query).eql(query = new Marisa.Query(query));
    should(query).be.an.instanceof(Marisa.Query);
    should(query).eql(query = Marisa.Query(query));
    should(query).be.an.instanceof(Marisa.Query);
    should(function () { new Marisa.Query({}); }).throw();
  });

  it('Marisa::Query::ptr', function () {
    var query = new Marisa.Query();
    should(query.ptr).be.a.Function();
    should(query.ptr()).equal('');
    should(function () { query.ptr.call({}); }).throw();
  });

  it('Marisa::Query::length', function () {
    var query = new Marisa.Query();
    should(query.length).be.a.Function();
    should(query.length()).equal(0);
    should(function () { query.length.call({}); }).throw();
  });

  it('Marisa::Query::id', function () {
    var query = new Marisa.Query();
    should(query.id).be.a.Function();
    should(query.id()).equal(0);
    should(function () { query.id.call({}); }).throw();
  });

  it('Marisa::Query::swap', function () {
    var query = new Marisa.Query();
    should(query.swap).be.a.Function();
    should(query.swap(query)).be.undefined();
    should(function () { query.swap(); }).throw();
    should(function () { query.swap({}); }).throw();
    should(function () { query.swap.call({}, query); }).throw();
  });

  it('Marisa::Query::clear', function () {
    var query = new Marisa.Query();
    should(query.clear).be.a.Function();
    should(query.clear()).be.undefined();
    should(function () { query.clear.call({}); }).throw();
  });

  it('Marisa::Query::[]', function () {
    var query = new Marisa.Query();
    should(query[-1]).be.undefined();
    should(query[0]).be.undefined();
    should(query[1]).be.undefined();
  });

});


describe('Marisa::Key', function () {

  it('new Marisa.Key', function () {
    should(Marisa.Key).be.a.Function();
    should(Marisa.Key).have.keys();
    should(Marisa.Key.prototype).have.keys(
        'ptr', 'length', 'id', 'weight', 'swap', 'clear');
    var key = new Marisa.Key();
    should(key).be.an.instanceof(Marisa.Key);
    should(key).eql(key = Marisa.Key());
    should(key).be.an.instanceof(Marisa.Key);
    should(key).eql(key = new Marisa.Key(key));
    should(key).be.an.instanceof(Marisa.Key);
    should(key).eql(key = Marisa.Key(key));
    should(key).be.an.instanceof(Marisa.Key);
    should(function () { new Marisa.Key({}); }).throw();
  });

  it('Marisa::Key::ptr', function () {
    var key = new Marisa.Key();
    should(key.ptr).be.a.Function();
    should(key.ptr()).equal('');
    should(function () { key.ptr.call({}); }).throw();
  });

  it('Marisa::Key::length', function () {
    var key = new Marisa.Key();
    should(key.length).be.a.Function();
    should(key.length()).equal(0);
    should(function () { key.length.call({}); }).throw();
  });

  it('Marisa::Key::id', function () {
    var key = new Marisa.Key();
    should(key.id).be.a.Function();
    should(key.id()).equal(0);
    should(function () { key.id.call({}); }).throw();
  });

  it('Marisa::Key::weight', function () {
    var key = new Marisa.Key();
    should(key.weight).be.a.Function();
    should(key.weight()).equal(0);
    should(function () { key.weight.call({}); }).throw();
  });

  it('Marisa::Key::swap', function () {
    var key = new Marisa.Key();
    should(key.swap).be.a.Function();
    should(key.swap(key)).be.undefined();
    should(function () { key.swap(); }).throw();
    should(function () { key.swap({}); }).throw();
    should(function () { key.swap.call({}, key); }).throw();
  });

  it('Marisa::Key::clear', function () {
    var key = new Marisa.Key();
    should(key.clear).be.a.Function();
    should(key.clear()).be.undefined();
    should(function () { key.clear.call({}); }).throw();
  });

  it('Marisa::Key::[]', function () {
    var key = new Marisa.Key();
    should(key[-1]).be.undefined();
    should(key[0]).be.undefined();
    should(key[1]).be.undefined();
  });

});


describe('Marisa::Keyset', function () {

  it('new Marisa.Keyset', function () {
    should(Marisa.Keyset).be.a.Function();
    should(Marisa.Keyset).have.keys();
    should(Marisa.Keyset.prototype).have.keys(
        'push_back', 'num_keys', 'empty', 'size',
        'total_length', 'reset', 'swap', 'clear');
    var keyset = new Marisa.Keyset();
    should(keyset).be.an.instanceof(Marisa.Keyset);
    should(keyset).eql(keyset = Marisa.Keyset());
    should(keyset).be.an.instanceof(Marisa.Keyset);
  });

  it('Marisa::Keyset::push_back', function () {
    var keyset = new Marisa.Keyset();
    should(keyset.push_back).be.a.Function();
    should(keyset.push_back(unicode)).be.undefined();
    should(keyset.push_back(unicode)).be.undefined();
    should(keyset.push_back(unicode, 100)).be.undefined();
    should(keyset.push_back(unicode, -10)).be.undefined();
    should(keyset.push_back(unicode, 3)).be.undefined();
    should(keyset.push_back(unicode, 3, 0.3)).be.undefined();
    should(keyset.push_back(new Marisa.Key())).be.undefined();
    should(function () { keyset.push_back(); }).throw();
    should(function () { keyset.push_back({}); }).throw();
    should(function () { keyset.push_back(unicode, {}); }).throw();
    should(function () { keyset.push_back(unicode, 3, {}); }).throw();
    should(function () { keyset.push_back.call({}); }).throw();
  });

  it('Marisa::Keyset::empty', function () {
    var keyset = new Marisa.Keyset();
    should(keyset.empty).be.a.Function();
    should(keyset.empty()).be.true();
    keyset.push_back(unicode);
    should(keyset.empty()).be.false();
    should(function () { keyset.empty.call({}); }).throw();
  });

  it('Marisa::Keyset::size', function () {
    var keyset = new Marisa.Keyset();
    should(keyset.size).be.a.Function();
    should(keyset.size()).equal(0);
    keyset.push_back(unicode);
    should(keyset.size()).equal(1);
    keyset.push_back(unicode);
    should(keyset.size()).equal(2);
    keyset.push_back(new Marisa.Key());
    should(keyset.size()).equal(3);
    should(function () { keyset.size.call({}); }).throw();
  });

  it('Marisa::Keyset::num_keys', function () {
    var keyset = new Marisa.Keyset();
    should(keyset.num_keys).be.a.Function();
    should(keyset.num_keys()).equal(0);
    keyset.push_back(unicode);
    should(keyset.num_keys()).equal(1);
    keyset.push_back(unicode);
    should(keyset.num_keys()).equal(2);
    keyset.push_back(new Marisa.Key());
    should(keyset.num_keys()).equal(3);
    should(function () { keyset.num_keys.call({}); }).throw();
  });

  it('Marisa::Keyset::total_length', function () {
    var keyset = new Marisa.Keyset();
    should(keyset.total_length).be.a.Function();
    should(keyset.total_length()).equal(0);
    keyset.push_back(unicode);
    should(keyset.total_length()).equal(Buffer.byteLength(unicode));
    keyset.push_back(unicode);
    should(keyset.total_length()).equal(Buffer.byteLength(unicode) * 2);
    should(function () { keyset.total_length.call({}); }).throw();
  });

  it('Marisa::Keyset::reset', function () {
    var keyset = new Marisa.Keyset();
    should(keyset.reset).be.a.Function();
    keyset.push_back(unicode);
    should(keyset.reset()).be.undefined();
    should(keyset.empty()).be.true();
    should(keyset.size()).equal(0);
    should(keyset.num_keys()).equal(0);
    should(function () { keyset.reset.call({}); }).throw();
  });

  it('Marisa::Keyset::swap', function () {
    var keyset = new Marisa.Keyset();
    should(keyset.swap).be.a.Function();
    keyset.push_back(unicode);
    should(keyset.swap(new Marisa.Keyset())).be.undefined();
    should(keyset.empty()).be.true();
    should(keyset.size()).equal(0);
    should(keyset.num_keys()).equal(0);
    should(function () { keyset.swap(); }).throw();
    should(function () { keyset.swap({}); }).throw();
    should(function () { keyset.swap.call({}, keyset); }).throw();
  });

  it('Marisa::Keyset::clear', function () {
    var keyset = new Marisa.Keyset();
    should(keyset.clear).be.a.Function();
    keyset.push_back(unicode);
    should(keyset.clear()).be.undefined();
    should(keyset.empty()).be.true();
    should(keyset.size()).equal(0);
    should(keyset.num_keys()).equal(0);
    should(function () { keyset.clear.call({}); }).throw();
  });

  it('Marisa::Keyset::[]', function () {
    var keyset = new Marisa.Keyset();
    should(keyset[-1]).be.undefined();
    should(keyset[1]).be.undefined();
    var len = 7;
    var weight = 0.5;
    keyset.push_back(unicode, len, weight);
    var key = keyset[0];
    should(key).be.an.instanceof(Marisa.Key);
    should(key.weight()).equal(weight);
    should(key.id()).equal(float2uint(weight));
    should(key.length()).equal(Buffer(unicode).slice(0, len).length);
    should(key.ptr()).equal(Buffer(unicode).slice(0, len).toString());
    for (var i = 0, l = key.length(), b = Buffer(unicode); i < l; i++) {
      should(key[i]).be.a.Number();
      should(key[i]).equal(b[i]);
    }
  });

});


describe('Marisa::Agent', function () {

  it('new Marisa.Agent', function () {
    should(Marisa.Agent).be.a.Function();
    should(Marisa.Agent).have.keys();
    should(Marisa.Agent.prototype).have.keys(
        'set_query', 'query', 'key', 'swap', 'clear');
    var agent = new Marisa.Agent();
    should(agent).be.an.instanceof(Marisa.Agent);
    should(agent).eql(agent = Marisa.Agent());
    should(agent).be.an.instanceof(Marisa.Agent);
  });

  it('Marisa::Agent::set_query', function () {
    var agent = new Marisa.Agent();
    should(agent.set_query).be.a.Function();
    should(agent.set_query(0)).be.undefined();
    should(agent.set_query(unicode)).be.undefined();
    should(agent.set_query(unicode, 3)).be.undefined();
    should(function () { agent.set_query(); }).throw();
    should(function () { agent.set_query({}); }).throw();
    should(function () { agent.set_query(unicode, {}); }).throw();
    should(function () { agent.set_query.call({}); }).throw();
  });

  it('Marisa::Agent::query', function () {
    var agent = new Marisa.Agent();
    should(agent.query).be.a.Function();
    var query = agent.query();
    should(query).be.an.instanceof(Marisa.Query);
    should(query.ptr()).equal('');
    should(query.length()).equal(0);
    should(query.id()).equal(0);
    var len = 7;
    agent.set_query(unicode, len);
    var query = agent.query();
    should(query.id()).equal(0);
    should(query.length()).equal(Buffer(unicode).slice(0, len).length);
    should(query.ptr()).equal(Buffer(unicode).slice(0, len).toString());
    for (var i = 0, l = query.length(), b = Buffer(unicode); i < l; i++) {
      should(query[i]).be.a.Number();
      should(query[i]).equal(b[i]);
    }

    var agent = new Marisa.Agent();
    agent.set_query(2);
    var query = agent.query();
    should(query.id()).equal(2);
    should(query.length()).equal(0);
    should(query.ptr()).equal('');
    should(function () { agent.query.call({}); }).throw();
  });

  it('Marisa::Agent::key', function () {
    var agent = new Marisa.Agent();
    should(agent.key).be.a.Function();
    var key = agent.key();
    should(key).be.an.instanceof(Marisa.Key);
    should(key.weight()).equal(0);
    should(key.id()).equal(0);
    should(key.length()).equal(0);
    should(key.ptr()).equal('');
    should(function () { agent.key.call({}); }).throw();
  });

  it('Marisa::Agent::swap', function () {
    var agent = new Marisa.Agent();
    should(agent.swap).be.a.Function();
    agent.set_query(unicode);
    should(agent.swap(new Marisa.Agent())).be.undefined();
    var query = agent.query();
    should(query.id()).equal(0);
    should(query.length()).equal(0);
    should(query.ptr()).equal('');

    var agent = new Marisa.Agent();
    agent.set_query(2);
    should(agent.swap(new Marisa.Agent())).be.undefined();
    var key = agent.key();
    should(key.weight()).equal(0);
    should(key.id()).equal(0);
    should(key.length()).equal(0);
    should(key.ptr()).equal('');
    should(function () { agent.swap(); }).throw();
    should(function () { agent.swap({}); }).throw();
    should(function () { agent.swap.call({}, agent); }).throw();
  });

  it('Marisa::Agent::clear', function () {
    var agent = new Marisa.Agent();
    should(agent.clear).be.a.Function();
    agent.set_query(unicode);
    should(agent.clear()).be.undefined();
    var query = agent.query();
    should(query.id()).equal(0);
    should(query.length()).equal(0);
    should(query.ptr()).equal('');

    var agent = new Marisa.Agent();
    agent.set_query(2);
    should(agent.clear()).be.undefined();
    var key = agent.key();
    should(key.weight()).equal(0);
    should(key.id()).equal(0);
    should(key.length()).equal(0);
    should(key.ptr()).equal('');
    should(function () { agent.clear.call({}); }).throw();
  });

});


describe('Marisa::Trie', function () {

  var keyset = new Marisa.Keyset();
  var expectedTrieSize = 0;
  fs.readFileSync(simpleTextPath, 'utf8')
    .split('\n')
    .sort()
    .forEach(function (word, i, words) {
      if (word.trim()) {
        keyset.push_back(word);
        if (words[i-1] !== word) {
          expectedTrieSize++;
        }
      }
    });

  it('new Marisa.Trie', function () {
    should(Marisa.Trie).be.a.Function();
    should(Marisa.Trie).have.keys();
    should(Marisa.Trie.prototype).have.keys(
        'build', 'empty', 'io_size', 'size', 'num_keys', 'num_tries',
        'num_nodes', 'swap', 'clear', 'tail_mode', 'node_order', 'lookup',
        'reverse_lookup', 'common_prefix_search', 'predictive_search',
        'mmap', 'map', 'load', 'read', 'save', 'write');
    var trie = new Marisa.Trie();
    should(trie).be.an.instanceof(Marisa.Trie);
    should(trie).eql(trie = Marisa.Trie());
    should(trie).be.an.instanceof(Marisa.Trie);
  });

  it('Marisa::Trie::build', function () {
    var trie = new Marisa.Trie();
    should(trie.build).be.a.Function();
    should(trie.build(new Marisa.Keyset())).be.undefined();
    should(trie.build(new Marisa.Keyset(),
          Marisa.DEFAULT_NUM_TRIES |
          Marisa.DEFAULT_CACHE |
          Marisa.DEFAULT_TAIL |
          Marisa.DEFAULT_ORDER)).be.undefined();
    should(trie.build(keyset)).be.undefined();
    should(trie.build(keyset,
          Marisa.DEFAULT_NUM_TRIES |
          Marisa.DEFAULT_CACHE |
          Marisa.DEFAULT_TAIL |
          Marisa.DEFAULT_ORDER)).be.undefined();
    should(function () { trie.build(); }).throw();
    should(function () { trie.build({}); }).throw();
    should(function () { trie.build(keyset, {}); }).throw();
    should(function () { trie.build.call({}, keyset); }).throw();
  });

  it('Marisa::Trie::empty', function () {
    var trie = new Marisa.Trie();
    should(trie.empty).be.a.Function();
    should(trie.empty).throw();
    trie.build(new Marisa.Keyset());
    should(trie.empty()).be.true();
    trie.build(keyset);
    should(trie.empty()).be.false();
    should(function () { trie.empty.call({}); }).throw();
  });

  it('Marisa::Trie::io_size', function () {
    var trie = new Marisa.Trie();
    should(trie.io_size).be.a.Function();
    should(trie.io_size).throw();
    trie.build(new Marisa.Keyset());
    should(trie.io_size()).be.a.Number();
    should(trie.io_size() >= 0).be.true();
    trie.build(keyset);
    should(trie.io_size() >= keyset.total_length()).be.true();
    should(function () { trie.io_size.call({}); }).throw();
  });

  it('Marisa::Trie::size', function () {
    var trie = new Marisa.Trie();
    should(trie.size).be.a.Function();
    should(trie.size).throw();
    trie.build(new Marisa.Keyset());
    should(trie.size()).equal(0);
    trie.build(keyset);
    should(trie.size()).equal(expectedTrieSize);
    should(function () { trie.size.call({}); }).throw();
  });

  it('Marisa::Trie::num_keys', function () {
    var trie = new Marisa.Trie();
    should(trie.num_keys).be.a.Function();
    should(trie.num_keys).throw();
    trie.build(new Marisa.Keyset());
    should(trie.num_keys()).equal(0);
    trie.build(keyset);
    should(trie.num_keys()).equal(expectedTrieSize);
    should(function () { trie.num_keys.call({}); }).throw();
  });

  it('Marisa::Trie::num_tries', function () {
    var trie = new Marisa.Trie();
    should(trie.num_tries).be.a.Function();
    should(trie.num_tries).throw();
    trie.build(new Marisa.Keyset());
    should(trie.num_tries()).be.a.Number();
    should(trie.num_tries() >= 0).be.true();
    should(function () { trie.num_tries.call({}); }).throw();
  });

  it('Marisa::Trie::num_nodes', function () {
    var trie = new Marisa.Trie();
    should(trie.num_nodes).be.a.Function();
    should(trie.num_nodes).throw();
    trie.build(new Marisa.Keyset());
    should(trie.num_nodes()).be.a.Number();
    should(trie.num_nodes() >= 0).be.true();
    should(function () { trie.num_nodes.call({}); }).throw();
  });

  it('Marisa::Trie::swap', function () {
    var trie = new Marisa.Trie();
    should(trie.swap).be.a.Function();
    trie.build(keyset);
    var emptyTrie = new Marisa.Trie();
    emptyTrie.build(new Marisa.Keyset());
    var size = trie.io_size();
    should(trie.swap(emptyTrie)).be.undefined();
    should(trie.empty()).be.true();
    should(trie.io_size() < size).be.true();
    should(trie.size()).equal(0);
    should(trie.num_keys()).equal(0);
    should(trie.num_tries()).be.a.Number();
    should(trie.num_nodes()).be.a.Number();
    should(function () { trie.swap(); }).throw();
    should(function () { trie.swap({}); }).throw();
    should(function () { trie.swap.call({}, trie); }).throw();
  });

  it('Marisa::Trie::clear', function () {
    var trie = new Marisa.Trie();
    should(trie.clear).be.a.Function();
    trie.build(keyset);
    should(trie.clear()).be.undefined();
    should(trie.empty).throw();
    should(function () { trie.clear.call({}); }).throw();
  });

  it('Marisa::Trie::tail_mode', function () {
    var trie = new Marisa.Trie();
    should(trie.tail_mode).be.a.Function();
    should(trie.tail_mode).throw();
    trie.build(keyset, Marisa.BINARY_TAIL);
    should(trie.tail_mode()).equal(Marisa.BINARY_TAIL);
    should(function () { trie.tail_mode.call({}); }).throw();
  });

  it('Marisa::Trie::node_order', function () {
    var trie = new Marisa.Trie();
    should(trie.node_order).be.a.Function();
    should(trie.node_order).throw();
    trie.build(keyset, Marisa.LABEL_ORDER);
    should(trie.node_order()).equal(Marisa.LABEL_ORDER);
    should(function () { trie.node_order.call({}); }).throw();
  });

  it('Marisa::Trie::lookup', function () {
    var trie = new Marisa.Trie();
    should(trie.lookup).be.a.Function();
    should(trie.lookup).throw();
    trie.build(keyset);
    testLookup(trie, keyset);
    should(function () { trie.lookup(); }).throw();
    should(function () { trie.lookup({}); }).throw();
    should(function () { trie.lookup.call({}, agent); }).throw();
  });

  it('Marisa::Trie::reverse_lookup', function () {
    var trie = new Marisa.Trie();
    should(trie.reverse_lookup).be.a.Function();
    should(trie.reverse_lookup).throw();
    trie.build(keyset);
    testReverseLookup(trie, keyset);
    should(function () { trie.reverse_lookup(); }).throw();
    should(function () { trie.reverse_lookup({}); }).throw();
    should(function () { trie.reverse_lookup.call({}, agent); }).throw();
  });

  it('Marisa::Trie::common_prefix_search', function () {
    var trie = new Marisa.Trie();
    should(trie.common_prefix_search).be.a.Function();
    should(trie.common_prefix_search).throw();
    trie.build(keyset);
    testCommonPrefixSearch(trie, keyset);
    should(function () { trie.common_prefix_search(); }).throw();
    should(function () { trie.common_prefix_search({}); }).throw();
    should(function () { trie.common_prefix_search.call({}, agent); }).throw();
  });

  it('Marisa::Trie::predictive_search', function () {
    var trie = new Marisa.Trie();
    should(trie.predictive_search).be.a.Function();
    should(trie.predictive_search).throw();
    trie.build(keyset);
    testPredictiveSearch(trie, keyset);
    should(function () { trie.predictive_search(); }).throw();
    should(function () { trie.predictive_search({}); }).throw();
    should(function () { trie.predictive_search.call({}, agent); }).throw();
  });

  it('Marisa::Trie::mmap', function () {
    var trie = new Marisa.Trie();
    should(trie.mmap).be.a.Function();
    should(trie.mmap).throw();
    should(trie.mmap(simpleDataPath)).be.undefined();
    should(trie.size()).equal(expectedTrieSize);
    testLookup(trie, keyset);
    should(function () { trie.mmap(); }).throw();
    should(function () { trie.mmap(__filename); }).throw();
    should(function () { trie.mmap({}); }).throw();
    should(function () { trie.mmap.call({}, agent); }).throw();
  });

  it('Marisa::Trie::map', function () {
    var trie = new Marisa.Trie();
    should(trie.map).be.a.Function();
    should(trie.map).throw();
    var buffer = fs.readFileSync(simpleDataPath);
    should(trie.map(buffer)).be.undefined();
    should(trie.size()).equal(expectedTrieSize);
    should(trie.map(buffer, buffer.length)).be.undefined();
    should(trie.size()).equal(expectedTrieSize);
    testLookup(trie, keyset);
    should(function () { trie.map(); }).throw();
    should(function () { trie.map({}); }).throw();
    should(function () { trie.map.call({}, agent); }).throw();
  });

  it('Marisa::Trie::load', function () {
    var trie = new Marisa.Trie();
    should(trie.load).be.a.Function();
    should(trie.load).throw();
    should(trie.load(simpleDataPath)).be.undefined();
    should(trie.size()).equal(expectedTrieSize);
    testLookup(trie, keyset);
    should(function () { trie.load(); }).throw();
    should(function () { trie.load(__filename); }).throw();
    should(function () { trie.load({}); }).throw();
    should(function () { trie.load.call({}, agent); }).throw();
  });

  it('Marisa::Trie::read', function () {
    var trie = new Marisa.Trie();
    should(trie.read).be.a.Function();
    should(trie.read).throw();
    var fd = fs.openSync(simpleDataPath, 'r');
    should(trie.read(fd)).be.undefined();
    fs.closeSync(fd);
    should(trie.size()).equal(expectedTrieSize);
    testLookup(trie, keyset);
    should(function () { trie.read(); }).throw();
    should(function () { trie.read({}); }).throw();
    should(function () { trie.read.call({}, agent); }).throw();
  });

  it('Marisa::Trie::save', function () {
    var trie = new Marisa.Trie();
    should(trie.save).be.a.Function();
    should(trie.save).throw();
    trie.mmap(simpleDataPath);
    var tmpfile = ts.openSync({dir: __dirname});
    should(trie.save(tmpfile.path)).be.undefined();
    testLookup(trie, keyset);
    tmpfile.unlink();
    should(function () { trie.save(); }).throw();
    should(function () { trie.save(__dirname); }).throw();
    should(function () { trie.save({}); }).throw();
    should(function () { trie.save.call({}, agent); }).throw();
  });

  it('Marisa::Trie::write', function () {
    var trie = new Marisa.Trie();
    should(trie.write).be.a.Function();
    should(trie.write).throw();
    trie.mmap(simpleDataPath);
    var tmpfile = ts.openSync({dir: __dirname});
    should(trie.write(tmpfile.fd)).be.undefined();
    testLookup(trie, keyset);
    tmpfile.unlink();
    should(function () { trie.write(); }).throw();
    should(function () { trie.write(__dirname); }).throw();
    should(function () { trie.write({}); }).throw();
    should(function () { trie.write.call({}, agent); }).throw();
  });

});

describe('Complex data test', function () {

  var trie = new Marisa.Trie();
  var keyset = new Marisa.Keyset();
  var expectedTrieSize = 0;

  before(function (done) {
    readline.createInterface({
      input: fs.createReadStream(complexTextPath),
      output: process.stdout,
      terminal: false
    }).on('line', function (word) {
      if (word.trim()) {
        keyset.push_back(word, Buffer.byteLength(word), Math.random());
        expectedTrieSize++;
      }
    }).on('close', done);
  });

  beforeEach('rebuild', function (done) {
    this.timeout(120000);
    trie.build(keyset,
        Marisa.MAX_NUM_TRIES |
        Marisa.HUGE_CACHE |
        Marisa.TEXT_TAIL |
        Marisa.LABEL_ORDER);
    should(trie.size()).equal(expectedTrieSize);
    done();
  });

  it('build', function (done) {
    this.timeout(120000);
    trie.build(keyset,
        Marisa.MAX_NUM_TRIES |
        Marisa.HUGE_CACHE |
        Marisa.TEXT_TAIL |
        Marisa.LABEL_ORDER);
    should(trie.size()).equal(expectedTrieSize);
    done();
  });

  it('save/load', function (done) {
    this.timeout(120000);
    var tmpfile = ts.openSync({dir: __dirname});
    trie.save(tmpfile.path);
    trie.load(tmpfile.path);
    tmpfile.unlink();
    should(trie.size()).equal(expectedTrieSize);
    done();
  });

  it('write/read', function (done) {
    this.timeout(120000);
    var tmpfile = ts.openSync({dir: __dirname});
    trie.write(tmpfile.fd);
    fs.closeSync(tmpfile.fd);
    var fd = fs.openSync(tmpfile.path, 'r');
    trie.read(fd);
    fs.closeSync(fd);
    tmpfile.unlink();
    should(trie.size()).equal(expectedTrieSize);
    done();
  });

  it('lookup', function (done) {
    this.timeout(120000);
    testLookup(trie, keyset);
    done();
  });

  it('reverse_lookup', function (done) {
    this.timeout(120000);
    testReverseLookup(trie, keyset, true);
    done();
  });

  it('common_prefix_search', function (done) {
    this.timeout(120000);
    testCommonPrefixSearch(trie, keyset, true);
    done();
  });

  it('predictive_search', function (done) {
    this.timeout(120000);
    testPredictiveSearch(trie, keyset, true);
    done();
  });

});
