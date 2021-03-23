/*
 * systemdefile.h
 *
 *  Created on: 2017-12-9
 *      Author: Administrator
 */

#ifndef SYSTEMDEFILE_H_
#define SYSTEMDEFILE_H_
enum HmiStatus
{
	HMI_STATUS_DEFAULT = 0,			   //初始值
	HMI_STATUS_ENTER_BOOT_ANIMATION,   //IGN_ON -> 开机动画
	HMI_STATUS_ENTER_IGNON_ANIMATION,  //进入开机动画
	HMI_STATUS_EXIT_IGNON_ANIMATION,   //开机动画结束
	HMI_STATUS_EXIT_SELFCHECK,		   //开机动画结束并进入自检
	HMI_STATUS_ENTER_IGNOFF_ANIMATION, //自检结束并准备进入关机动画
	HMI_STATUS_ENTER_IGNOFF,		   //进入关机动画
	HMI_STATUS_OUT_SHUTDOWN_ANIMATION  //关机
};
enum Language
{
	LANGUAGE_DEFAULT,
	CHINIESE,
	ENGLISH
};

#endif /* SYSTEMDEFILE_H_ */
