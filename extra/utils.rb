#!/usr/bin/env ruby

# filename
#
# Author: Michael Andrews <mjand@drexel.edu>
# $Id$

require 'json'

UTILS_PATH      = File.dirname(__FILE__) 
MEASURE_PATH    = "#{UTILS_PATH}/../measure"
RANDTREE_PATH   = "#{UTILS_PATH}/../utils"
CONVERT_PATH    = "#{UTILS_PATH}/../wht"
CLASSIFY_PATH   = "#{UTILS_PATH}/../wht"
IC_PATH         = "#{UTILS_PATH}/../model/ic"

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
  load_entry(env,info)['data'] = plans.sort { |a,b| a[0].to_i <=> b[0].to_i }

  File.open(file,'w+') do |fd|
    fd.puts(JSON.pretty_unparse(info))
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

def classify_exec(plan, *args)
  cmd = "#{CLASSIFY_PATH}/wht_classify -w '#{plan}' #{args}"
  puts "Executing #{cmd}" if @debug
  t = 0
  IO.popen(cmd) do |fd|
    t = fd.readlines
  end

  h = {}
  t.each do |l|
    k,v = l.split(/\s*:\s*/)
    h[k] = v.chomp
  end

  if h.empty?
    puts("Could not read: #{cmd}") if @debug
  end

  return h
end


def count_sse_exec(plan)
  h = {}
  ['shuffle', 'unpack', 'scalar_add', 'vector_add', 'scalar_mov', 'vector_mov'].each do |mode|
    cmd = "bash #{IC_PATH}/count_sse.sh '#{plan}' #{mode}"
    puts "Executing #{cmd}"  if @debug
    t = 0
    IO.popen(cmd) do |fd|
      t = fd.readlines
    end

    h[mode] = t.first.to_i
  end
  return h
end

