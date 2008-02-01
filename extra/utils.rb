#!/usr/bin/env ruby

# filename
#
# Author: Michael Andrews <mjand@drexel.edu>
# $Id$

require 'json'

UTILS_PATH      = File.dirname(__FILE__) 
#MEASURE_PATH    = "#{UTILS_PATH}/../bin"
MEASURE_PATH    = "../measure"
#RANDTREE_PATH   = "#{UTILS_PATH}/../bin"
RANDTREE_PATH   = "../rand"
CLASSIFY_PATH   = "../wht"
ATTACH_PATH     = "../wht"
COUNT_SSE_PATH  = "../model/ic"

INF = 1.0 / 0

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

def load_data(file, default)
  return default unless File.exists?(file)

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

def save_data(file, data)
  File.open(file,'w+') do |fd|
    fd.puts(JSON.pretty_generate(data))
  end
end

def time(&blk)
  t0 = Time.now
  yield
  t1 = Time.now
  puts t1 - t0
end

def wht_measure(plan, *args)
  cmd = "#{MEASURE_PATH}/wht_measure -s -w '#{plan}' #{args}"
  puts "Executing #{cmd}" if @debug
  t = 0
  IO.popen(cmd) do |fd|
    t = fd.gets
  end

  if t.nil?
    puts("Could not read: #{cmd}") if @debug
    return INF
  end

  a = t.split(/\s+/)
  return [ 
    a[0].to_f,
    a[1].to_f,
    a[2].to_i
  ]
end

def wht_classify(plan, *args)
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

def wht_attach(plan, rules)
  return plan if rules.empty?
  dup = rules.dup

  r0  = dup.shift
  cmd = "#{ATTACH_PATH}/wht_attach -w '#{plan}' -r '#{r0}' "
  dup.each do |ri|
    cmd += "| #{ATTACH_PATH}/wht_attach -r '#{ri}' "
  end

  puts "Executing #{cmd}" if @debug
  t = 0
  IO.popen(cmd) do |fd|
    t = fd.gets
  end

  unless t 
    puts("Could not read: #{cmd}") if @debug
    return plan
  else
    return t.chomp
  end
end

def wht_rand(size,a,b,p,q,*args)
  `#{RANDTREE_PATH}/wht_rand -n #{size} -a #{a} -b #{b} -p #{p} -q #{q} #{args}`.chomp
end

def count_sse_sh(plan)
  h = {}
  ['shuffle', 'unpack', 'scalar_add', 'vector_add', 'scalar_mov', 'vector_mov'].each do |mode|
    cmd = "bash #{COUNT_SSE_PATH}/count_sse.sh '#{plan}' #{mode}"
    puts "Executing #{cmd}"  if @debug
    t = 0
    IO.popen(cmd) do |fd|
      t = fd.readlines
    end

    h[mode] = t.first.to_i
  end
  return h
end

