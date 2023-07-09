void test_re()
{
  assert_true_rule(
      "rule test { strings: $a = /ssi/ condition: $a }",
      "mississippi\tmississippi.mississippi\nmississippi");

  assert_true_rule(
      "rule test { strings: $a = /ssi(s|p)/ condition: $a }",
      "mississippi\tmississippi.mississippi\nmississippi");

  assert_true_rule(
      "rule test { strings: $a = /ssim*/ condition: $a }",
      "mississippi\tmississippi.mississippi\nmississippi");

  assert_true_rule(
      "rule test { strings: $a = /ssa?/ condition: $a }",
      "mississippi\tmississippi.mississippi\nmississippi");

  assert_true_rule(
      "rule test { strings: $a = /Miss/ nocase condition: $a }",
      "mississippi\tmississippi.mississippi\nmississippi");

  assert_true_rule(
      "rule test { strings: $a = /(M|N)iss/ nocase condition: $a }",
      "mississippi\tmississippi.mississippi\nmississippi");

  assert_true_rule(
      "rule test { strings: $a = /[M-N]iss/ nocase condition: $a }",
      "mississippi\tmississippi.mississippi\nmississippi");

  assert_true_rule(
      "rule test { strings: $a = /(Mi|ssi)ssippi/ nocase condition: $a }",
      "mississippi\tmississippi.mississippi\nmississippi");

  assert_true_rule(
      "rule test { strings: $a = /ppi\\tmi/ condition: $a }",
      "mississippi\tmississippi.mississippi\nmississippi");

  assert_true_rule(
      "rule test { strings: $a = /ppi\\.mi/ condition: $a }",
      "mississippi\tmississippi.mississippi\nmississippi");

  assert_true_rule(
      "rule test { strings: $a = /^mississippi/ fullword condition: $a }",
      "mississippi\tmississippi.mississippi\nmississippi");

  assert_true_rule(
      "rule test { strings: $a = /mississippi.*mississippi$/s condition: $a }",
      "mississippi\tmississippi.mississippi\nmississippi");

  assert_false_rule(
      "rule test { strings: $a = /^ssi/ condition: $a }",
      "mississippi");

  assert_false_rule(
      "rule test { strings: $a = /ssi$/ condition: $a }",
      "mississippi");

  assert_false_rule(
      "rule test { strings: $a = /ssissi/ fullword condition: $a }",
      "mississippi");

  assert_false_rule(
      "rule test { strings: $a = /^[isp]+/ condition: $a }",
      "mississippi");

  assert_regexp_syntax_error(")");
  assert_true_regexp("abc", "abc", "abc");
  assert_false_regexp("abc", "xbc");
  assert_false_regexp("abc", "axc");
  assert_false_regexp("abc", "abx");
  assert_true_regexp("abc", "xabcx", "abc");
  assert_true_regexp("abc", "ababc", "abc");
  assert_true_regexp("a.c", "abc", "abc");
  assert_false_regexp("a.b", "a\nb");
  assert_false_regexp("a.*b", "acc\nccb");
  assert_false_regexp("a.{4,5}b", "acc\nccb");
  assert_true_regexp("a.b", "a\rb", "a\rb");
  assert_true_regexp("ab*c", "abc", "abc");
  assert_true_regexp("ab*c", "ac", "ac");
  assert_true_regexp("ab*bc", "abc", "abc");
  assert_true_regexp("ab*bc", "abbc", "abbc");
  assert_true_regexp("a.*bb", "abbbb", "abbbb");
  assert_true_regexp("a.*?bbb", "abbbbbb", "abbb");
  assert_true_regexp("a.*c", "ac", "ac");
  assert_true_regexp("a.*c", "axyzc", "axyzc");
  assert_true_regexp("ab+c", "abbc", "abbc");
  assert_false_regexp("ab+c", "ac");
  assert_true_regexp("ab+", "abbbb", "abbbb");
  assert_true_regexp("ab+?", "abbbb", "ab");
  assert_false_regexp("ab+bc", "abc");
  assert_false_regexp("ab+bc", "abq");
  assert_true_regexp("a+b+c", "aabbabc", "abc");
  assert_false_regexp("ab?bc", "abbbbc");
  assert_true_regexp("ab?c", "abc", "abc");
  assert_true_regexp("ab*?", "abbb", "a");
  assert_true_regexp("ab?c", "abc", "abc");
  assert_true_regexp("ab??", "ab", "a");
  assert_true_regexp("a(b|x)c", "abc", "abc");
  assert_true_regexp("a(b|x)c", "axc", "axc");
  assert_true_regexp("a(b|.)c", "axc", "axc");
  assert_true_regexp("a(b|x|y)c", "ayc", "ayc");
  assert_true_regexp("(a+|b)*", "ab", "ab");
  assert_true_regexp("a|b|c|d|e", "e", "e");
  assert_true_regexp("(a|b|c|d|e)f", "ef", "ef");
  assert_true_regexp(".b{2}", "abb", "abb");
  assert_true_regexp("ab{1}c", "abc", "abc");
  assert_true_regexp("ab{1,2}c", "abbc", "abbc");
  assert_true_regexp("ab{1,}c", "abbbc", "abbbc");
  assert_false_regexp("ab{1,}b", "ab");
  assert_false_regexp("ab{1}c", "abbc");
  assert_true_regexp("ab{0,}c", "ac", "ac");
  assert_true_regexp("ab{0,}c", "abbbc", "abbbc");
  assert_true_regexp("ab{,3}c", "abbbc", "abbbc");
  assert_false_regexp("ab{,2}c", "abbbc");
  assert_false_regexp("ab{4,5}bc", "abbbbc");
  assert_true_regexp("ab{2,3}?", "abbbbb", "abb");
  assert_true_regexp("ab{.*}", "ab{c}", "ab{c}");
  assert_true_regexp(".(aa){1,2}", "aaaaaaaaaa", "aaaaa");
  assert_true_regexp("a.(bc.){2}", "aabcabca", "aabcabca");
  assert_true_regexp("(ab{1,2}c){1,3}", "abbcabc", "abbcabc");
  assert_true_regexp("ab(c|cc){1,3}d", "abccccccd", "abccccccd");
  assert_true_regexp("a[bx]c", "abc", "abc");
  assert_true_regexp("a[bx]c", "axc", "axc");
  assert_true_regexp("a[0-9]*b", "ab", "ab");
  assert_true_regexp("a[0-9]*b", "a0123456789b", "a0123456789b");
  assert_true_regexp("[0-9a-f]+", "0123456789abcdef", "0123456789abcdef");
  assert_true_regexp("[0-9a-f]+", "xyz0123456789xyz", "0123456789");
  assert_true_regexp("a[\\s\\S]b", "a b", "a b");
  assert_true_regexp("a[\\d\\D]b", "a1b", "a1b");
  assert_false_regexp("[x-z]+", "abc");
  assert_true_regexp("a[-]?c", "ac", "ac");
  assert_true_regexp("a[-b]", "a-", "a-");
  assert_true_regexp("a[-b]", "ab", "ab");
  assert_true_regexp("a[b-]", "a-", "a-");
  assert_true_regexp("a[b-]", "ab", "ab");
  assert_true_regexp("[a-c-e]", "b", "b");
  assert_true_regexp("[a-c-e]", "-", "-");
  assert_false_regexp("[a-c-e]", "d");
  assert_regexp_syntax_error("[b-a]");
  assert_regexp_syntax_error("(abc");
  assert_regexp_syntax_error("abc)");
  assert_regexp_syntax_error("a[]b");
  assert_regexp_syntax_error("a\\");
  assert_true_regexp("a[\\-b]", "a-", "a-");
  assert_true_regexp("a[\\-b]", "ab", "ab");
  assert_regexp_syntax_error("a[\\");
  assert_true_regexp("a]", "a]", "a]");
  assert_true_regexp("a[]]b", "a]b", "a]b");
  assert_true_regexp("a[\\]]b", "a]b", "a]b");
  assert_true_regexp("a[^bc]d", "aed", "aed");
  assert_false_regexp("a[^bc]d", "abd");
  assert_true_regexp("a[^-b]c", "adc", "adc");
  assert_false_regexp("a[^-b]c", "a-c");
  assert_false_regexp("a[^]b]c", "a]c");
  assert_true_regexp("a[^]b]c", "adc", "adc");
  assert_true_regexp("[^ab]*", "cde", "cde");
  assert_regexp_syntax_error(")(");
  assert_true_regexp("a\\sb", "a b", "a b");
  assert_true_regexp("a\\sb", "a\tb", "a\tb");
  assert_true_regexp("a\\sb", "a\rb", "a\rb");
  assert_true_regexp("a\\sb", "a\nb", "a\nb");
  assert_true_regexp("a\\sb", "a\vb", "a\vb");
  assert_true_regexp("a\\sb", "a\fb", "a\fb");
  assert_false_regexp("a\\Sb", "a b");
  assert_false_regexp("a\\Sb", "a\tb");
  assert_false_regexp("a\\Sb", "a\rb");
  assert_false_regexp("a\\Sb", "a\nb");
  assert_false_regexp("a\\Sb", "a\vb");
  assert_false_regexp("a\\Sb", "a\fb");
  assert_true_regexp("\\n\\r\\t\\f\\a", "\n\r\t\f\a", "\n\r\t\f\a");
  assert_true_regexp("[\\n][\\r][\\t][\\f][\\a]", "\n\r\t\f\a", "\n\r\t\f\a");
  assert_true_regexp("\\x01\\x02\\x03", "\x01\x02\x03", "\x01\x02\x03");
  assert_true_regexp("[\\x01-\\x03]+", "\x01\x02\x03", "\x01\x02\x03");
  assert_false_regexp("[\\x00-\\x02]+", "\x03\x04\x05");
  assert_true_regexp("[\\x5D]", "]", "]");
  assert_true_regexp("[\\0x5A-\\x5D]", "\x5B", "\x5B");
  assert_true_regexp("[\\x5D-\\x5F]", "\x5E", "\x5E");
  assert_true_regexp("[\\x5C-\\x5F]", "\x5E", "\x5E");
  assert_true_regexp("[\\x5D-\\x5F]", "\x5E", "\x5E");
  assert_true_regexp("a\\wc", "abc", "abc");
  assert_true_regexp("a\\wc", "a_c", "a_c");
  assert_true_regexp("a\\wc", "a0c", "a0c");
  assert_false_regexp("a\\wc", "a*c");
  assert_true_regexp("\\w+", "--ab_cd0123--", "ab_cd0123");
  assert_true_regexp("[\\w]+", "--ab_cd0123--", "ab_cd0123");
  assert_true_regexp("\\D+", "1234abc5678", "abc");
  assert_true_regexp("[\\d]+", "0123456789", "0123456789");
  assert_true_regexp("[\\D]+", "1234abc5678", "abc");
  assert_true_regexp("[\\da-fA-F]+", "123abc", "123abc");
  assert_false_regexp("^(ab|cd)e", "abcde");
  assert_true_regexp("(abc|)ef", "abcdef", "ef");
  assert_true_regexp("(abc|)ef", "abcef", "abcef");
  assert_true_regexp("\\babc", "abc", "abc");
  assert_true_regexp("abc\\b", "abc", "abc");
  assert_false_regexp("\\babc", "1abc");
  assert_false_regexp("abc\\b", "abc1");
  assert_true_regexp("abc\\s\\b", "abc x", "abc ");
  assert_false_regexp("abc\\s\\b", "abc  ");
  assert_true_regexp("\\babc\\b", " abc ", "abc");
  assert_true_regexp("\\b\\w\\w\\w\\b", " abc ", "abc");
  assert_true_regexp("\\w\\w\\w\\b", "abcd", "bcd");
  assert_true_regexp("\\b\\w\\w\\w", "abcd", "abc");
  assert_false_regexp("\\b\\w\\w\\w\\b", "abcd");
  assert_false_regexp("\\Babc", "abc");
  assert_false_regexp("abc\\B", "abc");
  assert_true_regexp("\\Babc", "1abc", "abc");
  assert_true_regexp("abc\\B", "abc1", "abc");
  assert_false_regexp("abc\\s\\B", "abc x");
  assert_true_regexp("abc\\s\\B", "abc  ", "abc ");
  assert_true_regexp("\\w\\w\\w\\B", "abcd", "abc");
  assert_true_regexp("\\B\\w\\w\\w", "abcd", "bcd");
  assert_false_regexp("\\B\\w\\w\\w\\B", "abcd");

  // This is allowed in most regexp engines but in order to keep the
  // grammar free of shift/reduce conflicts I've decided not supporting
  // it. Users can use the (abc|) form instead.
  assert_regexp_syntax_error("(|abc)ef");

  assert_true_regexp("((a)(b)c)(d)", "abcd", "abcd");
  assert_true_regexp("(a|b)c*d", "abcd", "bcd");
  assert_true_regexp("(ab|ab*)bc", "abc", "abc");
  assert_true_regexp("a([bc]*)c*", "abc", "abc");
  assert_true_regexp("a([bc]*)c*", "ac", "ac");
  assert_true_regexp("a([bc]*)c*", "a", "a");
  assert_true_regexp("a([bc]*)(c*d)", "abcd", "abcd");
  assert_true_regexp("a([bc]+)(c*d)", "abcd", "abcd");
  assert_true_regexp("a([bc]*)(c+d)", "abcd", "abcd");
  assert_true_regexp("a[bcd]*dcdcde", "adcdcde", "adcdcde");
  assert_false_regexp("a[bcd]+dcdcde", "adcdcde");
  assert_true_regexp("\\((.*), (.*)\\)", "(a, b)", "(a, b)");
  assert_true_regexp("abc|123$", "abcx", "abc");
  assert_false_regexp("abc|123$", "123x");
  assert_true_regexp("abc|^123", "123", "123");
  assert_false_regexp("abc|^123", "x123");
  assert_true_regexp("^abc$", "abc", "abc");
  assert_false_regexp("^abc$", "abcc");
  assert_true_regexp("^abc", "abcc", "abc");
  assert_false_regexp("^abc$", "aabc");
  assert_true_regexp("abc$", "aabc", "abc");
  assert_true_regexp("^a(bc+|b[eh])g|.h$", "abhg", "abhg");
  assert_true_regexp("(bc+d$|ef*g.|h?i(j|k))", "effgz", "effgz");
  assert_true_regexp("(bc+d$|ef*g.|h?i(j|k))", "ij", "ij");
  assert_false_regexp("(bc+d$|ef*g.|h?i(j|k))", "effg");
  assert_false_regexp("(bc+d$|ef*g.|h?i(j|k))", "bcdd");
  assert_true_regexp("(bc+d$|ef*g.|h?i(j|k))", "reffgz", "effgz");

  // Test case for issue #324
  assert_true_regexp("whatever|   x.   x", "   xy   x", "   xy   x");
}