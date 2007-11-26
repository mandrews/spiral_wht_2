#!/usr/bin/env ruby

# filename
#
# Author: Michael Andrews <mjand@drexel.edu>
# $Id$

require 'yaml'

MEASURE_PATH    = '../measure'

def load_runtime_env
  cmd = "#{MEASURE_PATH}/wht_measure -v"
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

def load_entry(env,info)
  info.each do |e|
    return e if e['env'] == env
  end

  # Add a new entry
  entry = {
    'env'   => env,
    'data'  => {}
  }

  info << entry

  entry
end

def load_data(file, env)
  default = [ [], YAML::Omap.new ] # i.e. empty info and plans

  return default unless File.exists?(file)

  info = []

  File.open(file,'r') do |fd|
    begin
      yml  = YAML::load_stream(fd)
      raise "Initializing #{file}" unless yml
      info = yml.documents[0]
    rescue => e 
      puts e.message
      return default
    end
  end

  plans = load_entry(env, info)['data']

  plans
end

