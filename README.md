# VVAppTrace
VVAppTrace 跟踪ios方法执行性能工具

## 介绍
1. 通过pod集成到项目中 `pod 'VVAppTrace'`
1. 运行App后，在沙盒中导出apptrace文件夹，拷贝merge.py文件到apptrace同目录，执行命令`python merge.py -d apptrace` apptrace文件夹下生成trace.json
1. 在Chrome中执行`chrome://tracing` 然后将`trace.json`拖入Chrome中即可看到方法执行的视图

## 感谢
1. HookZz : https://github.com/jmpews/HookZz
1. catapult : https://github.com/catapult-project/catapult
1. AppleTrace : https://github.com/everettjf/AppleTrace
1. fishhook : https://github.com/facebook/fishhook
1. AppTrace :https://github.com/chenzhengxu/AppTrace
