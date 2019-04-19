# AudioCapture

以windows的core audio apis来采集声卡声音

## 调试

1.播放声音时点击play按钮
1.1s后点击save按钮，生成a.pcm
1.敲击命令播放a.pcm文件：ffplay.exe -ar 48000 -ac 2 -f f32le -i /d/a.pcm

## 待解决

以多线程来录音
