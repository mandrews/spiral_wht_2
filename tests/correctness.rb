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

  if out =~ /^correct/
    puts "PASS[A] #{plan}"
  else
    puts "FAIL[R] #{plan}"
    puts "!!!"
    exit(1)
  end
end

def expect_reject(plan)
  out = `#{WHT_VERIFY} -w '#{plan}'`

  if out =~ /^reject/
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

  if out =~ /^error/
    puts "PASS[R] #{plan}"
  else
    puts "FAIL[A] #{plan}"
    puts "!!!"
    exit(1)
  end
end

def split(*a)
  "split[" + a.join(',') + "]"
end

def splitil(*a)
  "splitil[" + a.join(',') + "]"
end

def small(s)
  ["small[#{s}]"]
end

def smallv(s, *v)
  ["smallv(#{v.join(',')})[#{s}]"]
end

def smallil(s, k)
  ["smallil(#{k})[#{s}]"]
end

if $0 == __FILE__ # Main Entry Point
  env = load_runtime_env

  n = env['max_unroll']

  puts "Default Tests"
  puts

  for size in 1 .. n do
    expect_correct(small(size))
  end

  for size in n+1 .. 2*n do
    expect_reject(small(size))
  end

  for size in 1 .. n do
    expect_correct(split(small(size),small(1)))
  end

  v = env['vector_size']

  if v > 0
    puts "\nVectorization Tests (1)"
    puts

    for size in v .. n do
      expect_correct(smallv(size,v))
    end

    for size in v .. n do
      expect_reject(split(smallv(size,v)*2))
    end

    for size in v .. n do
      expect_correct(split(split(small(1)*4),smallv(size,v)))
    end

    for size in v .. n do
      expect_reject(split(split(small(1)*3),smallv(size,v),small(1)))
    end
  end 

  i = env['max_interleave']

  if i > 0
    puts "\nInterleave Tests (1)"
    puts

    for x in 1 .. i do
      y = 2**x
      expect_correct(splitil(smallil(1,y)*1, small(n)))
      expect_correct(splitil(smallil(1,y)*2, small(n)))
    end

    # Need to use splitil with smallil
    for x in 1 .. i do
      y = 2**x
      expect_reject(split(smallil(1,y)*1, small(n)))
      expect_reject(split(smallil(1,y)*2, small(n)))
    end

    for x in i+1 .. 2*i do
      y = 2**x
      expect_reject(splitil(smallil(1,y)*1, small(n)))
    end

    for x in 2 .. i do
      y = 2**x
      expect_reject(splitil(smallil(1,y)*1, small(1)))
    end

    for x in 2 .. i do
      y = 2**x
      expect_reject(smallil(n,y))
    end
  end 

  if v > 0 and i > 0
    puts "\nVectorization Tests (2)"
    puts

    # Vary interleave factor fix size
    for size in v .. n do
      expect_correct(splitil(smallv(1,v,v,1)*4,smallv(size,v)))
      expect_correct(splitil(smallv(1,v,v,0)*4,smallv(size,v)))
    end

    for size in v .. n do
      expect_correct(splitil(smallv(1,v,v,1), splitil(smallv(1,v,v,1),smallv(size,v))))
      expect_correct(splitil(smallv(1,v,v,0), splitil(smallv(1,v,v,0),smallv(size,v))))
    end

    min_il = i

    for size in v .. n do
      # Reject aligned vectors here
      expect_reject(splitil(
        splitil(smallv(1,v,v,1), small(min_il)),
        splitil(smallv(1,v,v,1), smallv(min_il,v))))

      # Accept general vectors here
      expect_correct(splitil(
        splitil(smallv(1,v,v,0), small(min_il)),
        splitil(smallv(1,v,v,0), smallv(min_il,v))))
    end

    # Reject when interleaved codelets do not have splitil parent
    for size in v .. n do
      expect_reject(split(smallv(1,v,v,0)*4,smallv(size,v)))
      expect_reject(split(smallv(1,v,v,1)*4,smallv(size,v)))
    end

    # Reject when interleaved codelets do not have parent
    for x in 2 .. i do
      y = 2**x
      expect_reject(smallv(n,v,y,0))
      expect_reject(smallv(n,v,y,1))
    end

    for size in v .. n do
      expect_correct(splitil(smallv(size,v,8,0),smallv(min_il,v)))
    end

  end

  exit(0)

end
