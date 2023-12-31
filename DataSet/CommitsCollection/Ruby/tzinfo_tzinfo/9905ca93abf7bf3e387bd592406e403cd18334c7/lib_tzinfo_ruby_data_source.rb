module TZInfo
  # Use send as a workaround for erroneous 'wrong number of arguments' errors
  # with JRuby 9.0.5.0 when calling methods with Java implementations. See #114.
  send(:using, RubyCoreSupport::UntaintExt) if RubyCoreSupport.const_defined?(:UntaintExt)

  # A DataSource that loads data from the set of Ruby modules included in the
  # TZInfo::Data library (tzinfo-data gem).
  #
  # To have TZInfo use this DataSource, call TZInfo::DataSource.set as follows:
  #
  #   TZInfo::DataSource.set(:ruby)
  class RubyDataSource < DataSource
    # Whether the timezone index has been loaded yet.
    @@timezone_index_loaded = false
    
    # Whether the country index has been loaded yet.
    @@country_index_loaded = false
  
    # Initializes a new RubyDataSource instance.
    def initialize
      tzinfo_data = File.join('tzinfo', 'data')
      begin
        require(tzinfo_data)

        data_file = File.join('', 'tzinfo', 'data.rb')
        path = $".reverse_each.detect {|p| p.end_with?(data_file) }
        if path
          @base_path = File.join(File.dirname(path), 'data').untaint
        else
          @base_path = tzinfo_data
        end
      rescue LoadError
        @base_path = tzinfo_data
      end
    end

    # Returns a TimezoneInfo instance for a given identifier. 
    # Raises InvalidTimezoneIdentifier if the timezone is not found or the 
    # identifier is invalid.
    def load_timezone_info(identifier)
      raise InvalidTimezoneIdentifier, 'Invalid identifier' if identifier !~ /\A[A-Za-z0-9\+\-_]+(\/[A-Za-z0-9\+\-_]+)*\z/
      
      identifier = identifier.gsub(/-/, '__m__').gsub(/\+/, '__p__')
      
      # Untaint identifier after it has been reassigned to a new string. We
      # don't want to modify the original identifier. identifier may also be 
      # frozen and therefore cannot be untainted.
      identifier.untaint
      
      identifier = identifier.split('/')
      begin
        require_definition(identifier)
        
        m = Data::Definitions
        identifier.each {|part|
          m = m.const_get(part)
        }
        
        m.get
      rescue LoadError, NameError => e
        raise InvalidTimezoneIdentifier, e.message
      end
    end
    
    # Returns an array of all the available timezone identifiers.
    def timezone_identifiers
      load_timezone_index
      Data::Indexes::Timezones.timezones
    end
    
    # Returns an array of all the available timezone identifiers for
    # data timezones (i.e. those that actually contain definitions).
    def data_timezone_identifiers
      load_timezone_index
      Data::Indexes::Timezones.data_timezones
    end
    
    # Returns an array of all the available timezone identifiers that
    # are links to other timezones.
    def linked_timezone_identifiers
      load_timezone_index
      Data::Indexes::Timezones.linked_timezones
    end
    
    # Returns a CountryInfo instance for the given ISO 3166-1 alpha-2
    # country code. Raises InvalidCountryCode if the country could not be found
    # or the code is invalid.
    def load_country_info(code)
      load_country_index
      info = Data::Indexes::Countries.countries[code]
      raise InvalidCountryCode, 'Invalid country code' unless info
      info
    end
    
    # Returns an array of all the available ISO 3166-1 alpha-2
    # country codes.
    def country_codes
      load_country_index
      Data::Indexes::Countries.countries.keys.freeze
    end
    
    # Returns the name of this DataSource.
    def to_s
      "Ruby DataSource"
    end
    
    private
    
    # Requires a zone definition by its identifier (split on /).
    def require_definition(identifier)
      require_data(*(['definitions'] + identifier))
    end
    
    # Requires an index by its name.
    def require_index(name)
      require_data(*['indexes', name])
    end
    
    # Requires a file from tzinfo/data.
    def require_data(*file)
      require(File.join(@base_path, *file))
    end
    
    # Loads in the index of timezones if it hasn't already been loaded.
    def load_timezone_index
      unless @@timezone_index_loaded
        require_index('timezones')
        @@timezone_index_loaded = true
      end        
    end
    
    # Loads in the index of countries if it hasn't already been loaded.
    def load_country_index
      unless @@country_index_loaded
        require_index('countries')
        @@country_index_loaded = true
      end
    end
  end
end
