{
  'targets': [
    {
      'target_name': 'srv_api-native',
      'sources': [ 'src/srv_api.cc' ],
      'include_dirs': ["<!@(node -p \"require('node-addon-api').include\")", '/root/srv_api/include'],
      'libraries': ['/root/srv_api/hiredis/libhiredis.a'],
      'dependencies': ["<!(node -p \"require('node-addon-api').gyp\")"],
      'cflags!': [ '-fno-exceptions' ],
      'cflags_cc!': [ '-fno-exceptions' ],
      'xcode_settings': {
        'GCC_ENABLE_CPP_EXCEPTIONS': 'YES',
        'CLANG_CXX_LIBRARY': 'libc++',
        'MACOSX_DEPLOYMENT_TARGET': '10.7'
      },
      'msvs_settings': {
        'VCCLCompilerTool': { 'ExceptionHandling': 1 },
      }
    }
  ]
}
