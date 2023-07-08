# encoding: utf-8
require "digest/md5"
require "socket"

module BSON

  # Represents object_id data.
  #
  # @see http://bsonspec.org/#/specification
  #
  # @since 2.0.0
  class ObjectId
    include Comparable

    # A object_id is type 0x07 in the BSON spec.
    #
    # @since 2.0.0
    BSON_TYPE = 7.chr.force_encoding(BINARY).freeze

    # Check equality of the object id with another object.
    #
    # @example Check if the object id is equal to the other.
    #   object_id == other
    #
    # @param [ Object ] other The object to check against.
    #
    # @return [ true, false ] If the objects are equal.
    #
    # @since 2.0.0
    def ==(other)
      return false unless other.is_a?(ObjectId)
      to_bson == other.to_bson
    end

    # Check case equality on the object id.
    #
    # @example Check case equality.
    #   object_id === other
    #
    # @param [ Object ] other The object to check against.
    #
    # @return [ true, false ] If the objects are equal in a case.
    #
    # @since 2.0.0
    def ===(other)
      return to_str === other.to_str if other.respond_to?(:to_str)
      super
    end

    # Compare this object id with another object for use in sorting.
    #
    # @example Compare the object id with the other object.
    #   object <=> other
    #
    # @param [ Object ] other The object to compare to.
    #
    # @return [ Integer ] The result of the comparison.
    #
    # @since 2.0.0
    def <=>(other)
      to_bson <=> other.to_bson
    end

    # Return the UTC time at which this ObjectId was generated. This may
    # be used instread of a created_at timestamp since this information
    # is always encoded in the object id.
    #
    # @example Get the generation time.
    #   object_id.generation_time
    #
    # @return [ Time ] The time the id was generated.
    #
    # @since 2.0.0
    def generation_time
      ::Time.at(to_bson.unpack("N")[0]).utc
    end

    # Get the object id as it's raw BSON data.
    #
    # @example Get the raw bson bytes.
    #   object_id.to_bson
    #
    # @note Since Moped's BSON and 10gen BSON before 2.0.0 have different
    #   internal representations, we will attempt to repair the data for cases
    #   where the object was instantiated in a non-standard way. (Like a
    #   Marshal.load)
    #
    # @return [ String ] The raw bytes.
    #
    # @see http://bsonspec.org/#/specification
    #
    # @since 2.0.0
    def to_bson
      repair!(@data) if defined?(@data)
      @raw_data ||= @@generator.next
    end

    # Get the string representation of the object id.
    #
    # @example Get the object id as a string.
    #   object_id.to_s
    #
    # @return [ String ] The object id as a string.
    #
    # @since 2.0.0
    def to_s
      to_bson.unpack("H*")[0].force_encoding(UTF8)
    end
    alias :to_str :to_s

    # Raised when trying to create an object id with invalid data.
    #
    # @since 2.0.0
    class Invalid < RuntimeError; end

    private

    def data=(data)
      @raw_data = data
    end

    class << self

      # Create a new object id from raw bytes.
      #
      # @example Create an object id from raw bytes.
      #   BSON::ObjectId.from_data(data)
      #
      # @param [ String ] data The raw bytes.
      #
      # @return [ ObjectId ] The new object id.
      #
      # @since 2.0.0
      def from_data(data)
        object_id = allocate
        object_id.send(:data=, data)
        object_id
      end

      # Create a new object id from a string.
      #
      # @example Create an object id from the string.
      #   BSON::ObjectId.from_string(id)
      #
      # @param [ String ] string The string to create the id from.
      #
      # @raise [ BSON::ObjectId::Invalid ] If the provided string is invalid.
      #
      # @return [ BSON::ObjectId ] The new object id.
      #
      # @since 2.0.0
      def from_string(string)
        unless legal?(string)
          raise Invalid.new("'#{string}' is an invalid ObjectId.")
        end
        from_data([ string ].pack("H*"))
      end

      # Create a new object id from a time.
      #
      # @example Create an object id from a time.
      #   BSON::ObjectId.from_id(time)
      #
      # @example Create an object id from a time, ensuring uniqueness.
      #   BSON::ObjectId.from_id(time, unique: true)
      #
      # @param [ Time ] time The time to generate from.
      # @param [ Hash ] options The options.
      #
      # @option options [ true, false ] :unique Whether the id should be
      #   unique.
      #
      # @return [ ObjectId ] The new object id.
      #
      # @since 2.0.0
      def from_time(time, options = {})
        from_data(options[:unique] ? @@generator.next(time.to_i) : [ time.to_i ].pack("Nx8"))
      end

      # Determine if the provided string is a legal object id.
      #
      # @example Is the string a legal object id?
      #   BSON::ObjectId.legal?(string)
      #
      # @param [ String ] The string to check.
      #
      # @return [ true, false ] If the string is legal.
      #
      # @since 2.0.0
      def legal?(string)
        /\A\h{24}\Z/ === string.to_s
      end
    end

    # Inner class that encapsulates the behaviour of actually generating each
    # part of the ObjectId.
    #
    # @api private
    #
    # @since 2.0.0
    class Generator

      attr_reader :machine_id

      # Instantiate the new object id generator. Will set the machine id once
      # on the initial instantiation.
      #
      # @example Instantiate the generator.
      #   BSON::ObjectId::Generator.new
      #
      # @since 2.0.0
      def initialize
        @counter = 0
        @machine_id = Digest::MD5.digest(Socket.gethostname).unpack("N")[0]
        @mutex = Mutex.new
      end

      # Return object id data based on the current time, incrementing the
      # object id counter. Will use the provided time if not nil.
      #
      # @example Get the next object id data.
      #   generator.next
      #
      # @param [ Time ] time The optional time to generate with.
      #
      # @return [ String ] The raw object id bytes.
      #
      # @since 2.0.0
      def next(time = nil)
        @mutex.lock
        begin
          count = @counter = (@counter + 1) % 0xFFFFFF
        ensure
          @mutex.unlock rescue nil
        end
        generate(time || ::Time.new.to_i, count)
      end

      # Generate object id data for a given time using the provided counter.
      #
      # @example Generate the object id bytes.
      #   generator.generate(time)
      #
      # @param [ Integer ] time The time since epoch in seconds.
      # @param [ Integer ] counter The optional counter.
      #
      # @return [ String ] The raw object id bytes.
      #
      # @since 2.0.0
      def generate(time, counter = 0)
        process_thread_id = "#{Process.pid}#{Thread.current.object_id}".hash % 0xFFFF
        [ time, machine_id, process_thread_id, counter << 8 ].pack("N NX lXX NX")
      end
    end

    # We keep one global generator for object ids.
    #
    # @since 2.0.0
    @@generator = Generator.new

    # Register this type when the module is loaded.
    #
    # @since 2.0.0
    Registry.register(BSON_TYPE, self)
  end
end
