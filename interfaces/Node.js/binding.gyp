{
  "targets": [
    {
      "target_name": "qtac",
      "sources": [
        "src/addon.cc",
        "src/tacdev_wrapper.cc",
        "src/utils.cc"
      ],
      "include_dirs": [
        "<!@(node -p \"require('node-addon-api').include\")",
        "../C++/TACDev",
        "../../src/libraries/qcommon-console"
      ],
      "dependencies": [
        "<!(node -p \"require('node-addon-api').gyp\")"
      ],
      "defines": [ "NAPI_DISABLE_CPP_EXCEPTIONS" ],
      "cflags!": [ "-fno-exceptions" ],
      "cflags_cc!": [ "-fno-exceptions" ],
      "conditions": [
        ["OS=='win'", {
          "defines": [ "NAPI_VERSION=8" ],
          "msvs_settings": {
            "VCCLCompilerTool": {
              "ExceptionHandling": 1
            }
          }
        }],
        ["OS=='linux'", {
          "cflags": [ "-fexceptions" ],
          "cflags_cc": [ "-fexceptions" ]
        }]
      ]
    }
  ]
}
