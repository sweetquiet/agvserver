﻿#include "msgcenter.h"

#include "util/global.h"
#include "util/common.h"
#include "usermsgprocessor.h"
#include "agvpositionpublisher.h"
//宏定义一些

//包头
#define AGV_PACK_HEAD    0x55
//包尾
#define AGV_PACK_END   0xAA

//功能码: 手动模式
#define AGV_PACK_SEND_CODE_HAND_MODE    0x33
//功能码：自动模式
#define AGV_PACK_SEND_CODE_AUDTO_MODE   0x35
//功能码:升级
#define AGV_PACK_SEND_CODE_UPDATE_MODE  0x22

//功能码：接收Agv上报状态的
#define AGV_PACK_RECV_CODE_STATUS       0x44

//rfid是立即执行
#define AGV_PACK_SEND_RFID_CODE_IMMEDIATELY       0x00000000
#define AGV_PACK_SEND_RFID_CODE_ETERNITY          0xFFFFFFFF

//指令代码
#define AGV_PACK_SEND_INSTRUC_CODE_STOP      0x00
#define AGV_PACK_SEND_INSTRUC_CODE_FORWARD      0x01
#define AGV_PACK_SEND_INSTRUC_CODE_BACKWARD      0x02
#define AGV_PACK_SEND_INSTRUC_CODE_LEFT      0x03
#define AGV_PACK_SEND_INSTRUC_CODE_RIGHT      0x04
#define AGV_PACK_SEND_INSTRUC_CODE_MP3LEFT      0x05
#define AGV_PACK_SEND_INSTRUC_CODE_MP3RIGHT      0x06
#define AGV_PACK_SEND_INSTRUC_CODE_MP3VOLUME      0x07



const char CHAR_NULL = '\0';

MsgCenter::MsgCenter(QObject *parent) : QObject(parent),
    positionPublisher(NULL),
    statusPublisher(NULL)
{

}
MsgCenter::~MsgCenter()
{
    if(positionPublisher)delete positionPublisher;
    if(statusPublisher)delete statusPublisher;
}

bool MsgCenter::addAgvPostionSubscribe(int subscribe)
{
    if(!positionPublisher)return false;
    positionPublisher->addSubscribe(subscribe);
    return true;
}

bool MsgCenter::removeAgvPositionSubscribe(int subscribe)
{
    if(!positionPublisher)return false;
    positionPublisher->removeSubscribe(subscribe);
    return true;
}

bool MsgCenter::addAgvStatusSubscribe(int subscribe, int agvId)
{
    if(!statusPublisher)return false;
    statusPublisher->addSubscribe(subscribe,agvId);
    return true;
}

bool MsgCenter::removeAgvStatusSubscribe(int subscribe,int agvId)
{
    if(!statusPublisher)return false;
    statusPublisher->removeSubscribe(subscribe,agvId);
    return true;
}

void MsgCenter::init()
{
    //启动8个线程，同时处理来自client的消息。
    for(int i=0;i<8;++i){
        UserMsgProcessor *workerThread = new UserMsgProcessor(this);
        workerThread->start();
    }
    //启动订阅 小车状态信息的线程
    if(statusPublisher){
        delete statusPublisher;
        statusPublisher=NULL;
    }
    statusPublisher = new AgvStatusPublisher(this);
    statusPublisher->start();

    //启动订阅 小车位置信息的线程
    if(positionPublisher){
        delete positionPublisher;
        positionPublisher=NULL;
    }
    positionPublisher = new AgvPositionPublisher(this);
    positionPublisher->start();

    //启动订阅 日志
    if(logPublisher){
        delete logPublisher;
        logPublisher = NULL;
    }
    logPublisher = new AgvLogPublisher(this);
    logPublisher->start();

}

QByteArray MsgCenter::auto_instruct_wait(){
    QByteArray qba;
    qba.append(0xFF);
    qba.append(0xFF);
    qba.append(0xFF);
    qba.append(0xFF);
    qba.append(0xFF);
    return qba;
}

QByteArray MsgCenter::auto_instruct_stop(int rfid,int delay)
{
    QByteArray qba;
    qba.append(((rfid>>24) & 0xFF));
    qba.append(((rfid>>16) & 0xFF));
    qba.append(((rfid>>8) & 0xFF));
    qba.append(((rfid) & 0xFF));
    qba.append(((AGV_PACK_SEND_INSTRUC_CODE_STOP<<4)&0xF0)|(delay & 0x0F));
    return qba;
}

QByteArray MsgCenter::auto_instruct_forward(int rfid,int speed)
{
    QByteArray qba;
    qba.append(((rfid>>24) & 0xFF));
    qba.append(((rfid>>16) & 0xFF));
    qba.append(((rfid>>8) & 0xFF));
    qba.append(((rfid) & 0xFF));
    qba.append(((AGV_PACK_SEND_INSTRUC_CODE_FORWARD<<4)&0xF0)|(speed & 0x0F));
    return qba;
}

QByteArray MsgCenter::auto_instruct_backward(int rfid,int speed)
{
    QByteArray qba;
    qba.append(((rfid>>24) & 0xFF));
    qba.append(((rfid>>16) & 0xFF));
    qba.append(((rfid>>8) & 0xFF));
    qba.append(((rfid) & 0xFF));
    qba.append(((AGV_PACK_SEND_INSTRUC_CODE_BACKWARD<<4)&0xF0)|(speed & 0x0F));
    return qba;
}

QByteArray MsgCenter::auto_instruct_turnleft(int rfid,int angle)
{
    QByteArray qba;
    qba.append(((rfid>>24) & 0xFF));
    qba.append(((rfid>>16) & 0xFF));
    qba.append(((rfid>>8) & 0xFF));
    qba.append(((rfid) & 0xFF));
    qba.append(((AGV_PACK_SEND_INSTRUC_CODE_LEFT<<4)&0xF0)|(angle & 0x0F));
    return qba;
}

QByteArray MsgCenter::auto_instruct_turnright(int rfid,int speed)
{
    QByteArray qba;
    qba.append(((rfid>>24) & 0xFF));
    qba.append(((rfid>>16) & 0xFF));
    qba.append(((rfid>>8) & 0xFF));
    qba.append(((rfid) & 0xFF));
    qba.append(((AGV_PACK_SEND_INSTRUC_CODE_RIGHT<<4)&0xF0)|(speed & 0x0F));
    return qba;
}

QByteArray MsgCenter::auto_instruct_mp3_left(int rfid, int mp3Id)
{
    QByteArray qba;
    qba.append(((rfid>>24) & 0xFF));
    qba.append(((rfid>>16) & 0xFF));
    qba.append(((rfid>>8) & 0xFF));
    qba.append(((rfid) & 0xFF));
    qba.append(((AGV_PACK_SEND_INSTRUC_CODE_MP3LEFT<<4)&0xF0)|(mp3Id>>4 & 0x0F));
    return qba;
}

QByteArray MsgCenter::auto_instruct_mp3_right(int rfid,int mp3Id)
{
    QByteArray qba;
    qba.append(((rfid>>24) & 0xFF));
    qba.append(((rfid>>16) & 0xFF));
    qba.append(((rfid>>8) & 0xFF));
    qba.append(((rfid) & 0xFF));
    qba.append(((AGV_PACK_SEND_INSTRUC_CODE_MP3RIGHT<<4)&0xF0)|(mp3Id & 0x0F));
    return qba;
}

QByteArray MsgCenter::auto_instruct_mp3_volume(int rfid,int volume)
{
    QByteArray qba;
    qba.append(((rfid>>24) & 0xFF));
    qba.append(((rfid>>16) & 0xFF));
    qba.append(((rfid>>8) & 0xFF));
    qba.append(((rfid) & 0xFF));
    qba.append(((AGV_PACK_SEND_INSTRUC_CODE_MP3VOLUME<<4)&0xF0)|(volume&0x0F));
    return qba;
}

//将内容封包
//加入包头、(功能码)、包长、(内容)、校验和、包尾
QByteArray MsgCenter::packet(char code_mode,QByteArray content)
{
    //计算校验和
    unsigned char sum = checkSum((unsigned char *)content.data(),content.length());

    //组包//加入包头、功能码、内容、校验和、包尾
    QByteArray result;
    result.append(AGV_PACK_HEAD);//包头
    result.append(code_mode);//功能码

    //包长2Byte
    int contentLength = content.length();
    result.append((contentLength>>8) & 0xFF);
    result.append((contentLength) & 0xFF);

    result.append(content);//内容
    result.append(sum);//校验和
    result.append(AGV_PACK_END);//包尾

    return result;
}

bool MsgCenter::handControlCmd(int agvId,int agvHandType,int speed)
{
    if(!g_m_agvs.contains(agvId))return false;
    //组装一个手控的命令
    QByteArray content;
    content.append(0x33);//手控的功能码
    short baseSpeed = speed & 0xFFFF;
    short forwardSpeed = 0;
    short leftSpeed = 0;
    switch(agvHandType){
    case AGV_HAND_TYPE_STOP:
        break;
    case AGV_HAND_TYPE_FORWARD:
        forwardSpeed = baseSpeed;
        break;
    case AGV_HAND_TYPE_BACKWARD:
        forwardSpeed = -1*baseSpeed;
        break;
    case AGV_HAND_TYPE_TURNLEFT:
        leftSpeed = baseSpeed;
        break;
    case AGV_HAND_TYPE_TURNRIGHT:
        leftSpeed = -1 * baseSpeed;
        break;
    default:
        return false;
    }

    //前后方向 2Byte
    content.append((forwardSpeed>>8) &0xFF);
    content.append((forwardSpeed) &0xFF);

    //左右方向 2Byte
    content.append((leftSpeed>>8) &0xFF);
    content.append((leftSpeed) &0xFF);

    //附件命令 4Byte
    content.append(CHAR_NULL);
    content.append(CHAR_NULL);
    content.append(CHAR_NULL);
    content.append(CHAR_NULL);

    //灯带数据 1Byte
    content.append(CHAR_NULL);

    //控制交接 1Byte
    content.append(CHAR_NULL);

    //设备地址，指令发起者 2Byte
    content.append(CHAR_NULL);
    content.append(CHAR_NULL);

    //备用字节S32*4 = 16Byte
    for(int i=0;i<16;++i){
        content.append(CHAR_NULL);
    }

    QByteArray result = packet(AGV_PACK_SEND_CODE_HAND_MODE,content);


    //发送命令
    return g_m_agvs[agvId]->sendToAgv(result);
}

QByteArray MsgCenter::taskControlCmd(int agvId, bool changeDirect)
{
    //组装一个agv执行path的命令
    QByteArray content;

    g_m_agvs[agvId]->queueNumber++;
    g_m_agvs[agvId]->queueNumber = g_m_agvs[agvId]->queueNumber &0XFF;
    //队列编号 0-255循环使用
    content.append(g_m_agvs[agvId]->queueNumber);
    //接下来是五组指令
    int instrct_length = 0;
    //1.判断是否掉向
    //首先需要启动
    if(!changeDirect){
        //1.立即启动
        content.append(auto_instruct_forward(AGV_PACK_SEND_RFID_CODE_IMMEDIATELY,g_m_agvs[agvId]->speed()));
        instrct_length +=1;
    }
    //        else{
    //            //掉头行驶
    //            //TODO
    //            content.append(auto_instruct_turnleft(AGV_RFID_CODE_IMMEDIATELY,0x0f));
    //            content.append(auto_instruct_turnleft(AGV_RFID_CODE_IMMEDIATELY,0x00));
    //        }



    //然后对接下来的要执行的数量进行预判
    for(int i=0;i<g_m_agvs[agvId]->currentPath().length() && instrct_length <=5;++i){
        AgvLine *line = g_m_lines[g_m_agvs[agvId]->currentPath().at(i)];
        AgvStation *station = g_m_stations[line->endStation()];

        //加入一个命令
        content.append(auto_instruct_forward(station->rfid(),g_m_agvs[agvId]->speed()));
        instrct_length +=1;

    }

    //固定长度五组
    for(int i=instrct_length;i<5;++i){
        content.append(auto_instruct_wait());
    }

    //组包//加入包头、功能码、内容、校验和、包尾
    QByteArray result = packet(AGV_PACK_SEND_CODE_AUDTO_MODE,content);

    return result;
}
