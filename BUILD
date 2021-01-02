cc_library(
  name = 'haxecpp',
  visibility = ['//visibility:public'],
  deps = ["//extern/hxcpp:hxcpp"],
)

cc_library(
  name = 'main_hook',
  visibility = ['//visibility:public'],
  strip_include_prefix = 'src/',
  include_prefix = 'haxecpp',
  hdrs = [
    'src/main_hook.h',
  ],
  deps = ["//extern/hxcpp:hxcpp"],
)

cc_library(
  name = 'haxe_thread',
  visibility = ['//visibility:public'],
  strip_include_prefix = 'src/',
  include_prefix = 'haxecpp',
  deps = [
    ':main_hook',
  ],
  hdrs = [
    'src/haxe_thread.h',
  ],
  srcs = [
    'src/haxe_thread.cpp',
    'src/haxe_thread.h',
  ]
)
