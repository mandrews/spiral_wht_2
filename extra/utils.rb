#!/usr/bin/env ruby

# filename
#
# Author: Michael Andrews <mjand@drexel.edu>
# $Id$

require 'json'
require 'facets/dictionary' # Ordered Hash

INF       = 1.0 / 0

BASENAME  = File.dirname(__FILE__) 

PATH      = [ 'extra', 'bin', 'lib' ]  +                    # Install Path
            [ 'wht', 'measure', 'model/ic', 'wht/.libs' ]   # Development Path

def find_file(file)
  PATH.each do |path|
    full_path = "./#{BASENAME}/../#{path}/#{file}"

    return full_path if File.exists?(full_path)
  end

  raise Exeception.new("Could not find #{file}")
end


COUNT     = find_file("wht_count")
MEASURE   = find_file("wht_measure")
ATTACH    = find_file("wht_attach")


def load_runtime_env
  cmd = "#{MEASURE} -v"
  env = {}
  IO.popen(cmd) do |fd|
    while (line = fd.gets) do
      line.chomp!
      case line
        when /^max_unroll:\s*(\d+)/
          env['max_unroll'] = $1.to_i
        when /^max_interleave:\s*(\d+)/
          env['max_interleave'] = $1.to_i
        when /^vector_size:\s*(\d+)/
          env['vector_size'] = $1.to_i
        when /^CC:\s*(.*)$/
          env['compiler'] = $1.to_s
        when /^type:\s*(.*)$/
          env['type'] = $1.to_s
        when /^CFLAGS:\s*(.*)$/
          env['compiler_flags'] = $1.to_s
      end
    end
  end
  env
end

def load_data(fd)
  begin
    json  = JSON.load(fd)
    raise "Initializing #{file}" unless json
    return json
  rescue => e 
    puts e.message
  end
end

def save_data(fd, data)
  fd.puts(JSON.pretty_generate(data))
end

def time(&blk)
  t0 = Time.now
  yield
  t1 = Time.now
  $stderr.puts t1 - t0
end

class Options
  require 'optparse'

  attr_reader :params

  def initialize
    @params  = {} 
    @options = OptionParser.new 
  end

  def die(msg = nil)
    puts @options.to_s 
    puts "\n#{msg}" unless msg.nil?
    exit
  end

  def parse(args)
    begin
      @options.parse!(args)
    rescue => e
      die(e.message)
    end
  end
end

def ruby2unix(cmd, args, *flags)
  args.each do |k,v|
    cmd << " #{k} #{v} "
  end

  flags.each do |f|
    cmd << " #{f}"
  end

  cmd
end

def unix2string(cmd, args, *flags)
  cmd = ruby2unix(cmd, args, flags)

  out = 0
  IO.popen(cmd) do |fd|
    out = fd.gets
  end

  raise Exception.new("Could not read exec") if out.nil? 

  out.to_s.chomp
end


def unix2float(cmd, args, *flags)
  cmd = ruby2unix(cmd, args, flags)

  out = 0
  IO.popen(cmd) do |fd|
    out = fd.gets
  end

  raise Exception.new("Could not read exec") if out.nil? or out.to_f.zero?

  out.to_f
end

def unix2hash(delim, cmd, args, *flags)
  cmd = ruby2unix(cmd, args, flags)

  hash = Dictionary.new
  IO.popen(cmd) do |fd|
    while out = fd.gets
      out.chomp!
      k,v = out.split(delim)
      hash[k] = v
    end
  end

  raise Exception.new("Could not read exec") if hash.nil? or hash.keys.empty?
  hash
end

def vet(wht)
  new = unix2string("#{ATTACH}", { "-w" => wht })

  raise Exception.new("Could not parse #{wht}.") unless "'#{new}'" == wht
end

def measure(args, *flags)
  vet(args["-w"])

  unix2float("#{MEASURE}", args, flags)
end

def count(args, *flags)
  vet(args["-w"])

  hash = unix2hash(/\s*:\s*/, "#{COUNT}", args, flags)
  hash.each do |k,v|
    hash[k] = v.to_f
  end
  hash
end

