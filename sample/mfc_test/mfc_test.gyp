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
      'dependencies': [
        '../views.gyp:locales',
      ],
      'include_dirs': [
        '.',
        '../..',
      ],
      'sources': [
        'MfcChromiumMsgLoopTest.cpp',
        'MfcChromiumMsgLoopTest.h',
        'MfcChromiumMsgLoopTest.rc',
        'MfcChromiumMsgLoopTestDlg.cpp',
        'MfcChromiumMsgLoopTestDlg.h',
        'Resource.h',
        'cef_thread.cc',
        'cef_thread.h',
        'menu/menu_controller.cpp',
        'menu/menu_controller.h',
        'menu/menu_host.cpp',
        'menu/menu_host.h',
        'menu/menu_host_root_view.cpp',
        'menu/menu_host_root_view.h',
        'menu/menu_item_model.cpp',
        'menu/menu_item_model.h',
        'menu/menu_item_view.cpp',
        'menu/menu_item_view.h',
        'menu/menu_item_view_impl.cpp',
        'menu/menu_item_view_impl.h',
        'menu/menu_runner.cpp',
        'menu/menu_runner.h',
        'menu/menu_scroll_view_container.cpp',
        'menu/menu_scroll_view_container.h',
        'menu/menu_test.cpp',
        'menu/menu_test.h',
        'menu/submenu_view.cpp',
        'menu/submenu_view.h',
        'mfc_process.cpp',
        'mfc_process.h',
        'stdafx.cpp',
        'stdafx.h',
        'targetver.h',
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
