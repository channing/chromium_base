::call setup_env.bat
set PYTHONPATH=%PYTHONPATH%;%CD%\tools\grit
tools\gyp\gyp.bat -D library=shared_library -D component=static_library --depth . -Ibuild/common.gypi build/all.gyp
