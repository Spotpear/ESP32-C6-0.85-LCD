#pragma once

#include <Arduino.h>

extern volatile bool videoModeActive;

// 视频播放初始化（一次性，setup中调用）
void Video_Init(void);

// 切换到视频模式（打开第一个视频文件，注册按键中断）
void Video_Start(void);

// 退出视频模式（释放按键中断）
void Video_Stop(void);

// 视频播放循环（在loop中调用）
void Video_Play_Loop(void);

// 检查是否有视频文件
bool Video_CheckFiles(void);

// 检查视频系统是否已初始化
bool Video_IsInitialized(void);

bool Video_ShouldExit(void);

// 重置退出标志（切换回天气时钟后调用）
void Video_ResetExitFlag(void);
