# EvilsLive 
[![License MIT](https://img.shields.io/badge/license-MIT-brightgreen.svg?style=flat)](https://raw.githubusercontent.com/TinkKeep/evils/master/LICENSE)&nbsp;
[![Build Status](https://travis-ci.org/ThinkKeep/EvilsLive.svg?branch=master)](https://travis-ci.org/ThinkKeep/EvilsLive)

## 项目介绍
视频直播（目前只支持 Android）

**注意：**

>**目前此项目还在持续更新中....**


## 项目结构

## Snapshot

## Usage

**要求:**

Android Studio >= 2.2.3
api >= 19

**Step 1:**

在进行推流之前，你需要有流媒体服务（视频上传默认rtmp协议），本地搭建或搭建在公有云上，随你意。

例如：rtmp://127.0.0.1/live/hjd_phone
具体参照[ossrs/srs](https://github.com/ossrs/srs)搭建rtmp server

**Step 2:**

具体如何使用可以参照 [app](https://github.com/ThinkKeep/EvilsLive/tree/master/android/app) 中的代码

## TODO LIST
- [x] 推流(视频)功能 
- [ ] 推流(音频)功能
- [ ] 推流流程优化， 支持硬编、多种流媒体协议等
- [ ] 拉流功能
- [ ] 添加测试用例
- [ ] 机型适配

## 遗留问题
- [ ] 视频采集数据native层处理
- [ ] 支持多种体系结构

## Copyright and License
Copyright 2016-2017 ThinkKeep


Code released under the MIT License.
