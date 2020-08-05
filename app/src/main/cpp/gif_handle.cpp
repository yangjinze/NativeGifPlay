#include <jni.h>
#include <string>
#include <android/log.h>
#include <android/bitmap.h>
#include <pthread.h>
#include "gif/gif_lib.h"
#define  LOG_TAG    "yangGif"
#define  LOGD(...)  __android_log_print(ANDROID_LOG_DEBUG,LOG_TAG,__VA_ARGS__)
#define  argb(a,r,g,b) ( ((a) & 0xff) << 24 ) | ( ((b) & 0xff) << 16 ) | ( ((g) & 0xff) << 8 ) | ((r) & 0xff)

typedef struct GifBeen {
    //播放第几帧， 当前播放的帧数
    int currentFrame;
    //总共帧数
    int totalFrame;
    //每一帧之间延迟多久播放，单位毫秒，数组
    int *delays;
};


//绘制一张图片
void drawFrame(GifFileType *gifFileType, GifBeen *gifBeen, AndroidBitmapInfo info, void *pixels) {
    //  当前帧
    SavedImage savedImage = gifFileType->SavedImages[gifBeen->currentFrame];
    //整幅图片的首地址
    int* px = (int *)pixels;
    int  pointPixel;
    GifImageDesc frameInfo = savedImage.ImageDesc;
    GifByteType  gifByteType;//压缩数据
//    rgb数据     压缩工具
    ColorMapObject* colorMapObject=frameInfo.ColorMap;
//    Bitmap 往下编译    info.stride一行像素个数
    px = (int *) ((char*)px + info.stride * frameInfo.Top);
    //    每一行的首地址
    int *line;
    for (int y = frameInfo.Top; y <frameInfo.Top+frameInfo.Height ; ++y) {
        line=px;
        for (int x = frameInfo.Left; x <frameInfo.Left + frameInfo.Width ; ++x) {
//            拿到每一个坐标的位置  索引    ---》  数据
            pointPixel=  (y-frameInfo.Top)*frameInfo.Width+(x-frameInfo.Left);
//            索引   rgb   LZW压缩  字典   （）缓存在一个字典
//解压
            gifByteType= savedImage.RasterBits[pointPixel];
            GifColorType gifColorType=colorMapObject->Colors[gifByteType];
            line[x]=argb(255,gifColorType.Red, gifColorType.Green, gifColorType.Blue);
        }
        px = (int *) ((char*)px + info.stride);
    }
}

extern "C"
JNIEXPORT jlong JNICALL
Java_com_yang_nativegifplay_GifPlay_loadGif(JNIEnv *env, jobject thiz, jstring file_path) {
    LOGD("loadGif");
    const char *filePath = env->GetStringUTFChars(file_path, 0);
    int error;
    GifFileType *gifFileType = DGifOpenFileName(filePath, &error); //TODO 没声明读写权限，返回101
    LOGD("loadGif 11111111111 %d", error);
    DGifSlurp(gifFileType);  //解析gif文件相关信息,文件大的话很费时
    //用来存储gif当前播放的相关信息
    GifBeen *gifBeen = (GifBeen*) malloc(sizeof(GifBeen));
    //清空内存段
    memset(gifBeen, 0, sizeof(GifBeen));
    //建立2者联系
    gifFileType->UserData = gifBeen;
    //初始化数组
    gifBeen->delays = (int*)malloc(sizeof(int) * gifFileType->ImageCount);
    memset(gifBeen->delays, 0, sizeof(int) * gifFileType->ImageCount);
    gifBeen->currentFrame = 0;
    gifBeen->totalFrame = gifFileType->ImageCount;
    //读取扩展快，获取各帧的延迟时间

    LOGD("gif总帧数 %d", gifFileType->ImageCount);
    //TODO
    ExtensionBlock* ext;
    for (int i = 0; i < gifFileType->ImageCount; i++) {
        SavedImage frame = gifFileType->SavedImages[i];
        for (int j = 0; j < frame.ExtensionBlockCount; j++) {
            if (frame.ExtensionBlocks[j].Function == GRAPHICS_EXT_FUNC_CODE) {
                ext = &frame.ExtensionBlocks[j];
                break;
            }
        }
        if (ext) {
            //单位1/100秒  //TODO 转毫秒是*10?
            int  frameDelay = 10*(ext->Bytes[1] | (ext -> Bytes[2] << 8));
            LOGD("%d时间 %d", i, frameDelay);
            gifBeen->delays[i] = frameDelay;
        }
    }

    env->ReleaseStringUTFChars(file_path, filePath);
    return (jlong)gifFileType;
}

extern "C"
JNIEXPORT jint JNICALL
Java_com_yang_nativegifplay_GifPlay_getWidth(JNIEnv *env, jobject thiz, jlong ndk_gif) {
    LOGD("getWidth");
    GifFileType *gifFileType = (GifFileType*)ndk_gif;
    return gifFileType->SWidth;
}

extern "C"
JNIEXPORT jint JNICALL
Java_com_yang_nativegifplay_GifPlay_getHeight(JNIEnv *env, jobject thiz, jlong ndk_gif) {
    LOGD("getHeight");
    GifFileType *gifFileType = (GifFileType*)ndk_gif;
    return gifFileType->SHeight;
}

extern "C"
JNIEXPORT jint JNICALL
Java_com_yang_nativegifplay_GifPlay_updateFrame(JNIEnv *env, jobject thiz, jlong ndk_gif,
                                                jobject bitmap) {
    LOGD("updateFrame");
    GifFileType *gifFileType = (GifFileType*)ndk_gif;
    GifBeen *gifBeen = (GifBeen*)(gifFileType->UserData);

    AndroidBitmapInfo info;
    AndroidBitmap_getInfo(env, bitmap, &info);
    void* pixels;
    AndroidBitmap_lockPixels(env, bitmap, &pixels);
    drawFrame(gifFileType, gifBeen, info, pixels);
    gifBeen->currentFrame +=1;
    if (gifBeen->currentFrame >= gifBeen->totalFrame - 1) {
        gifBeen->currentFrame = 0;
        LOGD("重新过来  %d  ",gifBeen->currentFrame);
    }
    AndroidBitmap_unlockPixels(env, bitmap);
    return gifBeen->delays[gifBeen->currentFrame];
}