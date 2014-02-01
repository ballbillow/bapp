import os

debug = ARGUMENTS.get('debug', 0)
env = Environment(ENV=os.environ)
if int(debug):
    env.Append(CCFLAGS = ['-ggdb3', '-D DEBUG'])
else:
    env.Append(CCFLAGS = ['-O3', '-DNDEBUG'])
#lib
env.Append(CCFLAGS = ['-Wall', '-Bstatic', '-I.'])
env.Append(CCFLAGS = '-I../')
env.Append(LIBS = ['boost_system','boost_thread', 'boost_filesystem', 'boost_program_options'])
#env.Append(LIBS = ['bnet'])

env.Library("bapp", Glob('*.cpp'))

