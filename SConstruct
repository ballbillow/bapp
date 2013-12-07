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
env.Append(CCFLAGS = '-I/data/billowqiu/3rdlib/include')
env.Append(LIBPATH = '/data/billowqiu/3rdlib/lib')
env.Append(LIBS = ['boost_system','boost_thread', 'boost_filesystem', 'boost_program_options'])
#env.Append(LIBS = ['bnet'])

env.Library("bapp", Glob('*.cpp'))

#example
#env.Program("simple_app", Glob('example/*.cpp'), \
#             CCFLAGS = ['-I../', '-I' + os.environ['TSF4G'] +'/' + 'include'],
#             LIBPATH = ['-L./', os.environ['TSF4G'] + '/' + 'lib', '../bnet'],
#             LIBS = ['bapp', 'bnet', 'tdr', 'tdr_xml', 'tlog', 'tloghelp', 'tmng', 'scew', 'expat', 'pal', 'comm', 'boost_system','boost_thread', 'boost_filesystem', 'boost_program_options'])

