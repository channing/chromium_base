{
  'targets': [
    {
      'includes': [
        '../views_exe.gypi',
      ],
      'target_name': 'mfc_test',
      'variables': {
        'use_of_mfc': 1,
        #'no_minmax': 0,
      },
      'sources': [
        'MfcChromiumMsgLoopTest.cpp',
        'MfcChromiumMsgLoopTest.h',
        'MfcChromiumMsgLoopTest.rc',
        'MfcChromiumMsgLoopTestDlg.cpp',
        'MfcChromiumMsgLoopTestDlg.h',
        'Resource.h',
        'stdafx.cpp',
        'stdafx.h',
        'targetver.h',
        'cef_thread.cc',
        'cef_thread.h',
        'mfc_process.cpp',
        'mfc_process.h',
      ],
      'msvs_settings': {
        'VCLinkerTool': {
          'SubSystem': '2',         # Set /SUBSYSTEM:WINDOWS
        },
        'VCCLCompilerTool': {
          'RuntimeTypeInfo': 'true',
          'WarningLevel': '3',
        },
      },      
      'msvs_precompiled_header': 'stdafx.h',
      'msvs_precompiled_source': 'stdafx.cpp',
    },
  ],
}
