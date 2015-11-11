var fs = require('fs');
var readline = require('readline');
var Table = require('cli-table');
var Marisa = require(__dirname + '/..');

var Timer = function (title) {
  this.tick = function () {
    var diff = process.hrtime(this.time);
    this.tick = null;
    process.stdout.write(' Done!\n\n');
    return diff[0] * 1e9 + diff[1];
  };
  process.stdout.write('Benchmarking ' + title + ' ... ');
  this.time = process.hrtime();
};


var table = new Table({
  head: ['METHOD', 'TIME (ms/r)', 'SPEED (K/s)', 'SPEED (ns/key)', 'SIZE (N)', 'IO_SIZE (bytes)', 'ROUNDS (N)'],
  chars: {'top': '', 'top-mid': '', 'top-left': '', 'top-right': '', 'bottom': '', 'bottom-mid': '', 'bottom-left': '', 'bottom-right': '', 'left': '', 'left-mid': '', 'right': '', 'right-mid': '', 'middle': '|', 'mid': '', 'mid-mid': ''}
});

var datadir = __dirname + '/../test/fixtures';
var words = [];
var rl = readline.createInterface({
  input: fs.createReadStream(datadir + '/words.txt'),
  output: process.stdout,
  terminal: false
});
rl.on('line', function (word) {
  word = word.trim();
  if (word) {
    words.push(word);
  }
});
rl.on('close', function () {
  var trie = new Marisa.Trie();
  var agent = new Marisa.Agent();
  var keyset = new Marisa.Keyset();
  for (var i = 0; i < words.length; i++) {
    keyset.push_back(words[i], Buffer.byteLength(words[i]), Math.random());
  }

  var cycles = 10;
  var timer = new Timer('Marisa::Trie::build');
  for (var i = 1; i <= cycles; i++) {
    trie.build(keyset,
        Marisa.MARISA_MAX_NUM_TRIES |
        Marisa.MARISA_HUGE_CACHE |
        Marisa.MARISA_TEXT_TAIL |
        Marisa.MARISA_LABEL_ORDER);
  }
  var elapsedTime = timer.tick();
  table.push([
    'build',
    (elapsedTime / 1e6 / cycles).toFixed(2),
    (cycles * trie.size() / elapsedTime * 1e6).toFixed(2),
    (elapsedTime / cycles / trie.size()).toFixed(2),
    trie.size(),
    trie.io_size(),
    cycles
  ]);


  var cycles = 10;
  var timer = new Timer('Marisa::Trie::lookup');
  for (var i = 1; i <= cycles; i++) {
    for (var j = 0, l = keyset.size(); j < l; j++) {
      agent.set_query(words[j]);
      trie.lookup(agent);
    }
  }
  var elapsedTime = timer.tick();
  table.push([
    'lookup',
    (elapsedTime / 1e6 / cycles).toFixed(2),
    (cycles * trie.size() / elapsedTime * 1e6).toFixed(2),
    (elapsedTime / cycles / trie.size()).toFixed(2),
    trie.size(),
    trie.io_size(),
    cycles
  ]);


  var cycles = 10;
  var timer = new Timer('Marisa::Trie::reverse_lookup');
  for (var i = 1; i <= cycles; i++) {
    for (var j = 0, l = keyset.size(); j < l; j++) {
      agent.set_query(j);
      trie.reverse_lookup(agent);
    }
  }
  var elapsedTime = timer.tick();
  table.push([
    'reverse_lookup',
    (elapsedTime / 1e6 / cycles).toFixed(2),
    (cycles * trie.size() / elapsedTime * 1e6).toFixed(2),
    (elapsedTime / cycles / trie.size()).toFixed(2),
    trie.size(),
    trie.io_size(),
    cycles
  ]);


  var cycles = 10;
  var timer = new Timer('Marisa::Trie::common_prefix_search');
  for (var i = 1; i <= cycles; i++) {
    for (var j = 0, l = keyset.size(); j < l; j++) {
      agent.set_query(words[j]);
      while (trie.common_prefix_search(agent)) {
        // Do nothing.
      }
    }
  }
  var elapsedTime = timer.tick();
  table.push([
    'common_prefix_search',
    (elapsedTime / 1e6 / cycles).toFixed(2),
    (cycles * trie.size() / elapsedTime * 1e6).toFixed(2),
    (elapsedTime / cycles / trie.size()).toFixed(2),
    trie.size(),
    trie.io_size(),
    cycles
  ]);


  var cycles = 10;
  var timer = new Timer('Marisa::Trie::predictive_search');
  for (var i = 1; i <= cycles; i++) {
    for (var j = 0, l = keyset.size(); j < l; j++) {
      agent.set_query(words[j]);
      while (trie.predictive_search(agent)) {
        // Do nothing.
      }
    }
  }
  var elapsedTime = timer.tick();
  table.push([
    'predictive_search',
    (elapsedTime / 1e6 / cycles).toFixed(2),
    (cycles * trie.size() / elapsedTime * 1e6).toFixed(2),
    (elapsedTime / cycles / trie.size()).toFixed(2),
    trie.size(),
    trie.io_size(),
    cycles
  ]);

  console.log(table.toString());
});
