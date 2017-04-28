package com.thinkkeep.videolib.util;

/**
 * 全局的常量定义
 * Created by jason on 17/4/13.
 */

public class Defines {
    public static final int DEFAULT_FRAME_RATE = 15;

    public enum EcameraFacing {
        CAMERA_FACING_FRONT(1),
        CAMERA_FACING_BACK(0),
        ;
        public final int index;

        EcameraFacing(int index) {
            this.index = index;
        }
    }
    public enum EencodeMethod {
        SOFTWARE_ENCODE,
        HARDWARE_ENCODE,
        ;
    }

    public enum EvideoResolution {
        E360P(480, 360),
        E640P(640, 480),
        E480P(858, 480),
        E540P(960, 540),
        E720P(1280, 720),
        ;
        private final int width;
        private final int height;

        public int getWidth() {
            return width;
        }

        public int getHeight() {
            return height;
        }

        EvideoResolution(int width, int height) {
            this.width = width;
            this.height = height;
        }
    }

}
