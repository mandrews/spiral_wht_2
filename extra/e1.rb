#!/usr/bin/env ruby

# e1
#
# Author: Michael Andrews <mjand@drexel.edu>
# $Id$

MEASURE_PATH    = '../measure'
RANDTREE_PATH   = '../utils'
CONVERT_PATH    = '../wht'
CLASSIFY_PATH   = '../wht'
IC_PATH         = '../model'

#MAX = 5000
MAX = 5

METRICS = [ 'TOT_CYC', 'TOT_INS', 'L1_DCM' ]

PARAMS = {
  'TOT_CYC' => '-a 0.05 -p 1 -n 50',
  'L1_DCM'  => '-a 0.05 -p 0.05 -n 50',
#  'L2_TCM'  => '-a 0.05 -p 0.05 -n 50'
}

SIZE = 9

require 'yaml'

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
  default = [ [], YAML::Omap.new ] # i.e. empty info and trees

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

  trees = load_entry(env, info)['data']

  [info, trees]
end

def save_data(file, env, trees, info)
  load_entry(env,info)['data'] = trees

  File.open(file,'w+') do |fd|
    YAML::dump(info, fd)
  end
end

def generate(model, n)
  return if model.size >= MAX

  a,b,p,q = 1,4,1,4

  until model.size >= MAX
    plan = random(n,a,b,p,q)
    type = classify(plan)
    m = model.size
    model[[plan,m]] = { 'type' => type }
  end

end

TRANSFORMS = {
  "vectorize(2,2)" => "-v 2 -i 2",
  "vectorize(2,4)" => "-v 2 -i 4",
  "vectorize(2,8)" => "-v 2 -i 8",
  "interleave(2)"  => "-i 2",
  "interleave(4)"  => "-i 4",
  "interleave(8)"  => "-i 8"
}

def transform(model)
  return unless model.size == MAX

  model2 = model

  model.each do |key,node|
    plan,m = key
    TRANSFORMS.each do |name, args|
      new_plan = convert(plan, args)
      model2[[new_plan,m]] = {
        'type' => node['type'],
        'transform' => name
      }
    end
  end
  model2
end

def measure(plan, metric, args)
  values = `#{MEASURE_PATH}/wht_measure -w '#{plan}' -e PAPI -m #{metric} -s #{args}`.split
  values.map { |x| x.to_f }
end

def random(size,a,b,p,q)
  `#{RANDTREE_PATH}/wht_randtree -n #{size} -a #{a} -b #{b} -p #{p} -q #{q}`.chomp
end

def convert(plan, args)
  `#{CONVERT_PATH}/wht_convert -w '#{plan}' #{args}`.chomp
end

def classify(plan, args = nil)
  `#{CLASSIFY_PATH}/wht_classify -w '#{plan}' #{args}`.chomp
end

def test(model)
  model.each do |key, node|
    plan, m = key
    #puts "Working on #{plan} ..."

    METRICS.each do |metric|
      next if node.has_key?(metric + "_mean")

      value = measure(plan, metric, PARAMS[metric])
      node[metric + "_mean" ]    = value[0]
      node[metric + "_stdev" ]   = value[1]
      node[metric + "_samples" ] = value[2].to_i

      print '.'
      $stdout.flush
    end

    node['updated_on'] = DateTime.now.to_s
  end

  puts "\ndone"
end

def ic(model)
  def open_pipe
    close_pipe
    puts "exec #{IC_PATH}/ic"
    @pipe = IO.popen("#{IC_PATH}/ic", mode = 'r+')
  end

  def close_pipe
    @pipe.close unless @pipe.nil? or @pipe.closed?
  end

  @pipe = open_pipe

  begin
    model.each do |key, node|
      next if node.has_key?('IC_Model')
      plan, m = key
      @pipe.puts(plan + "\n")
      @pipe.flush
      value = @pipe.gets
      node['IC_Model'] = value.chomp.to_i
    end
  rescue Errno::EPIPE
    open_pipe
    retry
  ensure
    close_pipe
  end
end

def time(&blk)
  t0 = Time.now
  yield
  t1 = Time.now
  puts t1 - t0
end

file = 'e1.txt'

info, model = nil, nil

env = load_runtime_env("#{MEASURE_PATH}/wht_measure")
time {
time {
  puts "Loading ..."
  info, model = load_data(file, env)
}

time {
  puts "Generate ..."
  generate(model, SIZE)
}

time {
  puts "Transform ..."
  transform(model)
}

time {
  puts "Test ..."
  test(model)
}

time {
  puts "Model ..."
  ic(model)
}
puts "Total."
}

save_data(file, env, model, info)
