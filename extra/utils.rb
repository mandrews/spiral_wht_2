#!/usr/bin/env ruby

# filename
#
# Author: Michael Andrews <mjand@drexel.edu>
# $Id$

require 'json/ext'

SCRIPT_PATH     = File.dirname(__FILE__) 
MEASURE_PATH    = "#{SCRIPT_PATH}/../measure"
RANDTREE_PATH   = "#{SCRIPT_PATH}/../utils"
CONVERT_PATH    = "#{SCRIPT_PATH}/../wht"
CLASSIFY_PATH   = "#{SCRIPT_PATH}/../wht"
IC_PATH         = "#{SCRIPT_PATH}/../model"

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

def load_json(file, default)
  File.open(file,'r') do |fd|
    begin
      json  = JSON.load(fd)
      raise "Initializing #{file}" unless json
      return json
    rescue => e 
      puts e.message
      return default
    end
  end
end

def load_data(file, env)
  default = [[],{}]

  return default unless File.exists?(file)

  info = load_json(file, [])

  plans = load_entry(env, info)['data']

  [info, plans]
end

def save_data(file, env, plans, info)
  load_entry(env,info)['data'] = plans

  File.open(file,'w+') do |fd|
    fd.puts(JSON.pretty_generate(info))
  end
end

def measure(plan, *args)
  cmd = "#{MEASURE_PATH}/wht_measure -w '#{plan}' #{args}"
  puts "Executing #{cmd}" if @debug
  t = 0
  IO.popen(cmd) do |fd|
    t = fd.gets
  end

  unless t 
    puts("Could not read: #{cmd}") if @debug
    return INF
  else
    t.to_f
  end
end

