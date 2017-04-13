package com.thinkkeep.videolib.util;

/**
 * 全局的常量定义
 * Created by jason on 17/4/13.
 */

public class Defines {
    public enum EENCODE_METHOD {
        SOFTWARE_ENCODE,
        HARDWARE_ENCODE,
        ;
    }

    public enum EVIDEO_RESOLUTION {
        E360P(480, 360),
        E640P(640, 480),
        E480P(858, 480),
        E540P(960, 540),
        E720P(1080, 720),
        ;
        private final int width;
        private final int height;

        public int getWidth() {
            return width;
        }

        public int getHeight() {
            return height;
        }

        EVIDEO_RESOLUTION(int width, int height) {
            this.width = width;
            this.height = height;
        }
    }

}
