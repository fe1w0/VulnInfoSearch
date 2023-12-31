require File.join(File.expand_path(File.dirname(__FILE__)), 'test_utils')

include TZInfo

# Use send as a workaround for erroneous 'wrong number of arguments' errors with
# JRuby 9.0.5.0 when calling methods with Java implementations. See #114.
send(:using, TaintExt) if Module.const_defined?(:TaintExt)

class TCRubyDataSource < Minitest::Test
  def setup
    @data_source = RubyDataSource.new
  end
  
  def test_initialize_not_found
    # A failure to load tzinfo/data in initialize will not cause an exception.
    # Attempts to load data files will subsequently fail.
    code = <<-EOF
      begin
        ds = TZInfo::RubyDataSource.new
        puts 'Initialized'
        ds.load_timezone_info('Europe/London')
      rescue Exception => e
        puts e.class
      end
    EOF

    assert_sub_process_returns(['Initialized', 'TZInfo::InvalidTimezoneIdentifier'], code)
  end

  def test_load_timezone_info_data
    info = @data_source.load_timezone_info('Europe/London')
    assert_kind_of(DataTimezoneInfo, info)
    assert_equal('Europe/London', info.identifier)
  end
  
  def test_load_timezone_info_linked
    info = @data_source.load_timezone_info('UTC')
    assert_kind_of(LinkedTimezoneInfo, info)
    assert_equal('UTC', info.identifier)
    assert_equal('Etc/UTC', info.link_to_identifier)
  end
  
  def test_load_timezone_info_does_not_exist
    assert_raises(InvalidTimezoneIdentifier) do
      @data_source.load_timezone_info('Nowhere/Special')
    end
  end
  
  def test_load_timezone_info_invalid
    assert_raises(InvalidTimezoneIdentifier) do
      @data_source.load_timezone_info('../Definitions/UTC')
    end
  end

  def test_load_timezone_info_directory_traversal
    test_data_depth = TZINFO_TEST_DATA_DIR.scan('/').size
    payload_path = File.join(TESTS_DIR, 'assets', 'payload')
    assert_raises(InvalidTimezoneIdentifier) { Timezone.get("foo\n#{'/..' * (test_data_depth + 4)}#{payload_path}") }
  end
  
  def test_load_timezone_info_nil
    assert_raises(InvalidTimezoneIdentifier) do
      @data_source.load_timezone_info(nil)
    end
  end
  
  def test_load_timezone_info_case
    assert_raises(InvalidTimezoneIdentifier) do
      @data_source.load_timezone_info('europe/london')
    end
  end

  def test_load_timezone_info_plus
    info = @data_source.load_timezone_info('Etc/GMT+1')
    assert_equal('Etc/GMT+1', info.identifier)
  end
  
  def test_load_timezone_info_minus
    info = @data_source.load_timezone_info('Etc/GMT-1')
    assert_equal('Etc/GMT-1', info.identifier)
  end

  def test_load_timezone_info_tainted
    skip_if_has_bug_14060

    safe_test(:unavailable => :skip) do
      identifier = 'Europe/Amsterdam'.dup.taint
      assert(identifier.tainted?)
      info = @data_source.load_timezone_info(identifier)
      assert_equal('Europe/Amsterdam', info.identifier)
      assert(identifier.tainted?)
    end
  end
  
  def test_load_timezone_info_tainted_and_frozen
    skip_if_has_bug_14060

    safe_test do
      info = @data_source.load_timezone_info('Europe/Amsterdam'.dup.taint.freeze)
      assert_equal('Europe/Amsterdam', info.identifier)
    end
  end
  
  def test_timezone_identifiers
    all = @data_source.timezone_identifiers
    assert_equal(TZInfo::Data::Indexes::Timezones.timezones, all)
    assert_equal(true, all.frozen?)
  end
  
  def test_data_timezone_identifiers
    all_data = @data_source.data_timezone_identifiers
    assert_equal(TZInfo::Data::Indexes::Timezones.data_timezones, all_data)
    assert_equal(true, all_data.frozen?)
  end
  
  def test_linked_timezone_identifiers
    all_linked = @data_source.linked_timezone_identifiers
    assert_equal(TZInfo::Data::Indexes::Timezones.linked_timezones, all_linked)
    assert_equal(true, all_linked.frozen?)
  end
  
  def test_load_country_info
    info = @data_source.load_country_info('GB')
    assert_equal('GB', info.code)
  end
    
  def test_load_country_info_not_exist
    assert_raises(InvalidCountryCode) do
      @data_source.load_country_info('ZZ')
    end
  end
  
  def test_load_country_info_invalid
    assert_raises(InvalidCountryCode) do
      @data_source.load_country_info('../Countries/GB')
    end
  end
  
  def test_load_country_info_nil
    assert_raises(InvalidCountryCode) do
      @data_source.load_country_info(nil)
    end
  end
  
  def test_load_country_info_case
    assert_raises(InvalidCountryCode) do
      @data_source.load_country_info('gb')
    end
  end
  
  def test_load_country_info_tainted
    safe_test(:unavailable => :skip) do
      code = 'NL'.dup.taint
      assert(code.tainted?)
      info = @data_source.load_country_info(code)
      assert_equal('NL', info.code)
      assert(code.tainted?)
    end
  end
  
  def test_load_country_info_tainted_and_frozen
    safe_test do
      info = @data_source.load_country_info('NL'.dup.taint.freeze)
      assert_equal('NL', info.code)
    end
  end
  
  def test_country_codes
    codes = @data_source.country_codes
    assert_equal(TZInfo::Data::Indexes::Countries.countries.keys, codes)
    assert_equal(true, codes.frozen?)
  end
end
