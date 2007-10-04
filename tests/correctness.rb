#!/usr/bin/env ruby

# correctness.rb
#
# Author: Michael Andrews <mjand@drexel.edu>
# $Id$

BUILDDIR   = ENV['BUILDDIR'] || File.dirname(__FILE__) + '../'
WHT_VERIFY = "#{BUILDDIR}/wht/wht_verify"

def load_runtime_env
  cmd = "#{WHT_VERIFY} -v"
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

def expect_correct(plan)
  out = `#{WHT_VERIFY} -w '#{plan}'`

  if out =~ /correct/
    puts "PASS[A] #{plan}"
  else
    puts "FAIL[R] #{plan}"
    puts "!!!"
    exit(1)
  end
end

def expect_reject(plan)
  out = `#{WHT_VERIFY} -w '#{plan}'`

  if out =~ /reject/
    puts "PASS[R] #{plan}"
  else
    puts "FAIL[A] #{plan}"
    puts "!!!"
    exit(1)
  end
end

def expect_error(plan)
  # Pipe stderr to stdout
  out = `#{WHT_VERIFY} -w '#{plan}' 2>&1 `

  if out =~ /error/
    puts "PASS[R] #{plan}"
  else
    puts "FAIL[A] #{plan}"
    puts "!!!"
    exit(1)
  end
end

def split(plan, k, extra = nil) 
  a = []
  k.times { a << plan }
  a << extra if extra
  "split[" + a.join(',') + "]"
end

if $0 == __FILE__ # Main Entry Point
  env = load_runtime_env

  n = env['max_unroll']

  puts "Default Tests"
  puts

  for size in 1 .. n do
    expect_correct("small[#{size}]")
  end

  for size in n+1 .. 2*n do
    expect_error("small[#{size}]")
  end

  for size in 1 .. n do
    expect_correct(split("small[#{size}]",2))
  end

  v = env['vector_size']

  if v > 0

  puts "\nVectorization Tests"
  puts

  for size in v .. n do
    expect_correct("smallv(#{v})[#{size}]")
  end

  for size in v .. n do
    expect_reject(split("smallv(#{v})[#{size}]",2))
  end

  for size in v .. n do
    expect_correct("split[" + split("small[1]",4) + ",smallv(#{v})[#{size}]]")
  end

  for size in v .. n do
    expect_reject("split[" + split("small[1]",3, "smallv(#{v})[#{size}]") + ", small[1]]")
  end

  end 

  i = env['max_interleave']

  if i > 0

  puts "\nInterleave Tests"
  puts

  for x in 1 .. i do
    y = 2**x
    expect_correct(split("smallil(#{y})[1]",1,"small[#{n}]"))
    expect_correct(split("smallil(#{y})[1]",2,"small[#{n}]"))
  end

  for x in i+1 .. 2*i do
    y = 2**x
    expect_error(split("smallil(#{y})[1]",1,"small[#{n}]"))
  end

  for x in 2 .. i do
    y = 2**x
    expect_reject(split("smallil(#{y})[1]",1,"small[1]"))
  end

  end 

  exit(0)

end
