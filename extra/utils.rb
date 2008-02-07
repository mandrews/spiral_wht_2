#!/usr/bin/env ruby

# filename
#
# Author: Michael Andrews <mjand@drexel.edu>
# $Id$

require 'json'

INF = 1.0 / 0

def load_runtime_env(measure)
  cmd = "#{measure} -v"
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

